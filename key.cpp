#include "std.hpp"
#include "key.hpp"
#include "kaos_sdl.hpp"
#include <cstring>
#include <SDL3/SDL.h>

extern char s_fullscreen;

char Key[129] = {}, CodeAr[6] = {};
char key_pause = 0;

static char KeyP[129] = {};
static bool key_installed = false;
static bool oldcall = false;
static int mouse_dx = 0;
static int mouse_dy = 0;
static bool mouse_left = false;
static bool mouse_right = false;

static int dos_scancode(SDL_Scancode sc)
{
    switch (sc) {
    case SDL_SCANCODE_ESCAPE: return KB_ESC;
    case SDL_SCANCODE_UP: return KB_UP;
    case SDL_SCANCODE_DOWN: return KB_DOWN;
    case SDL_SCANCODE_LEFT: return KB_LEFT;
    case SDL_SCANCODE_RIGHT: return KB_RIGHT;
    case SDL_SCANCODE_LALT:
    case SDL_SCANCODE_RALT: return KB_ALT;
    case SDL_SCANCODE_LSHIFT: return KB_LSHIFT;
    case SDL_SCANCODE_RSHIFT: return KB_RSHIFT;
    case SDL_SCANCODE_LCTRL:
    case SDL_SCANCODE_RCTRL: return KB_CTRL;
    case SDL_SCANCODE_SPACE: return KB_SPACE;
    case SDL_SCANCODE_RETURN: return KB_ENTER;
    case SDL_SCANCODE_KP_ENTER: return KB_tENTER;
    case SDL_SCANCODE_DELETE: return KB_CANC;
    case SDL_SCANCODE_TAB: return KB_TAB;
    case SDL_SCANCODE_BACKSLASH: return KB_BSLASH;
    case SDL_SCANCODE_Z: return KB_Z;
    case SDL_SCANCODE_X: return KB_X;
    case SDL_SCANCODE_Q: return KB_Q;
    case SDL_SCANCODE_W: return KB_W;
    case SDL_SCANCODE_E: return KB_E;
    case SDL_SCANCODE_A: return KB_A;
    case SDL_SCANCODE_S: return KB_S;
    case SDL_SCANCODE_D: return KB_D;
    case SDL_SCANCODE_F: return KB_F;
    case SDL_SCANCODE_P: return KB_P;
    case SDL_SCANCODE_O: return KB_O_;
    case SDL_SCANCODE_L: return KB_L;
    case SDL_SCANCODE_APOSTROPHE: return KB_A_;
    case SDL_SCANCODE_M: return KB_M;
    case SDL_SCANCODE_H: return KB_H;
    case SDL_SCANCODE_J: return KB_J;
    case SDL_SCANCODE_K: return KB_K;
    case SDL_SCANCODE_U: return KB_U;
    case SDL_SCANCODE_COMMA: return KB_COMMA;
    case SDL_SCANCODE_F1: return KB_F1;
    case SDL_SCANCODE_F2: return KB_F2;
    case SDL_SCANCODE_F3: return KB_F3;
    case SDL_SCANCODE_F4: return KB_F4;
    case SDL_SCANCODE_F5: return KB_F5;
    case SDL_SCANCODE_F6: return KB_F6;
    case SDL_SCANCODE_F7: return KB_F7;
    case SDL_SCANCODE_F8: return KB_F8;
    case SDL_SCANCODE_F9: return KB_F9;
    case SDL_SCANCODE_F10: return KB_F10;
    case SDL_SCANCODE_F11: return KB_F11;
    case SDL_SCANCODE_F12: return KB_F12;
    case SDL_SCANCODE_1: return KB_1;
    case SDL_SCANCODE_2: return KB_2;
    case SDL_SCANCODE_3: return KB_3;
    case SDL_SCANCODE_4: return KB_4;
    case SDL_SCANCODE_5: return KB_5;
    case SDL_SCANCODE_6: return KB_6;
    case SDL_SCANCODE_7: return KB_7;
    case SDL_SCANCODE_8: return KB_8;
    case SDL_SCANCODE_9: return KB_9;
    case SDL_SCANCODE_0: return KB_0;
    case SDL_SCANCODE_MINUS: return KB_MINUS;
    case SDL_SCANCODE_SLASH: return KB_SLASH;
    case SDL_SCANCODE_PERIOD: return KB_PER;
    default: return 0;
    }
}

static void update_shift_state()
{
    Key[KB_SHIFT] =
        static_cast<char>(Key[KB_LSHIFT] || Key[KB_RSHIFT]);
}

static void pump_keyboard()
{
    SDL_Event event;
    kaos_sdl_pump_events();
    while (kaos_sdl_poll_event(event)) {
        if (event.type == SDL_EVENT_KEY_DOWN ||
            event.type == SDL_EVENT_KEY_UP) {
            const int code = dos_scancode(event.key.scancode);
            if (!code || code >= 129)
                continue;

            if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.scancode == SDL_SCANCODE_F8 &&
                !event.key.repeat) {
                kaos_sdl_toggle_fullscreen();
                s_fullscreen = kaos_sdl_get_fullscreen() ? 1 : 0;
            }

            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (!event.key.repeat)
                    KeyP[code] = 1;
                Key[code] = 1;
            } else {
                Key[code] = 0;
            }

            update_shift_state();

            if (event.type == SDL_EVENT_KEY_DOWN &&
                code != KB_F4 && code >= KB_1 && code <= KB_0) {
                for (int i = 0; i < 5; ++i)
                    CodeAr[i] = static_cast<char>(CodeAr[i + 1] - 1);
                CodeAr[5] = static_cast<char>(code + 5);
            }
        }
    }

    bool left = false;
    bool right = false;
    int dx = 0;
    int dy = 0;
    kaos_sdl_get_mouse_state(dx, dy, left, right);
    mouse_dx += dx;
    mouse_dy += dy;
    mouse_left = left;
    mouse_right = right;
}

char kb_pressed(byte c)
{
    pump_keyboard();
    if (!c || c >= 129) return 0;

    const char pressed = static_cast<char>(KeyP[c] | Key[c]);
    KeyP[c] = 0;
    return pressed;
}

char kb_onepressed(byte c)
{
    pump_keyboard();
    if (!c || c >= 129) return 0;

    const char pressed = KeyP[c];
    KeyP[c] = 0;
    return pressed;
}

void kb_presskey(byte c)
{
    if (c < 129) {
        KeyP[c] = 1;
        Key[c] = 1;
    }
}

void kb_releasekey(byte c)
{
    if (c < 129)
        Key[c] = 0;
}

void kb_reset()
{
    std::memset(Key, 0, sizeof(Key));
    std::memset(KeyP, 0, sizeof(KeyP));
    mouse_dx = 0;
    mouse_dy = 0;
    mouse_left = false;
    mouse_right = false;
}

void kb_initkey()
{
    if (key_installed)
        return;

    if (!kaos_sdl_video_init(320, 200))
        return;

    key_installed = true;
    oldcall = false;
    kb_reset();
}

void kb_enablestd()
{
    oldcall = true;
}

void kb_disablestd()
{
    oldcall = false;
    kb_reset();
}

void kb_donekey()
{
    if (!key_installed)
        return;

    key_installed = false;
    kb_reset();
}

int kb_mouse_dx()
{
    pump_keyboard();
    const int value = mouse_dx;
    mouse_dx = 0;
    return value;
}

int kb_mouse_dy()
{
    pump_keyboard();
    const int value = mouse_dy;
    mouse_dy = 0;
    return value;
}

char kb_mouse_left()
{
    pump_keyboard();
    return mouse_left ? 1 : 0;
}

char kb_mouse_right()
{
    pump_keyboard();
    return mouse_right ? 1 : 0;
}
