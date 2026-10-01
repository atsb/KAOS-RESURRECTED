 






#include "std.hpp"
#include "fixed.h"
#include "lgraph.hpp"
#include "3dengine.hpp"
#include "kaos.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>

#define MAXSHADE 16
#define HISHMASK (0xFF - MAXSHADE + 1)
#define LOSHMASK (MAXSHADE - 1)

int linex, liney, lineh, column;
std::uint32_t wstep, woffs;
void far *texture;
#ifdef __DOUBLEWALLS__
void far *htexture;
#endif

word *coldist = new word[MAXVIEWWIDTH];

fixed curx, cury, curincx, curincy;
char shade;
int maxcolumn;
int viewtop, viewbottom, viewleft, viewright;
int mskx, msky;

void far *hid;
unsigned fcdest;
fixed txt_indx, indincx;
fixed backx, backy1, backy2;

static inline byte shade_pixel(byte value, char shader)
{
    byte shaded = static_cast<byte>(value + static_cast<byte>(shader));
    const byte hi = static_cast<byte>((value ^ shaded) & HISHMASK);
    if (!hi)
        return shaded;
    return static_cast<byte>((shaded ^ hi) | LOSHMASK);
}

 
static inline byte *vpage_bytes()
{
    return vpage;
}

static inline byte *texture_bytes()
{
    return static_cast<byte *>(texture);
}

static inline int screen_offset(int x, int y)
{
    return y * 320 + x;
}

void drawwallHP(char shader)
{
    byte *dst = vpage_bytes();
    const byte *tex = texture_bytes();

    if (!dst || !tex || lineh <= 0)
        return;

    std::uint32_t texpos = static_cast<std::uint32_t>(woffs);
    const std::uint32_t step = static_cast<std::uint32_t>(wstep);

    for (int i = 0; i < lineh; ++i) {
        const int x = linex;
        const int y = liney + i;

        if (x >= 0 && x < 320 && y >= 0 && y < 200) {
            const unsigned tex_y =
                static_cast<unsigned>((texpos >> 20) & 63u);
            const byte c = tex[column * 64 + tex_y];
            dst[screen_offset(x, y)] = shade_pixel(c, shader);
        }

        texpos += step;
    }
}

#ifdef __DOUBLEWALLS__
void drawwallHP2(char shader)
{
    byte *dst = vpage_bytes();
    const byte *low = texture_bytes();
    const byte *high = static_cast<const byte *>(htexture);
    if (!dst || !low || !high || lineh <= 0)
        return;

    std::uint32_t pos = static_cast<std::uint32_t>(woffs);
    const std::uint32_t step = static_cast<std::uint32_t>(wstep);
    const std::uint32_t column_base = static_cast<std::uint32_t>(column) << 6;

     
     
    const bool start_low = ((pos >> 16) & 0xfc00u) != 0;
    const byte *current = start_low ? low : high;
    const std::uint32_t base = column_base;

    for (int i = 0; i < lineh; ++i) {
        const int x = linex;
        const int y = liney + i;
        const unsigned tex_y = (pos >> 20) & 63u;
        if (x >= 0 && x < 320 && y >= 0 && y < 200) {
            const byte c = current[base + tex_y];
            dst[screen_offset(x, y)] = shade_pixel(c, shader);
        }

        const bool was_high = ((pos >> 16) & 0xfc00u) == 0;
        pos += step;
        const bool now_high = ((pos >> 16) & 0xfc00u) == 0;
        if (was_high != now_high) {
            current = now_high ? high : low;
        }
    }
}

#endif

void vline(byte col)
{
    byte *dst = vpage_bytes();
    if (!dst || lineh <= 0)
        return;

    for (int i = 0; i < lineh; ++i) {
        const int y = liney + i;
        if (linex >= 0 && linex < 320 && y >= 0 && y < 200)
            dst[screen_offset(linex, y)] = col;
    }
}

void init_fc()
{
     







    fcdest = static_cast<unsigned>(liney * 320 + linex);

    indincx = static_cast<fixed>(
        static_cast<std::int64_t>(curincx) << 6);

    txt_indx = static_cast<fixed>(
        static_cast<std::int64_t>(
            static_cast<std::uint32_t>(curx) & 0x003fffffu) << 6);

    mskx = (curx >> 16) & 0xffc0;
    msky = (cury >> 16) & 0xffc0;
}

static inline const byte *floor_texture()
{
    return texture_bytes();
}

static inline const std::uint16_t *hide_buffer()
{
     


    return static_cast<const std::uint16_t *>(hid);
}

static inline byte sample_floor()
{
    const byte *tex = floor_texture();
    if (!tex) return 0;

     



    const std::uint32_t tx =
        (static_cast<std::uint32_t>(txt_indx) >> 16) & 0x0fc0u;
    const std::uint32_t ty =
        (static_cast<std::uint32_t>(cury) >> 16) & 0x003fu;
    return tex[tx + ty];
}

static inline void fc_advance_coordinates()
{
    txt_indx += indincx;
    curx += curincx;
    cury += curincy;
}

static inline int fc_map_index()
{
     












    const std::uint16_t ax = mskx;
    const std::uint16_t bx = static_cast<std::uint16_t>(msky >> 6);
    const std::uint16_t result =
        static_cast<std::uint16_t>(ax + bx);  
    return static_cast<int>(result);
}

int fast_drawfc()
{
    const std::uint16_t *hide = hide_buffer();
    int c = column;
    unsigned dest = fcdest;

    for (;;) {
        const int available = hide ? hide[c] : lineh;

        if (lineh < available) {
            const byte pixel = sample_floor();
            if (dest < 320u * 200u)
                vpage_bytes()[dest] = shade_pixel(pixel, shade);
        }

        ++c;
        ++dest;
        fc_advance_coordinates();

        const int nx = (curx >> 16) & 0xffc0;
        const int ny = (cury >> 16) & 0xffc0;
        if (nx != mskx || ny != msky) {
            mskx = nx;
            msky = ny;
            fcdest = dest;           
            column = c;
            return fc_map_index();   
        }

        if (c >= maxcolumn)
            break;
    }

    fcdest = dest;
    column = c;
    return fc_map_index();
}

int fast_drawtfc(byte col)
{
    const std::uint16_t *hide = hide_buffer();
    int c = column;
    unsigned dest = fcdest;

    for (;;) {
        const int available = hide ? hide[c] : lineh;

        if (lineh < available) {
            byte pixel = sample_floor();
            if (pixel) {
                pixel = shade_pixel(pixel, shade);
                if (dest < 320u * 200u)
                    vpage_bytes()[dest] = pixel;
            } else {
                 






                pixel |= col;
                if (pixel && dest < 320u * 200u)
                    vpage_bytes()[dest] = pixel;
            }
        }

        ++c;
        ++dest;
        fc_advance_coordinates();

        const int nx = (curx >> 16) & 0xffc0;
        const int ny = (cury >> 16) & 0xffc0;
        if (nx != mskx || ny != msky) {
            mskx = nx;
            msky = ny;
            fcdest = dest;
            column = c;
            return fc_map_index();
        }

        if (c >= maxcolumn)
            break;
    }

    fcdest = dest;
    column = c;
    return fc_map_index();
}

int fast_drawfcblack(byte black)
{
    const std::uint16_t *hide = hide_buffer();
    int c = column;
    unsigned dest = fcdest;

    for (;;) {
        const int available = hide ? hide[c] : lineh;
        if (lineh < available && dest < 320u * 200u)
            vpage_bytes()[dest] = black;

        ++c;
        ++dest;
        fc_advance_coordinates();

        const int nx = (curx >> 16) & 0xffc0;
        const int ny = (cury >> 16) & 0xffc0;
        if (nx != mskx || ny != msky) {
            mskx = nx;
            msky = ny;
            fcdest = dest;
            column = c;
            return fc_map_index();
        }

        if (c >= maxcolumn)
            break;
    }

    fcdest = dest;
    column = c;
    return fc_map_index();
}

int fast_passfc()
{
    int c = column;
    unsigned dest = fcdest;

    for (;;) {
        ++c;
        ++dest;
        fc_advance_coordinates();

         
        if (c >= maxcolumn)
            break;

        const int nx = (curx >> 16) & 0xffc0;
        const int ny = (cury >> 16) & 0xffc0;
        if (nx == mskx && ny == msky)
            continue;

        mskx = nx;
        msky = ny;
        fcdest = dest;

        const int mapidx = fc_map_index();
         

        if (mapidx & 0xf000)
            continue;

        column = c;
        return mapidx;
    }

    fcdest = dest;
    column = c;
    return fc_map_index();
}

static void draw_scaled_columns(int x, int y, word dex, word dey,
                                int vdx, int vdy, word dist,
                                char shader, const byte *rfig,
                                bool reverse)
{
    if (!rfig || vdx <= 0 || vdy <= 0 || dex == 0 || dey == 0)
        return;

    fixed adjx = fixdiv(static_cast<fixed>(dex) << 16, static_cast<fixed>(vdx));
    fixed adjy = fixdiv(static_cast<fixed>(dey) << 16, static_cast<fixed>(vdy));
    int draw_y = y;
    int draw_h = vdy;
    fixed yoff = 0;

    if (draw_y < viewtop) {
        yoff = adjy * static_cast<fixed>(viewtop - draw_y);
        draw_h -= viewtop - draw_y;
        draw_y = viewtop;
    }
    if (draw_y + draw_h > viewbottom)
        draw_h = viewbottom - draw_y;
    if (draw_h <= 0) return;

    for (int ox = 0; ox < vdx; ++ox) {
        const int sx_screen = reverse ? x - ox : x + ox;
        if (sx_screen < viewleft || sx_screen >= viewright)
            continue;

        const int coldist_index = std::clamp((sx_screen - viewleft),
                                             0, MAXVIEWWIDTH - 1);
        if (coldist && coldist[coldist_index] < dist)
            continue;

        int sx = static_cast<int>((adjx * ox) >> 16);
        if (sx < 0) sx = 0;
        if (sx >= dex) sx = dex - 1;

        fixed sypos = yoff;
        for (int oy = 0; oy < draw_h; ++oy) {
            int sy = static_cast<int>(sypos >> 16);
            if (sy < 0) sy = 0;
            if (sy >= dey) sy = dey - 1;

            const byte c = rfig[sx * dey + sy];
            const int sy_screen = draw_y + oy;
            if (c && sx_screen >= 0 && sx_screen < 320 &&
                sy_screen >= 0 && sy_screen < 200)
                vpage_bytes()[screen_offset(sx_screen, sy_screen)] =
                    shade_pixel(c, shader);
            sypos += adjy;
        }
    }
}

void drawcobjHP(int x, int y, word dex, word dey,
                int vdx, int vdy, word dist, char shader, void *rfig)
{
    draw_scaled_columns(x, y, dex, dey, vdx, vdy, dist, shader,
                        static_cast<const byte *>(rfig), false);
}

void drawrcobjHP(int x, int y, word dex, word dey,
                 int vdx, int vdy, word dist, char shader, void *rfig)
{
    draw_scaled_columns(x + vdx - 1, y, dex, dey, vdx, vdy,
                        dist, shader, static_cast<const byte *>(rfig), true);
}

void drawbackground(int startcol, int y)
{
     










    if (y <= viewtop - halfheight || y >= viewbottom)
        return;

    const fixed startx = backx * startcol;
    fixed starty;
    int height;

    if (y < viewtop) {
        const int toty_pixels = viewtop - y;
        height = halfheight - toty_pixels;
        starty = backy1 * toty_pixels;
        y = viewtop;
    } else {
        starty = 0;
        height = (y > viewtop + halfheight) ? (viewbottom - y) : halfheight;
    }

    if (height <= 0 || !texture)
        return;

    byte *dst_base = vpage_bytes();
    const byte *src_base = texture_bytes();
    if (!dst_base || !src_base)
        return;

    const int vieww = viewwidth - startcol;
    if (vieww <= 0 || startcol < 0)
        return;

     
    std::size_t savedi = screen_offset(viewleft, y);

     
    const std::uint32_t starty_u = static_cast<std::uint32_t>(starty);
    std::size_t src_offset =
        static_cast<std::size_t>(starty_u >> 16) * 320u;
    std::uint16_t toty = static_cast<std::uint16_t>(starty_u & 0xffffu);

    const std::uint32_t lhx = static_cast<std::uint32_t>(backx);
    const std::uint16_t lhy_lo =
        static_cast<std::uint16_t>(static_cast<std::uint32_t>(backy2) & 0xffffu);
    const std::uint16_t lhy_hi =
        static_cast<std::uint16_t>((static_cast<std::uint32_t>(backy2) >> 16) & 0xffffu);

    for (int row = 0; row < height; ++row) {
         
        std::uint32_t hpos = static_cast<std::uint32_t>(startx);
        std::size_t di = savedi;

         
        for (int col = 0; col < vieww; ++col) {
            const std::uint32_t tx = hpos >> 16;
            if (tx < 320u && src_offset + tx < 32000u)
                dst_base[di] = src_base[src_offset + tx];

            ++di;
            hpos += lhx;
        }

         
         
         
         
         
        hpos &= 0xffffu;
        for (int col = 0; col < startcol; ++col) {
            const std::uint32_t tx = hpos >> 16;
            if (tx < 320u && src_offset + tx < 32000u)
                dst_base[di] = src_base[src_offset + tx];

            ++di;
            hpos += lhx;
        }

         
        const std::uint32_t frac = static_cast<std::uint32_t>(toty) + lhy_lo;
        if (frac > 0xffffu)
            src_offset += static_cast<std::size_t>(lhy_hi) + 320u;
        else
            src_offset += static_cast<std::size_t>(lhy_hi);

        toty = static_cast<std::uint16_t>(frac & 0xffffu);
        savedi += 320u;
    }
}

void drawclightHP(int x, int y, word dex, word dey,
                  int vdx, int vdy, word dist, char shader, void *rfig)
{
    if (!rfig || vdx <= 0 || vdy <= 0 || dex == 0 || dey == 0)
        return;

    const byte *light = static_cast<const byte *>(rfig);
    fixed adjx = fixdiv(static_cast<fixed>(dex) << 16, static_cast<fixed>(vdx));
    fixed adjy = fixdiv(static_cast<fixed>(dey) << 16, static_cast<fixed>(vdy));
    int draw_y = y;
    int draw_h = vdy;
    fixed yoff = 0;

    if (draw_y < viewtop) {
        yoff = adjy * static_cast<fixed>(viewtop - draw_y);
        draw_h -= viewtop - draw_y;
        draw_y = viewtop;
    }
    if (draw_y + draw_h > viewbottom)
        draw_h = viewbottom - draw_y;
    if (draw_h <= 0) return;

    for (int ox = 0; ox < vdx; ++ox) {
        const int screen_x = x + ox;
        if (screen_x < viewleft || screen_x >= viewright)
            continue;

        const int cd = std::clamp((screen_x - viewleft),
                                  0, MAXVIEWWIDTH - 1);
        if (coldist && coldist[cd] < dist)
            continue;

        int sx = static_cast<int>((adjx * ox) >> 16);
        if (sx < 0) sx = 0;
        if (sx >= dex) sx = dex - 1;

        fixed sypos = yoff;
        for (int oy = 0; oy < draw_h; ++oy) {
            int sy = static_cast<int>(sypos >> 16);
            if (sy < 0) sy = 0;
            if (sy >= dey) sy = dey - 1;

            const byte light_value = light[sx * dey + sy];
            const int py = draw_y + oy;
            if (light_value && shader < static_cast<char>(light_value) &&
                screen_x >= 0 && screen_x < 320 && py >= 0 && py < 200) {
                const std::size_t off = static_cast<std::size_t>(py) * 320 + screen_x;
                const byte pixel = vpage_bytes()[off];
                byte result = static_cast<byte>(pixel + shader - light_value);
                const byte hi = static_cast<byte>((pixel ^ result) & HISHMASK);
                if (hi)
                    result = static_cast<byte>(result & HISHMASK);
                vpage_bytes()[off] = result;
            }
            sypos += adjy;
        }
    }
}
