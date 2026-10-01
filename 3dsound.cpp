 














#include "std.hpp"
#include "fixed.h"
#include "3dsound.hpp"
#include "events.hpp"
#include "kaos.hpp"
#include "objects.hpp"
#include "3dengine.hpp"
#include "mm4.hpp"
#include "kaos_sdl.hpp"

#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <chrono>
#include <mutex>
#include <string>
#include <vector>

struct SoundData {
    int memidx = -1;
    word size = 0;
    std::vector<std::int8_t> samples;
    int sample_rate = 11025;
};

#pragma pack(push, 2)
struct TSound {
     
     
    std::int16_t id = -1;
    std::int16_t ownerid = static_cast<std::int16_t>(SYSOWN);
    std::int16_t sndnum = -1;        
    std::uint16_t size = 0;          
    std::uint16_t curpos = 0;        
    std::int32_t x = 0, y = 0;    
    byte lvol = 0, rvol = 0;
    char desync = 0;
    byte flags = 0;
    char fadespeed = 0;
    byte fadecount = 0;
};
#pragma pack(pop)

static_assert(sizeof(TSound) == 24,
              "KAOS TSound must retain the original 24-byte layout");
static_assert(offsetof(TSound, id) == 0 &&
              offsetof(TSound, ownerid) == 2 &&
              offsetof(TSound, sndnum) == 4 &&
              offsetof(TSound, size) == 6 &&
              offsetof(TSound, curpos) == 8 &&
              offsetof(TSound, x) == 10 &&
              offsetof(TSound, y) == 14 &&
              offsetof(TSound, lvol) == 18 &&
              offsetof(TSound, rvol) == 19 &&
              offsetof(TSound, desync) == 20 &&
              offsetof(TSound, flags) == 21 &&
              offsetof(TSound, fadespeed) == 22 &&
              offsetof(TSound, fadecount) == 23,
              "KAOS TSound field offsets must match the original 16-bit layout");

static constexpr int OUTPUT_RATE = 44100;
static constexpr int MIX_SOURCE_FRAMES = 1024;
static constexpr int MIX_OUTPUT_FRAMES = 4096;  
static constexpr int MAX_QUEUED_OUTPUT_FRAMES = MIX_OUTPUT_FRAMES * 2;
static constexpr std::uint64_t MIX_SOURCE_RATE = 11025ull;

 
static int bufferready = 0;
static std::uint64_t dma_sample_time_accum = 0;
static std::chrono::steady_clock::time_point dma_last_time{};
static bool dma_clock_running = false;
static constexpr std::uint64_t DMA_HALF_TIME_DEN = 1024ull * 1000000000ull;
static unsigned long sound_mix_seq = 0;

int sm_soundnum = 0;
char sm_lrinverted = 0;
char sm_soundok = 0;
int sm_soundvol = SM_NORMVOL;

static int idcnt = 0;
static std::vector<SoundData> sounddata(MAXSNDDATA);
static TSound soundlist[MAXSOUNDS];
static SDL_AudioStream *audio_stream = nullptr;
static std::mutex sound_mutex;

static int queued_audio_bytes()
{
    if (!audio_stream)
        return 0;
    const int bytes = SDL_GetAudioStreamQueued(audio_stream);
    return bytes < 0 ? 0 : bytes;
}

static int sample_at(const SoundData &snd, int index)
{
    if (snd.samples.empty())
        return 0;
    if (index < 0 || index >= static_cast<int>(snd.samples.size()))
        return 0;
    return static_cast<int>(snd.samples[static_cast<std::size_t>(index)]);
}

static void mix_sample(float *output, int frame, int sample,
                       int lvol, int rvol)
{
    const float s = static_cast<float>(sample) / 128.0f;
    output[frame * 2] += s * (static_cast<float>(lvol) / 32.0f);
    output[frame * 2 + 1] += s * (static_cast<float>(rvol) / 32.0f);
}

 



static bool mix_sound_chunk(TSound &sound, const SoundData &snd,
                            int lvol, int rvol, char desync,
                            float *output)
{
    if (snd.samples.empty())
        return false;

    int size = static_cast<int>(sound.size);
    if (size <= 0)
        size = static_cast<int>(snd.samples.size());
    if (size <= 0)
        return false;
    if (size > static_cast<int>(snd.samples.size()))
        size = static_cast<int>(snd.samples.size());

    int lpos = static_cast<int>(sound.curpos);
    int rpos = lpos;
    const bool ldrive = desync < 0;

    if (ldrive) {
        rpos += desync;
        if (rpos < 0 && (sound.flags & SFL_CONTINUE))
            rpos += size;
    } else {
        lpos -= desync;
        if (lpos < 0 && (sound.flags & SFL_CONTINUE))
            lpos += size;
    }

    bool ended = false;

    for (int frame = 0; frame < MIX_OUTPUT_FRAMES; ++frame) {
         
        const int source_step = frame >> 2;
        int li = lpos + source_step;
        int ri = rpos + source_step;

        if (sound.flags & SFL_CONTINUE) {
            li %= size;
            ri %= size;
            if (li < 0) li += size;
            if (ri < 0) ri += size;
        }

        if (li >= 0 && li < size)
            mix_sample(output, frame, sample_at(snd, li), lvol, 0);
        if (ri >= 0 && ri < size)
            mix_sample(output, frame, sample_at(snd, ri), 0, rvol);

         
        const int primary = ldrive ? li : ri;
        if (!(sound.flags & SFL_CONTINUE) && primary >= size) {
            ended = true;
            break;
        }
    }

    if (sound.flags & SFL_CONTINUE) {
        sound.curpos = static_cast<std::uint16_t>(
            (static_cast<std::uint32_t>(sound.curpos) + MIX_SOURCE_FRAMES) %
            static_cast<std::uint32_t>(size));
    } else {
        const std::uint32_t next = static_cast<std::uint32_t>(sound.curpos) +
                                   MIX_SOURCE_FRAMES;
        sound.curpos = static_cast<std::uint16_t>(next);
    }

    return !ended;
}

static bool mix_sound_chunk_mono_or_stereo(TSound &sound,
                                           const SoundData &snd,
                                           int lvol, int rvol,
                                           float *output)
{
    if (snd.samples.empty())
        return false;

    int size = static_cast<int>(sound.size);
    if (size <= 0)
        size = static_cast<int>(snd.samples.size());
    if (size > static_cast<int>(snd.samples.size()))
        size = static_cast<int>(snd.samples.size());
    const std::uint32_t start = sound.curpos;

    for (int frame = 0; frame < MIX_OUTPUT_FRAMES; ++frame) {
        const int source = static_cast<int>(start + (frame >> 2));
        if (source >= size) {
            if (!(sound.flags & SFL_CONTINUE))
                break;
            const int wrapped = source % size;
            mix_sample(output, frame, sample_at(snd, wrapped), lvol, rvol);
        } else {
            mix_sample(output, frame, sample_at(snd, source), lvol, rvol);
        }
    }

    const std::uint32_t next = start + MIX_SOURCE_FRAMES;
    if (sound.flags & SFL_CONTINUE)
        sound.curpos = static_cast<std::uint16_t>(next % static_cast<std::uint32_t>(size));
    else {
        sound.curpos = static_cast<std::uint16_t>(next);
        if (next >= static_cast<std::uint32_t>(size))
            return false;
    }
    return true;
}

static void delsound_locked(int index)
{
    TSound &sound = soundlist[index];
    const int ended_id = sound.id;
    const int ended_owner = sound.ownerid;

    if (ended_owner != SYSOWN)
        eventmanager.senddirect(EV_ENDSOUND, ended_id,
                                ended_owner, 0, 0);
    else
        eventmanager.sendshort(EV_ENDSOUND, ended_id, SYSOWN, 0, 0);

    soundlist[index] = soundlist[--sm_soundnum];
    soundlist[sm_soundnum] = TSound{};
}

static int getownsound(int owner, char all)
{
    if (owner == SYSOWN)
        return -1;

    for (int i = 0; i < sm_soundnum; ++i) {
        if (soundlist[i].ownerid == owner &&
            (all || (soundlist[i].flags & SFL_OVERSOUND)))
            return i;
    }

    return -1;
}

static bool sounddata_prepare(int sndnum)
{
    if (sndnum < 0 || sndnum >= MAXSNDDATA)
        return false;

    SoundData &snd = sounddata[sndnum];
    if (!snd.samples.empty())
        return true;

#ifdef __DATAFILE__
    if (snd.memidx < 0 || snd.size == 0)
        return false;

    void *raw = mm_recall(snd.memidx);
    if (!raw)
        return false;

    snd.samples.resize(snd.size);
    std::memcpy(snd.samples.data(), raw, snd.size);
    snd.sample_rate = 11025;

     
    mm_unload(snd.memidx);
    return true;
#else
    return false;
#endif
}

void loadsound(int sndnum, const char *wavname)
{
    if (sndnum < 0 || sndnum >= MAXSNDDATA)
        return;

    SoundData &snd = sounddata[sndnum];

#ifdef __DATAFILE__
    if (snd.memidx >= 0)
        error("loadsound", "multiple assign");

    (void)wavname;
    snd.memidx = mm_alloc(0);
    snd.size = mm_getblocksize(snd.memidx);
    mm_unload(snd.memidx);
    return;
#else
    if (!wavname)
        return;

    SDL_AudioSpec spec{};
    Uint8 *data = nullptr;
    Uint32 length = 0;

    if (!SDL_LoadWAV(wavname, &spec, &data, &length)) {
        error("loadsound", wavname);
        return;
    }

    snd.samples.clear();
    snd.sample_rate = spec.freq;

    if (spec.format == SDL_AUDIO_U8 && spec.channels == 1) {
        snd.samples.resize(length);
        for (Uint32 i = 0; i < length; ++i)
            snd.samples[i] = static_cast<std::int8_t>(
                static_cast<int>(data[i]) - 128);
    } else {
        const int bytes_per_sample =
            SDL_AUDIO_BYTESIZE(spec.format) * spec.channels;
        if (bytes_per_sample <= 0) {
            SDL_free(data);
            return;
        }

        const std::size_t frames =
            length / static_cast<Uint32>(bytes_per_sample);
        snd.samples.resize(frames);

        if (spec.format == SDL_AUDIO_S16LE && spec.channels == 1) {
            const auto *src = reinterpret_cast<const std::int16_t *>(data);
            for (std::size_t i = 0; i < frames; ++i)
                snd.samples[i] = static_cast<std::int8_t>(src[i] >> 8);
        } else {
            for (std::size_t i = 0; i < frames; ++i) {
                const byte u = data[i * bytes_per_sample];
                snd.samples[i] = static_cast<std::int8_t>(
                    static_cast<int>(u) - 128);
            }
        }
    }

    SDL_free(data);
#endif
}

int play3Dsound(fixed x, fixed y, int sndnum, byte vol,
                int owner, byte flags)
{
    if (!sm_soundok || !sounddata_prepare(sndnum))
        return -1;

    std::lock_guard<std::mutex> lock(sound_mutex);

    int index = getownsound(owner, 0);
    if (sm_soundnum >= MAXSOUNDS && index < 0)
        return -1;

    if (index < 0 || !(flags & SFL_OVERSOUND))
        index = sm_soundnum;

    TSound &sound = soundlist[index];
    sound = TSound{};
    sound.id = static_cast<std::int16_t>(idcnt);
    idcnt = (idcnt + 1) & 0x7fff;
    sound.ownerid = static_cast<std::int16_t>(owner);
    sound.sndnum = static_cast<std::int16_t>(sndnum);
    sound.size = static_cast<std::uint16_t>(sounddata[sndnum].samples.size());
    sound.curpos = 0;
    sound.x = x;
    sound.y = y;
    sound.flags = flags;
    sound.desync = 0;
    sound.rvol = sound.lvol = vol ? vol : SM_MAXVOL;

    if (flags & SFL_FADE) {
        sound.fadespeed = 1;
        sound.fadecount = 0;
    }

    if (index == sm_soundnum)
        ++sm_soundnum;

    return sound.id;
}

int playsound(int sndnum, byte lvol, byte rvol,
              char desync, int owner, byte flags)
{
    if (!sm_soundok || !sounddata_prepare(sndnum))
        return -1;

    std::lock_guard<std::mutex> lock(sound_mutex);

    int index = getownsound(owner, 0);
    if (sm_soundnum >= MAXSOUNDS && index < 0)
        return -1;

    if (index < 0 || !(flags & SFL_OVERSOUND))
        index = sm_soundnum;

    TSound &sound = soundlist[index];
    sound = TSound{};
    sound.id = static_cast<std::int16_t>(idcnt);
    idcnt = (idcnt + 1) & 0x7fff;
    sound.ownerid = static_cast<std::int16_t>(owner);
    sound.sndnum = static_cast<std::int16_t>(sndnum);
    sound.size = static_cast<std::uint16_t>(sounddata[sndnum].samples.size());
    sound.curpos = 0;
    sound.flags = flags;
    sound.desync = desync;
    if (lvol) {
        sound.lvol = lvol;
        sound.rvol = rvol;
    } else {
        sound.rvol = sound.lvol = SM_MAXVOL;
    }

    if (flags & SFL_FADE) {
        sound.fadespeed = 1;
        sound.fadecount = 0;
    }

    if (index == sm_soundnum)
        ++sm_soundnum;

    return sound.id;
}

static void dma_update()
{
    if (!dma_clock_running)
        return;

    const auto now = std::chrono::steady_clock::now();
    const auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        now - dma_last_time).count();
    dma_last_time = now;
    if (elapsed_ns <= 0)
        return;

    dma_sample_time_accum += static_cast<std::uint64_t>(elapsed_ns) *
                             MIX_SOURCE_RATE;
    while (bufferready > 0 && dma_sample_time_accum >= DMA_HALF_TIME_DEN) {
        dma_sample_time_accum -= DMA_HALF_TIME_DEN;
        --bufferready;
    }

    if (bufferready <= 0) {
        dma_sample_time_accum = 0;
        dma_clock_running = false;
    }
}

void soundmanager(fixed cx, fixed cy, int angle)
{
    if (!sm_soundok)
        return;

    std::lock_guard<std::mutex> lock(sound_mutex);

    dma_update();

    if (!sm_soundnum || bufferready >= 2)
        return;

    const int chunk_bytes = MIX_OUTPUT_FRAMES *
                            static_cast<int>(sizeof(float) * 2);

    std::vector<float> output(static_cast<std::size_t>(MIX_OUTPUT_FRAMES) * 2,
                              0.0f);

    const fixed viewcos = costab[angle];
    const fixed viewsin = sintab[angle];

    for (int i = 0; i < sm_soundnum; ++i) {
        TSound &sound = soundlist[i];
        const SoundData &snd = sounddata[sound.sndnum];
        if (snd.samples.empty()) {
            delsound_locked(i--);
            continue;
        }

        int lv, rv;
        if (sound.flags & SFL_FADE) {
            if (sound.fadespeed > 0) {
                if (sound.fadecount < sound.lvol) {
                    int fc = sound.fadecount + 2;
                    if (fc > sound.lvol) fc = sound.lvol;
                    sound.fadecount = static_cast<byte>(fc);
                } else {
                    sound.fadespeed = 0;
                }
            } else if (sound.fadespeed < 0) {
                if (sound.fadecount > 0) {
                    if (sound.fadecount < 2)
                        sound.fadecount = 0;
                    else
                        sound.fadecount = static_cast<byte>(sound.fadecount - 2);
                } else {
                    delsound_locked(i--);
                    continue;
                }
            }
            lv = rv = sound.fadecount;
        } else {
            lv = sound.lvol;
            rv = sound.rvol;
        }

        char desync = 0;
        if ((sound.flags & SFL_3D) && cx >= 0) {
            fixed x = sound.x;
            fixed y = sound.y;

            if (sound.flags & SFL_FOLLOW) {
                if (Object *obj = objectslist.get(sound.ownerid)) {
                    x += obj->mover->x;
                    y += obj->mover->y;
                }
            }

            int lk = lshr16(fixmul(x - cx, viewcos) +
                            fixmul(y - cy, viewsin)) >> 4;
            int rk = lshr16(fixmul(y - cy, viewcos) -
                            fixmul(x - cx, viewsin)) >> 4;

            int vdl = intdist(lk, rk + 1);
            int vdr = intdist(lk, rk - 1);
            if (rk > 1) vdl += rk - 1;
            if (rk < -1) vdr -= rk + 1;
            if (lk < 0) { vdl += 4; vdr += 4; }

            lv -= vdl;
            rv -= vdr;

            if (sound.flags & SFL_RAY) {
                const int obstacles =
                    lookray(x, y, cx, cy, OMM_SOUNDABLE) << 2;
                lv -= obstacles;
                rv -= obstacles;
            }

            if (lv < 0) lv = 0;
            if (rv < 0) rv = 0;

            if (!lv && !rv) {
                if (sound.flags & SFL_CONTINUE) {
                    const std::uint32_t sz = sound.size ? sound.size
                        : static_cast<std::uint32_t>(snd.samples.size());
                    if (sz) {
                        std::uint16_t next = static_cast<std::uint16_t>(
                            static_cast<std::uint32_t>(sound.curpos) +
                            MIX_SOURCE_FRAMES);
                        if (next >= sz)
                            next = static_cast<std::uint16_t>(next - sz);
                        sound.curpos = next;
                    }
                } else {
                    delsound_locked(i--);
                }
                continue;
            }

            int dl = lk, dr = rk;
            while (std::abs(dl) > 8 || std::abs(dr) > 8) {
                dl >>= 1;
                dr >>= 1;
            }

            static const char synctab[17][9] = {
                {0,0,1,2,3,4,5,5,5},
                {0,0,1,2,3,4,5,5,6},
                {0,0,2,3,4,5,5,6,7},
                {0,0,3,4,4,5,5,6,7},
                {0,1,3,4,5,6,6,7,8},
                {0,1,4,5,6,7,7,9,9},
                {0,2,5,8,9,9,9,9,9},
                {0,5,8,9,9,10,10,10,10},
                {0,10,10,10,10,10,10,10,10},
                {0,60,8,10,10,10,10,10,10},
                {0,80,50,20,15,15,12,12,12},
                {0,100,70,50,30,20,15,15,15},
                {0,110,90,60,50,30,25,20,20},
                {0,115,100,70,55,50,40,30,25},
                {0,120,110,80,65,55,50,40,40},
                {0,127,120,90,70,60,55,50,45},
                {0,127,127,100,80,65,60,55,50}
            };

            if (dr >= 0)
                desync = synctab[8 - dl][min(dr, 8)];
            else
                desync = static_cast<char>(-synctab[8 - dl][min(-dr, 8)]);
        } else {
            desync = sound.desync;
        }

        if (sm_lrinverted) {
            std::swap(lv, rv);
            desync = -desync;
        }

        lv = lv * sm_soundvol >> SM_VOLSHIFT;
        rv = rv * sm_soundvol >> SM_VOLSHIFT;

        bool alive;
        if (desync) {
            alive = mix_sound_chunk(sound, snd, lv, rv, desync, output.data());
        } else {
            alive = mix_sound_chunk_mono_or_stereo(sound, snd, lv, rv,
                                                    output.data());
        }

        if (!alive) {
            delsound_locked(i--);
            continue;
        }
    }

    for (float &v : output)
        v = std::clamp(v, -1.0f, 1.0f);

    const bool start_dma = (bufferready == 0);
    ++bufferready;
    ++sound_mix_seq;

    if (start_dma) {
        dma_sample_time_accum = 0;
        dma_last_time = std::chrono::steady_clock::now();
        dma_clock_running = true;
    }

    if (audio_stream &&
        queued_audio_bytes() <
            MAX_QUEUED_OUTPUT_FRAMES * static_cast<int>(sizeof(float) * 2)) {
        (void)SDL_PutAudioStreamData(audio_stream, output.data(), chunk_bytes);
    }
}

void ownerdead(int owner)
{
    std::lock_guard<std::mutex> lock(sound_mutex);

    int index;
    while ((index = getownsound(owner, 1)) >= 0) {
        if (soundlist[index].flags & SFL_AUTOSTOP)
            delsound_locked(index);
        else if (soundlist[index].flags & SFL_CONTINUE)
            soundlist[index].flags &= static_cast<byte>(~SFL_CONTINUE);
        else
            break;
    }
}

char stopsound(int id)
{
    std::lock_guard<std::mutex> lock(sound_mutex);

    for (int i = 0; i < sm_soundnum; ++i) {
        if (soundlist[i].id != id)
            continue;

        if (soundlist[i].flags & SFL_FADE) {
            soundlist[i].fadespeed = -1;
            return 2;
        }

        delsound_locked(i);
        return 1;
    }

    return 0;
}

void stopallsounds()
{
    std::lock_guard<std::mutex> lock(sound_mutex);
    int k = 0;
    const int end = sm_soundnum;
    for (int i = 0; i < end; i++) {
        TSound &sound = soundlist[k];
        if (sound.flags & SFL_FADE) {
            sound.fadespeed = -1;
            k++;
        } else {
            delsound_locked(k);
        }
    }
}

void initsounds()
{
    {
        std::lock_guard<std::mutex> lock(sound_mutex);

        sm_soundok = 0;
        sm_soundnum = 0;
        idcnt = 0;
        bufferready = 0;
        dma_sample_time_accum = 0;
        dma_last_time = {};
        dma_clock_running = false;
        sound_mix_seq = 0;
        for (SoundData &snd : sounddata) {
            snd.memidx = -1;
            snd.size = 0;
            snd.samples.clear();
            snd.sample_rate = 11025;
        }

    if (!kaos_sdl_audio_init())
        return;

    const SDL_AudioSpec spec = { SDL_AUDIO_F32, 2, OUTPUT_RATE };
    audio_stream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
        &spec, nullptr, nullptr);

    if (audio_stream) {
        if (!SDL_ResumeAudioStreamDevice(audio_stream)) {
            SDL_DestroyAudioStream(audio_stream);
            audio_stream = nullptr;
        }
    }

     
        sm_lrinverted = 0;
        sm_soundok = 1;
    }

}

void donesounds()
{
    std::lock_guard<std::mutex> lock(sound_mutex);

    sm_soundok = 0;
    sm_soundnum = 0;
    bufferready = 0;
    dma_sample_time_accum = 0;
    dma_last_time = {};
    dma_clock_running = false;

    if (audio_stream) {
        SDL_DestroyAudioStream(audio_stream);
        audio_stream = nullptr;
    }
}
