#include "std.hpp"
#include "lgraph.hpp"
#include "lmouse.hpp"
#if defined(KAOS_EDITOR) && defined(_WIN32)
#include "win32_editor_platform.hpp"
#else
#include "kaos_sdl.hpp"
#include <SDL3/SDL.h>
#endif
#include <algorithm>
#include <cstring>

Mouse mouse;

static byte mousedata[9] = {
    0, 255, 0,
    255, 255, 255,
    0, 255, 0
};
static byte underdata[9];

static void poll_mouse(int &x, int &y, byte &buttons)
{
#if defined(KAOS_EDITOR) && defined(_WIN32)
    kaos_editor_win32::get_mouse(x, y, buttons);
#else
    float fx = 0.0f, fy = 0.0f;
    const SDL_MouseButtonFlags state = SDL_GetMouseState(&fx, &fy);

    int window_w = 320, window_h = 200;
    kaos_sdl_get_window_size(window_w, window_h);

    x = window_w > 0 ? static_cast<int>(fx * 320.0f / window_w) : 0;
    y = window_h > 0 ? static_cast<int>(fy * 200.0f / window_h) : 0;

    buttons = 0;
    if (state & SDL_BUTTON_LMASK) buttons |= MB_LEFT;
    if (state & SDL_BUTTON_RMASK) buttons |= MB_RIGHT;
    if (state & SDL_BUTTON_MMASK) buttons |= MB_MIDDLE;
#endif
}

void Mouse::getmouserect(int &x1, int &y1, int &x2, int &y2)
{
    x1 = curx - cursor.hotx;
    y1 = cury - cursor.hoty;
    x2 = x1 + cursor.dx - 1;
    y2 = y1 + cursor.dy - 1;
}

void Mouse::refresh(int x, int y, byte butt)
{
    curx = std::clamp(x, 0, 319);
    cury = std::clamp(y, 0, 199);
    buttons = butt;
}

void Mouse::draw()
{
    if (!invisible && cursor.data) {
        byte *saved = vbuff;
        vbuff = VMEMPTR;

        getlvfig(curx - cursor.hotx, cury - cursor.hoty,
                 under.dx, under.dy, under.data);
        putlcfig(curx - cursor.hotx, cury - cursor.hoty,
                 cursor.dx, cursor.dy, cursor.data);

        vbuff = saved;
    }
}

void Mouse::undraw()
{
    if (!invisible && under.data) {
        byte *saved = vbuff;
        vbuff = VMEMPTR;

        storelcfig(curx - cursor.hotx, cury - cursor.hoty,
                   under.dx, under.dy, under.data);

        vbuff = saved;
    }
}

Mouse::Mouse()
{
    invisible = 1;
    installed = 0;
    curx = 160;
    cury = 100;
    buttons = 0;
    butnum = 3;
    std::memset(&cursor, 0, sizeof(cursor));
    std::memset(&under, 0, sizeof(under));
}

char Mouse::install()
{
    if (installed)
        return 1;

    if (!reset())
        return 0;

    setvrect(0, 0, 320, 200);
    setrange(0, 0, 319, 199);
    go(160, 100);

    under.data = nullptr;
    setcursor(nullptr);
    installed = 1;
    return 1;
}

char Mouse::reset()
{
    butnum = 3;
    curx = 160;
    cury = 100;
    buttons = 0;
    return 1;
}

void Mouse::show()
{
    if (invisible) {
        invisible--;
        draw();
    }
}

void Mouse::hide()
{
    if (invisible < 0xff) {
        undraw();
        invisible++;
    }
}

void Mouse::get(int &x, int &y, byte &butt)
{
    int mx, my;
    byte mb;

     


#if defined(KAOS_EDITOR) && defined(_WIN32)
    kaos_editor_win32::pump_events();
#else
    kaos_sdl_pump_events();
#endif

    const bool visible = (invisible == 0);

     


    if (visible)
        hide();
    poll_mouse(mx, my, mb);
    refresh(mx, my, mb);
    if (visible)
        show();

    x = curx;
    y = cury;
    butt = buttons;
}

void Mouse::getpos(int &x, int &y)
{
    byte b;
    get(x, y, b);
}

byte Mouse::getbuttons()
{
    int x, y;
    byte b;
    get(x, y, b);
    return b;
}

void Mouse::go(int x, int y)
{
    curx = std::clamp(x, 0, 319);
    cury = std::clamp(y, 0, 199);
#if defined(KAOS_EDITOR) && defined(_WIN32)
    kaos_editor_win32::warp_mouse(curx, cury);
#else
    kaos_sdl_warp_mouse(curx, cury);
#endif
}

void Mouse::setxrange(int x1, int x2)
{
    if (curx < x1 || curx > x2)
        go((x1 + x2) >> 1, cury);
     
}

void Mouse::setyrange(int y1, int y2)
{
    if (cury < y1 || cury > y2)
        go(curx, (y1 + y2) >> 1);
}

void Mouse::setrange(int x1, int y1, int x2, int y2)
{
    setxrange(x1, x2);
    setyrange(y1, y2);
}

void Mouse::setcursor(MouseCursor *newcurs)
{
    hide();

    if (under.data && under.data != underdata)
        delete[] static_cast<byte *>(under.data);

    if (!newcurs) {
        cursor.hotx = 1;
        cursor.hoty = 1;
        under.dx = cursor.dx = 3;
        under.dy = cursor.dy = 3;
        cursor.data = mousedata;
        under.data = underdata;
    } else {
        cursor = *newcurs;
        under.dx = cursor.dx;
        under.dy = cursor.dy;
        under.data = new byte[under.dx * under.dy];
    }

    show();
}

void Mouse::setcursormem(MouseCursor *newcurs, void *undr)
{
    hide();

    if (!newcurs) {
        cursor.hotx = 1;
        cursor.hoty = 1;
        under.dx = cursor.dx = 3;
        under.dy = cursor.dy = 3;
        cursor.data = mousedata;
        under.data = underdata;
    } else {
        cursor = *newcurs;
        under.dx = cursor.dx;
        under.dy = cursor.dy;
        under.data = undr;
    }

    show();
}

void Mouse::update(int x1, int y1, int dx, int dy)
{
    int mx1, my1, mx2, my2;
    const int x2 = x1 + dx - 1;
    const int y2 = y1 + dy - 1;

    getmouserect(mx1, my1, mx2, my2);

    if ((mx1 >= x1 && mx1 <= x2 || mx2 >= x1 && mx2 <= x2) &&
        (my1 >= y1 && my1 <= y2 || my2 >= y1 && my2 <= y2))
        hide();
}

void Mouse::uninstall()
{
    if (!installed)
        return;

    hide();
    reset();
    installed = 0;
}

Mouse::~Mouse()
{
    uninstall();
}
