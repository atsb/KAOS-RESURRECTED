 






#include "std.hpp"
#include "fixed.h"
#include "lgraph.hpp"
#if defined(KAOS_EDITOR) && defined(_WIN32)
#include "win32_editor_platform.hpp"
#else
#include "kaos_sdl.hpp"
#endif
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cctype>
#include <filesystem>
#include <string>
#include <cstring>
#include <new>

static constexpr int SCREEN_W = 320;
static constexpr int SCREEN_H = 200;

byte *vbuff = nullptr;

 

byte *vpage = new (std::nothrow) byte[SCREEN_W * SCREEN_H]{};
static byte current_palette[768] = {};
static int lg_minx = 0, lg_miny = 0, lg_maxx = SCREEN_W - 1, lg_maxy = SCREEN_H - 1;

byte *kaos_screen_buffer()
{
    return vpage;
}

static inline bool inside(int x, int y)
{
    return x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H;
}

static inline byte *pixel_ptr(byte *buffer, int x, int y)
{
    return buffer + y * SCREEN_W + x;
}

static inline byte shade_pixel(byte value, char shader)
{
    const byte shaded = static_cast<byte>(value + static_cast<byte>(shader));
    const byte hi = static_cast<byte>((value ^ shaded) & 0xF0);
    if (!hi)
        return shaded;
    return static_cast<byte>((shaded ^ hi) | 0x0F);
}

word xytodisp(word x, word y)
{
    return static_cast<word>(x + y * SCREEN_W);
}

void setvrect(int x, int y, int dx, int dy)
{
    lg_minx = std::clamp(x, 0, SCREEN_W);
    lg_miny = std::clamp(y, 0, SCREEN_H);
    lg_maxx = std::clamp(x + dx - 1, -1, SCREEN_W - 1);
    lg_maxy = std::clamp(y + dy - 1, -1, SCREEN_H - 1);
}

void initL()
{
     





    if (!vpage)
        error("initL", "visible framebuffer allocation failed");
    std::memset(vpage, 0, SCREEN_W * SCREEN_H);
#if defined(KAOS_EDITOR) && defined(_WIN32)
    if (!kaos_editor_win32::init(SCREEN_W, SCREEN_H, "KAOS Level Editor"))
        error("initL", "Win32 editor display initialization failed");
#else
    if (!kaos_sdl_video_init(SCREEN_W, SCREEN_H))
        error("initL", kaos_sdl_video_error());
#endif
}

void doneL()
{
    vbuff = nullptr;
    if (vpage)
        std::memset(vpage, 0, SCREEN_W * SCREEN_H);
#if defined(KAOS_EDITOR) && defined(_WIN32)
    kaos_editor_win32::shutdown();
#else
    kaos_sdl_video_shutdown();
#endif
}

void setvbuff(void *buff)
{
    vbuff = static_cast<byte *>(buff);
}

void putpix(word x, word y, byte col)
{
    if (x < SCREEN_W && y < SCREEN_H)
        vbuff[y * SCREEN_W + x] = col;
}

byte getpix(word x, word y)
{
    if (x >= SCREEN_W || y >= SCREEN_H)
        return 0;
    return vbuff[y * SCREEN_W + x];
}

static void fill_bytes(byte *dest, std::size_t count, std::uint32_t value)
{
    const byte pattern[4] = {
        static_cast<byte>(value),
        static_cast<byte>(value >> 8),
        static_cast<byte>(value >> 16),
        static_cast<byte>(value >> 24)
    };

    for (std::size_t i = 0; i < count; ++i)
        dest[i] = pattern[i & 3];
}

void fillpage(byte col)
{
    if (!vbuff) return;
    std::memset(vbuff, col, SCREEN_W * SCREEN_H);
}

void ffillscreen(long col)
{
    fill_bytes(vbuff, SCREEN_W * SCREEN_H,
               static_cast<std::uint32_t>(col));
}

void ffillarea(word y, word dey, long col)
{
    const int y0 = min<int>(y, SCREEN_H);
    const int y1 = min<int>(y0 + dey, SCREEN_H);
    for (int yy = y0; yy < y1; ++yy)
        fill_bytes(vbuff + yy * SCREEN_W, SCREEN_W,
                   static_cast<std::uint32_t>(col));
}

void ftransfscreen(void *dest)
{
    std::memcpy(dest, vbuff, SCREEN_W * SCREEN_H);
}

void ftransfarea(word y, word dey, void *dest)
{
    const int y0 = min<int>(y, SCREEN_H);
    const int h = min<int>(dey, SCREEN_H - y0);
    if (h > 0)
        std::memcpy(dest, vbuff + y0 * SCREEN_W, h * SCREEN_W);
}

void fcopyarea(word y, word y1, word dey, void *dest)
{
    const int sy = min<int>(y, SCREEN_H);
    const int dy = min<int>(y1, SCREEN_H);
    const int h = min<int>(dey, min(SCREEN_H - sy, SCREEN_H - dy));
    if (h > 0)
        std::memmove(static_cast<byte *>(dest) + dy * SCREEN_W,
                     vbuff + sy * SCREEN_W,
                     h * SCREEN_W);
}

static inline int aligned_width(int width)
{
    return (width + 3) & ~3;
}

void ffillblock(word x, word y, word dex, word dey, long col)
{
    const int w = aligned_width(dex);
    for (int yy = 0; yy < dey && static_cast<int>(y) + yy < SCREEN_H; ++yy) {
        if (y + yy < 0) continue;
        const int copy_w = min(w, SCREEN_W - static_cast<int>(x));
        if (copy_w > 0 && x < SCREEN_W)
            fill_bytes(vbuff + (y + yy) * SCREEN_W + x, copy_w,
                       static_cast<std::uint32_t>(col));
    }
}

void ftransfblock(word x, word y, word dex, word dey, void *dest)
{
     











    byte *d = static_cast<byte *>(dest) + y * SCREEN_W + x;
    const byte *s = vbuff + y * SCREEN_W + x;
    const int w = aligned_width(dex);

    for (int yy = 0; yy < dey; ++yy) {
        std::memcpy(d, s, static_cast<std::size_t>(w));
        d += SCREEN_W;
        s += SCREEN_W;
    }
}

void fcopyblock(word x, word y, word x1, word y1,
                word dex, word dey, void *dest)
{
    byte *d = static_cast<byte *>(dest);
    const int w = aligned_width(dex);

    for (int yy = 0; yy < dey; ++yy) {
        if (y + yy >= SCREEN_H || x >= SCREEN_W) break;
        if (y1 + yy >= SCREEN_H || x1 >= SCREEN_W) break;

        const int copy_w = min(w, min(
            SCREEN_W - static_cast<int>(x),
            SCREEN_W - static_cast<int>(x1)));

        if (copy_w > 0)
            std::memmove(d + (y1 + yy) * SCREEN_W + x1,
                         vbuff + (y + yy) * SCREEN_W + x,
                         copy_w);
    }
}

void getlfig(word x, word y, word dex, word dey, void *data)
{
    byte *out = static_cast<byte *>(data);
    for (int yy = 0; yy < dey; ++yy)
        for (int xx = 0; xx < dex; ++xx)
            out[yy * dex + xx] =
                inside(x + xx, y + yy) ? vbuff[(y + yy) * SCREEN_W + x + xx] : 0;
}

void getrfig(word x, word y, word dex, word dey, void *data)
{
    byte *out = static_cast<byte *>(data);
    for (int xx = 0; xx < dex; ++xx)
        for (int yy = 0; yy < dey; ++yy)
            out[xx * dey + yy] =
                inside(x + xx, y + yy) ? vbuff[(y + yy) * SCREEN_W + x + xx] : 0;
}

void storelfig(word x, word y, word dex, word dey, void *data)
{
    const byte *in = static_cast<const byte *>(data);
    for (int yy = 0; yy < dey; ++yy)
        for (int xx = 0; xx < dex; ++xx)
            if (inside(x + xx, y + yy))
                vbuff[(y + yy) * SCREEN_W + x + xx] = in[yy * dex + xx];
}

void storerfig(word x, word y, word dex, word dey, void *data)
{
    const byte *in = static_cast<const byte *>(data);
    for (int xx = 0; xx < dex; ++xx)
        for (int yy = 0; yy < dey; ++yy)
            if (inside(x + xx, y + yy))
                vbuff[(y + yy) * SCREEN_W + x + xx] = in[xx * dey + yy];
}

void putlfig(word x, word y, word dex, word dey, void *data)
{
    const byte *in = static_cast<const byte *>(data);
    for (int yy = 0; yy < dey; ++yy)
        for (int xx = 0; xx < dex; ++xx) {
            const byte c = in[yy * dex + xx];
            if (c && inside(x + xx, y + yy))
                vbuff[(y + yy) * SCREEN_W + x + xx] = c;
        }
}

void putrfig(word x, word y, word dex, word dey, void *data)
{
    const byte *in = static_cast<const byte *>(data);
    for (int xx = 0; xx < dex; ++xx)
        for (int yy = 0; yy < dey; ++yy) {
            const byte c = in[xx * dey + yy];
            if (c && inside(x + xx, y + yy))
                vbuff[(y + yy) * SCREEN_W + x + xx] = c;
        }
}

void putlchar(word x, word y, word dex, word dey,
              byte col1, byte col2, void *data)
{
    const byte *in = static_cast<const byte *>(data);
    for (int yy = 0; yy < dey; ++yy)
        for (int xx = 0; xx < dex; ++xx) {
            byte c = in[yy * dex + xx];
            if (!c || !inside(x + xx, y + yy))
                continue;
            if (c == 1) c = col1;
            else if (c == 2) c = col2;
            vbuff[(y + yy) * SCREEN_W + x + xx] = c;
        }
}

void putlschar(word x, word y, word dex, word dey,
               byte addcol, void *data)
{
    const byte *in = static_cast<const byte *>(data);
    for (int yy = 0; yy < dey; ++yy)
        for (int xx = 0; xx < dex; ++xx) {
            const byte c = in[yy * dex + xx];
            if (c && inside(x + xx, y + yy))
                vbuff[(y + yy) * SCREEN_W + x + xx] =
                    static_cast<byte>(c + addcol);
        }
}

void getlvfig(int x, int y, word dex, word dey, void *data)
{
    byte *out = static_cast<byte *>(data);
    for (int yy = 0; yy < dey; ++yy)
        for (int xx = 0; xx < dex; ++xx) {
            const int sx = x + xx;
            const int sy = y + yy;
            out[yy * dex + xx] = inside(sx, sy)
                ? vbuff[sy * SCREEN_W + sx] : 0;
        }
}

void storelcfig(int x, int y, word dex, word dey, void *data)
{
    const byte *in = static_cast<const byte *>(data);
    for (int yy = 0; yy < dey; ++yy) {
        const int sy = y + yy;
        if (sy < lg_miny || sy > lg_maxy)
            continue;

        for (int xx = 0; xx < dex; ++xx) {
            const int sx = x + xx;
            if (sx >= lg_minx && sx <= lg_maxx && inside(sx, sy))
                vbuff[sy * SCREEN_W + sx] = in[yy * dex + xx];
        }
    }
}

void putlcfig(int x, int y, word dex, word dey, void *data)
{
    const byte *in = static_cast<const byte *>(data);
    for (int yy = 0; yy < dey; ++yy) {
        const int sy = y + yy;
        if (sy < lg_miny || sy > lg_maxy)
            continue;

        for (int xx = 0; xx < dex; ++xx) {
            const int sx = x + xx;
            const byte c = in[yy * dex + xx];
            if (c && sx >= lg_minx && sx <= lg_maxx && inside(sx, sy))
                vbuff[sy * SCREEN_W + sx] = c;
        }
    }
}

void putlcchar(int x, int y, word dex, word dey,
               byte col1, byte col2, void *data)
{
    const byte *in = static_cast<const byte *>(data);
    for (int yy = 0; yy < dey; ++yy) {
        const int sy = y + yy;
        if (sy < lg_miny || sy > lg_maxy)
            continue;

        for (int xx = 0; xx < dex; ++xx) {
            const int sx = x + xx;
            byte c = in[yy * dex + xx];
            if (!c || sx < lg_minx || sx > lg_maxx || !inside(sx, sy))
                continue;
            if (c == 1) c = col1;
            else if (c == 2) c = col2;
            vbuff[sy * SCREEN_W + sx] = c;
        }
    }
}

void storelmfig(word x, word y, word dex, word dey, void *mask, void *data)
{
    if (!vbuff || !mask || !data || x >= SCREEN_W || y >= SCREEN_H) return;
    const byte *m = static_cast<const byte *>(mask);
    const byte *in = static_cast<const byte *>(data);
    const int w = min<int>(dex, SCREEN_W - x);
    const int h = min<int>(dey, SCREEN_H - y);
    for (int yy = 0; yy < h; ++yy) {
        for (int xx = 0; xx < w; ++xx) {
            if (m[yy * dex + xx])
                vbuff[(y + yy) * SCREEN_W + x + xx] = in[yy * dex + xx];
        }
    }
}

void scalerscfig(int x, int y, word dex, word dey, int vdx, int vdy,
                 char shader, void *rfig)
{
    if (!vbuff || !rfig || dex == 0 || dey == 0 || vdx <= 0 || vdy <= 0) return;

    const byte *src = static_cast<const byte *>(rfig);
    fixed adjx = fixdiv(static_cast<fixed>(dex) << 16, static_cast<fixed>(vdx));
    fixed adjy = fixdiv(static_cast<fixed>(dey) << 16, static_cast<fixed>(vdy));
    fixed adjo = 0;

    int draw_y = y;
    int draw_h = vdy;
    if (draw_y < lg_miny) {
        adjo = adjy * static_cast<fixed>(lg_miny - draw_y);
        draw_h -= lg_miny - draw_y;
        draw_y = lg_miny;
    }
    if (draw_y + draw_h - 1 > lg_maxy)
        draw_h = lg_maxy + 1 - draw_y;
    if (draw_h <= 0) return;

    fixed src_y = adjo;
    for (int ox = 0; ox < vdx; ++ox) {
        const int dx = x + ox;
        if (dx >= lg_minx && dx <= lg_maxx) {
            fixed src_x = adjx * ox;
            int sx = static_cast<int>(src_x >> 16);
            if (sx < 0) sx = 0;
            if (sx >= dex) sx = dex - 1;

            fixed sy_fixed = src_y;
            for (int oy = 0; oy < draw_h; ++oy) {
                int sy = static_cast<int>(sy_fixed >> 16);
                if (sy < 0) sy = 0;
                if (sy >= dey) sy = dey - 1;
                const byte c = src[sx * dey + sy];
                const int dy = draw_y + oy;
                if (c && dy >= 0 && dy < SCREEN_H)
                    vbuff[dy * SCREEN_W + dx] = shade_pixel(c, shader);
                sy_fixed += adjy;
            }
        }
    }
}

static std::uint32_t read_le32_from_bytes(const byte b[4])
{
    return std::uint32_t(b[0]) | (std::uint32_t(b[1]) << 8) |
           (std::uint32_t(b[2]) << 16) | (std::uint32_t(b[3]) << 24);
}

static std::uint32_t read_le32(std::FILE *f)
{
    byte b[4];
    if (std::fread(b, 1, 4, f) != 4) return 0;
    return std::uint32_t(b[0]) | (std::uint32_t(b[1]) << 8) |
           (std::uint32_t(b[2]) << 16) | (std::uint32_t(b[3]) << 24);
}

static std::uint16_t read_be16(std::FILE *f)
{
    byte b[2];
    if (std::fread(b, 1, 2, f) != 2) return 0;
    return static_cast<std::uint16_t>((b[0] << 8) | b[1]);
}

static std::FILE *open_lbm_dos_path(const char *filename)
{
    if (!filename) return nullptr;

     


    std::string name(filename);
    std::replace(name.begin(), name.end(), '\\', '/');

    if (std::FILE *f = std::fopen(name.c_str(), "rb"))
        return f;

#if defined(KAOS_EDITOR) && defined(_WIN32)
    const char *base = kaos_editor_win32::executable_directory();
    if (base && *base) {
        std::string path(base);
        if (!path.empty() && path.back() != '\\' && path.back() != '/')
            path += '\\';
        path += name;
        if (std::FILE *f = std::fopen(path.c_str(), "rb"))
            return f;
    }
    return nullptr;
#else
     


    const std::filesystem::path input(name);
    std::filesystem::path current = input.is_absolute() ? input.root_path() : ".";
    const auto relative = input.is_absolute() ? input.relative_path() : input;
    std::error_code ec;
    for (const auto &part : relative) {
        const std::string wanted = part.string();
        if (wanted.empty() || wanted == ".") continue;
        if (wanted == "..") {
            current /= part;
            continue;
        }
        bool found = false;
        for (std::filesystem::directory_iterator it(current, ec), end;
             !ec && it != end; it.increment(ec)) {
            const std::string actual = it->path().filename().string();
            if (actual.size() != wanted.size()) continue;
            bool same = true;
            for (std::size_t i = 0; i < actual.size(); ++i) {
                if (std::tolower(static_cast<unsigned char>(actual[i])) !=
                    std::tolower(static_cast<unsigned char>(wanted[i]))) {
                    same = false;
                    break;
                }
            }
            if (same) {
                current /= it->path().filename();
                found = true;
                break;
            }
        }
        if (!found) return nullptr;
    }
    return std::fopen(current.string().c_str(), "rb");
#endif
}

int swapb(int n)
{
    return static_cast<int>(((static_cast<unsigned>(n) & 0xffu) << 8) |
                             ((static_cast<unsigned>(n) >> 8) & 0xffu));
}

char loadLBM(const char *filename, void *pal)
{
    std::FILE *f = open_lbm_dos_path(filename);
    if (!f) return 0;

    if (read_le32(f) != CK_FORM) {
        std::fclose(f);
        return 0;
    }

    std::fseek(f, 4, SEEK_CUR);
    if (read_le32(f) != ID_PBM) {
        std::fclose(f);
        return 0;
    }

    bitmap_header bmhead{};
    byte *pbuf = vbuff;

    for (;;) {
        byte idb[4], size_bytes[4];
        if (std::fread(idb, 1, 4, f) != 4) break;
        if (std::fread(size_bytes, 1, 4, f) != 4) break;

         



        const std::uint32_t id = read_le32_from_bytes(idb);
        const std::uint32_t size =
            (std::uint32_t(size_bytes[0]) << 24) |
            (std::uint32_t(size_bytes[1]) << 16) |
            (std::uint32_t(size_bytes[2]) << 8) |
            std::uint32_t(size_bytes[3]);
        const long data_start = std::ftell(f);
        const long chunk_end = data_start + static_cast<long>((size + 1u) & ~1u);

        if (id == ID_BMHD) {
            bmhead.width = read_be16(f);
            bmhead.height = read_be16(f);
            bmhead.x = read_be16(f);
            bmhead.y = read_be16(f);
            std::fread(&bmhead.nPlanes, 1, 4, f);
            bmhead.transparent = read_be16(f);
            bmhead.x_aspect = static_cast<byte>(std::fgetc(f));
            bmhead.y_aspect = static_cast<byte>(std::fgetc(f));
            bmhead.p_width = read_be16(f);
            bmhead.p_height = read_be16(f);
        } else if (id == ID_CMAP) {
            if (pal) {
                byte *p = static_cast<byte *>(pal);
                const std::size_t n = min<std::uint32_t>(size, 768u);
                if (std::fread(p, 1, n, f) != n) {
                    std::fclose(f);
                    return 0;
                }
                for (std::size_t i = 0; i < n; ++i)
                    p[i] = static_cast<byte>(p[i] >> 2);
            } else {
                std::fseek(f, static_cast<long>(size), SEEK_CUR);
            }
        } else if (id == ID_BODY) {
            for (unsigned row = 0; row < bmhead.height; ++row) {
                unsigned remaining = (bmhead.width + 1u) & 0x7ffeu;
                if (bmhead.compression) {
                    while (remaining) {
                        const int raw = std::fgetc(f);
                        if (raw == EOF) {
                            std::fclose(f);
                            return 0;
                        }
                        const signed char code = static_cast<signed char>(raw);
                        if (code >= 0) {
                            const unsigned count = static_cast<unsigned>(code) + 1u;
                            if (count > remaining || std::fread(pbuf, 1, count, f) != count) {
                                std::fclose(f);
                                return 0;
                            }
                            pbuf += count;
                            remaining -= count;
                        } else if (code != -128) {
                            const unsigned count = static_cast<unsigned>(-code) + 1u;
                            const int value = std::fgetc(f);
                            if (value == EOF || count > remaining) {
                                std::fclose(f);
                                return 0;
                            }
                            std::memset(pbuf, value, count);
                            pbuf += count;
                            remaining -= count;
                        }
                    }
                } else {
                    if (std::fread(pbuf, 1, remaining, f) != remaining) {
                        std::fclose(f);
                        return 0;
                    }
                    pbuf += remaining;
                }
            }
        }

        std::fseek(f, chunk_end, SEEK_SET);
    }

    std::fclose(f);
    return 1;
}

