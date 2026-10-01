#include "std.hpp"
#include "vgatool3.hpp"
#if defined(KAOS_EDITOR) && defined(_WIN32)
#include "win32_editor_platform.hpp"
#else
#include "kaos_sdl.hpp"
#endif
#include "lgraph.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <chrono>
#include <thread>

namespace {

 









using steady_clock = std::chrono::steady_clock;

constexpr std::uint64_t VGA_CLOCK_HZ = 25175000ull;
constexpr std::uint64_t VGA_TOTAL_CLOCKS = 800ull * 449ull;
constexpr std::uint64_t VGA_PERIOD_NUM_NS = VGA_TOTAL_CLOCKS * 1000000000ull;
constexpr std::uint64_t VGA_PERIOD_DEN = VGA_CLOCK_HZ;
constexpr std::uint64_t VGA_PERIOD_NS = VGA_PERIOD_NUM_NS / VGA_PERIOD_DEN;
constexpr std::uint64_t VGA_PERIOD_REM = VGA_PERIOD_NUM_NS % VGA_PERIOD_DEN;

steady_clock::time_point g_vblank_next{};
std::uint64_t g_vblank_rem = 0;
bool g_vblank_started = false;

void advance_vblank_deadline()
{
    g_vblank_next += std::chrono::nanoseconds(VGA_PERIOD_NS);
    g_vblank_rem += VGA_PERIOD_REM;
    if (g_vblank_rem >= VGA_PERIOD_DEN) {
        g_vblank_next += std::chrono::nanoseconds(1);
        g_vblank_rem -= VGA_PERIOD_DEN;
    }
}

void sleep_until_vblank(steady_clock::time_point deadline)
{
     


    for (;;) {
        const auto now = steady_clock::now();
        if (now >= deadline)
            return;

        const auto remaining = deadline - now;
        if (remaining > std::chrono::milliseconds(2)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } else {
            std::this_thread::yield();
        }
    }
}

}  

void waitvsync()
{
     


#if defined(KAOS_EDITOR) && defined(_WIN32)
    kaos_editor_win32::present(kaos_screen_buffer());
    kaos_editor_win32::pump_events();
#else
    kaos_sdl_present(kaos_screen_buffer());
    kaos_sdl_pump_events();
#endif

    const steady_clock::time_point now = steady_clock::now();
    if (!g_vblank_started) {
        g_vblank_started = true;
        g_vblank_next = now;
        g_vblank_rem = 0;
    }

     


    while (g_vblank_next <= now)
        advance_vblank_deadline();

    sleep_until_vblank(g_vblank_next);
    advance_vblank_deadline();
}

void setpal(PPal pal, word fromcol, word numcol)
{
    if (!pal) return;
#if defined(KAOS_EDITOR) && defined(_WIN32)
    kaos_editor_win32::set_palette(reinterpret_cast<const byte *>(pal),
                                   static_cast<int>(fromcol),
                                   static_cast<int>(numcol));
#else
    kaos_sdl_set_palette(reinterpret_cast<const byte *>(pal),
                         static_cast<int>(fromcol),
                         static_cast<int>(numcol));
#endif
}

static byte pal_work[768];

static void pal_upload(PPal pal, word fromcol, word numcol)
{
    (void)pal;
    setpal(reinterpret_cast<PPal>(pal_work), fromcol, numcol);
}

static byte pal_interp(byte initv, byte endv, word step, word total)
{
    if (!total) return endv;
    const int a = static_cast<int>(initv);
    const int b = static_cast<int>(endv);
    const int diff = b - a;
    const int mag = diff < 0 ? -diff : diff;
    const int delta = (mag * static_cast<int>(step)) /
                      static_cast<int>(total);
    const int v = a + (diff < 0 ? -delta : delta);
    return static_cast<byte>(v);
}

void palfilter(PPal pal, byte r, byte g, byte b)
{
    if (!pal) return;
    byte *p = reinterpret_cast<byte *>(pal);

     
     
    for (int i = 0; i < 256; ++i) {
        p[i * 3 + 0] = static_cast<byte>((p[i * 3 + 0] + r) >> 1);
        p[i * 3 + 1] = static_cast<byte>((p[i * 3 + 1] + g) >> 1);
        p[i * 3 + 2] = static_cast<byte>((p[i * 3 + 2] + b) >> 1);
    }
}

void fadeinpart(PPal pal, word fromcol, word numcol, word ciclo, word numcicli)
{
    if (!pal || !numcicli) return;
    const byte *src = reinterpret_cast<const byte *>(pal);
    const int first = static_cast<int>(fromcol);
    const int count = static_cast<int>(numcol);
    const int last = std::min(256, first + count);
    const int step = static_cast<int>(ciclo);
    const int total = static_cast<int>(numcicli);

    std::memcpy(pal_work, src, sizeof(pal_work));
    for (int i = std::max(0, first); i < last; ++i) {
        pal_work[i * 3 + 0] = static_cast<byte>((src[i * 3 + 0] * step) / total);
        pal_work[i * 3 + 1] = static_cast<byte>((src[i * 3 + 1] * step) / total);
        pal_work[i * 3 + 2] = static_cast<byte>((src[i * 3 + 2] * step) / total);
    }
    pal_upload(pal, fromcol, numcol);
}

void fadeoutpart(PPal pal, word fromcol, word numcol, word ciclo, word numcicli)
{
    if (!pal || !numcicli) return;
    const byte *src = reinterpret_cast<const byte *>(pal);
    const int first = static_cast<int>(fromcol);
    const int count = static_cast<int>(numcol);
    const int last = std::min(256, first + count);
    const int total = static_cast<int>(numcicli);
    const int step = static_cast<int>(ciclo);
    const int factor = std::max(0, total - step);

    std::memcpy(pal_work, src, sizeof(pal_work));
    for (int i = std::max(0, first); i < last; ++i) {
        pal_work[i * 3 + 0] = static_cast<byte>((src[i * 3 + 0] * factor) / total);
        pal_work[i * 3 + 1] = static_cast<byte>((src[i * 3 + 1] * factor) / total);
        pal_work[i * 3 + 2] = static_cast<byte>((src[i * 3 + 2] * factor) / total);
    }
    pal_upload(pal, fromcol, numcol);
}

void fadein(PPal pal, word fromcol, word numcol, word numcicli)
{
    if (!pal || !numcicli) return;
    const byte *src = reinterpret_cast<const byte *>(pal);
    std::memcpy(pal_work, src, sizeof(pal_work));

    const int first = static_cast<int>(fromcol);
    const int last = std::min(256, first + static_cast<int>(numcol));
    std::memset(pal_work, 0, sizeof(pal_work));
    pal_upload(pal, fromcol, numcol);

    for (word step = 1; step <= numcicli; ++step) {
        for (int i = std::max(0, first); i < last; ++i) {
            pal_work[i * 3 + 0] = static_cast<byte>(
                (src[i * 3 + 0] * step) / numcicli);
            pal_work[i * 3 + 1] = static_cast<byte>(
                (src[i * 3 + 1] * step) / numcicli);
            pal_work[i * 3 + 2] = static_cast<byte>(
                (src[i * 3 + 2] * step) / numcicli);
        }
        pal_upload(pal, fromcol, numcol);
        waitvsync();
    }
}

void fadeout(PPal pal, word fromcol, word numcol, word numcicli)
{
    if (!pal || !numcicli) return;
    const byte *src = reinterpret_cast<const byte *>(pal);
    std::memcpy(pal_work, src, sizeof(pal_work));

    const int first = static_cast<int>(fromcol);
    const int last = std::min(256, first + static_cast<int>(numcol));

    for (int step = static_cast<int>(numcicli) - 1; step >= 0; --step) {
        for (int i = std::max(0, first); i < last; ++i) {
            pal_work[i * 3 + 0] = static_cast<byte>(
                (src[i * 3 + 0] * step) / numcicli);
            pal_work[i * 3 + 1] = static_cast<byte>(
                (src[i * 3 + 1] * step) / numcicli);
            pal_work[i * 3 + 2] = static_cast<byte>(
                (src[i * 3 + 2] * step) / numcicli);
        }
        pal_upload(pal, fromcol, numcol);
        waitvsync();
    }
}

void palmorph(PPal initpal, PPal endpal,
              word fromcol, word numcol, word numcicli)
{
    if (!initpal || !endpal || !numcicli) return;
    const byte *init = reinterpret_cast<const byte *>(initpal);
    const byte *target = reinterpret_cast<const byte *>(endpal);

    const int first = static_cast<int>(fromcol);
    const int last = std::min(256, first + static_cast<int>(numcol));
    for (word step = 1; step <= numcicli; ++step) {
        std::memcpy(pal_work, init, sizeof(pal_work));
        for (int i = std::max(0, first); i < last; ++i)
            for (int c = 0; c < 3; ++c)
                pal_work[i * 3 + c] = pal_interp(
                    init[i * 3 + c], target[i * 3 + c], step, numcicli);
        pal_upload(initpal, fromcol, numcol);
        waitvsync();
    }
}

void palmorphpart(PPal initpal, PPal endpal,
                  word fromcol, word numcol,
                  word ciclo, word numcicli)
{
    if (!initpal || !endpal || !numcicli) return;
    const byte *init = reinterpret_cast<const byte *>(initpal);
    const byte *target = reinterpret_cast<const byte *>(endpal);

    std::memcpy(pal_work, init, sizeof(pal_work));
    const int first = static_cast<int>(fromcol);
    const int last = std::min(256, first + static_cast<int>(numcol));
    for (int i = std::max(0, first); i < last; ++i)
        for (int c = 0; c < 3; ++c)
            pal_work[i * 3 + c] = pal_interp(
                init[i * 3 + c], target[i * 3 + c], ciclo, numcicli);

    pal_upload(initpal, fromcol, numcol);
}
