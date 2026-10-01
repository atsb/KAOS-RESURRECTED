#include "kaos_sdl.hpp"
#include <SDL3/SDL.h>
#include <array>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <thread>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <vector>
#include <deque>

static SDL_Window *g_window = nullptr;
static SDL_Renderer *g_renderer = nullptr;
static SDL_Texture *g_texture = nullptr;
#ifdef KAOS_EDITOR
static SDL_Surface *g_window_surface = nullptr;
static SDL_Surface *g_frame_surface = nullptr;
#endif
static int g_width = 320;
static int g_height = 200;
static bool g_video_initialized = false;
static bool g_audio_initialized = false;
static std::array<SDL_Color, 256> g_palette{};
static std::vector<Uint8> g_rgba_frame;
static std::deque<SDL_Event> g_event_queue;
static bool g_fullscreen = true;
static int g_mouse_dx = 0;
static int g_mouse_dy = 0;
static bool g_mouse_left = false;
static bool g_mouse_right = false;
static int g_pending_extended_key = -1;
static char g_video_error[512] = {};

static void kaos_set_video_error(const char *message)
{
    if (!message) message = "unknown SDL3 error";
    std::snprintf(g_video_error, sizeof(g_video_error), "%s", message);
}

static void init_black_palette()
{
    for (SDL_Color &c : g_palette)
        c = SDL_Color{0, 0, 0, 255};
}

bool kaos_sdl_video_init(int width, int height)
{
    if (width <= 0 || height <= 0) {
        kaos_set_video_error("invalid framebuffer size");
        return false;
    }

    g_width = width;
    g_height = height;
    g_video_error[0] = '\0';

    if (!g_video_initialized) {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
            kaos_set_video_error(SDL_GetError());
            return false;
        }
        g_video_initialized = true;
    }

    if (g_window
#ifdef KAOS_EDITOR
        && g_window_surface
#else
        && g_renderer && g_texture
#endif
        )
        return true;

#ifdef KAOS_EDITOR
    g_window = SDL_CreateWindow("KAOS Level Editor",
                                width * 3, height * 3,
                                SDL_WINDOW_RESIZABLE);
    if (!g_window) {
        kaos_set_video_error(SDL_GetError());
        goto fail;
    }

    g_window_surface = SDL_GetWindowSurface(g_window);
    if (!g_window_surface) {
        kaos_set_video_error(SDL_GetError());
        goto fail;
    }

    g_rgba_frame.resize(static_cast<std::size_t>(width) *
                        static_cast<std::size_t>(height) * 4u);
    std::fill(g_rgba_frame.begin(), g_rgba_frame.end(), 0);
    g_frame_surface = SDL_CreateSurfaceFrom(
        width, height, SDL_PIXELFORMAT_RGBA32,
        g_rgba_frame.data(), width * 4);
    if (!g_frame_surface) {
        kaos_set_video_error(SDL_GetError());
        goto fail;
    }
    SDL_SetSurfaceBlendMode(g_frame_surface, SDL_BLENDMODE_NONE);
    init_black_palette();
    SDL_SetWindowSurfaceVSync(g_window, 0);

     
    SDL_FillSurfaceRect(g_window_surface, nullptr,
                        SDL_MapSurfaceRGBA(g_window_surface, 0, 0, 0, 255));
    if (!SDL_UpdateWindowSurface(g_window)) {
        kaos_set_video_error(SDL_GetError());
        goto fail;
    }
    return true;
#else
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, SDL_SOFTWARE_RENDERER);

    g_window = SDL_CreateWindow("KAOS",
                                width * 3, height * 3,
                                SDL_WINDOW_RESIZABLE);
    if (!g_window) {
        kaos_set_video_error(SDL_GetError());
        goto fail;
    }

    g_renderer = SDL_CreateRenderer(g_window, SDL_SOFTWARE_RENDERER);
    if (!g_renderer)
        g_renderer = SDL_CreateRenderer(g_window, nullptr);
    if (!g_renderer) {
        kaos_set_video_error(SDL_GetError());
        goto fail;
    }

    SDL_SetRenderVSync(g_renderer, 0);

    if (!SDL_SetRenderLogicalPresentation(
            g_renderer, width, height,
            SDL_LOGICAL_PRESENTATION_STRETCH)) {
        kaos_set_video_error(SDL_GetError());
        goto fail;
    }

    g_texture = SDL_CreateTexture(g_renderer,
                                  SDL_PIXELFORMAT_RGBA32,
                                  SDL_TEXTUREACCESS_STREAMING,
                                  width, height);
    if (!g_texture) {
        kaos_set_video_error(SDL_GetError());
        goto fail;
    }

    if (!SDL_SetTextureScaleMode(g_texture, SDL_SCALEMODE_NEAREST)) {
        kaos_set_video_error(SDL_GetError());
        goto fail;
    }

#ifdef KAOS_GAME
    if (!SDL_SetWindowRelativeMouseMode(g_window, true)) {
        kaos_set_video_error(SDL_GetError());
        goto fail;
    }
#endif

    g_rgba_frame.resize(static_cast<std::size_t>(width) *
                        static_cast<std::size_t>(height) * 4u);
    init_black_palette();
    std::fill(g_rgba_frame.begin(), g_rgba_frame.end(), 0);
    SDL_UpdateTexture(g_texture, nullptr, g_rgba_frame.data(), width * 4);
    SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
    SDL_RenderClear(g_renderer);
    SDL_RenderTexture(g_renderer, g_texture, nullptr, nullptr);
    SDL_RenderPresent(g_renderer);
    return true;
#endif

fail:
    if (!g_video_error[0])
        kaos_set_video_error(SDL_GetError());
    kaos_sdl_video_shutdown();
    return false;
}

const char *kaos_sdl_video_error()
{
    return g_video_error[0] ? g_video_error : SDL_GetError();
}

bool kaos_sdl_set_window_title(const char *title)
{
    return g_window && title && SDL_SetWindowTitle(g_window, title);
}

char *kaos_sdl_get_user_path(const char *filename)
{
    if (!filename || !*filename)
        return nullptr;
    char *directory = SDL_GetPrefPath("NicoSoft", "KAOS");
    if (!directory)
        return nullptr;
    const std::size_t dir_len = std::strlen(directory);
    const std::size_t name_len = std::strlen(filename);
    char *path = static_cast<char *>(SDL_malloc(dir_len + name_len + 1));
    if (path) {
        std::memcpy(path, directory, dir_len);
        std::memcpy(path + dir_len, filename, name_len + 1);
    }
    SDL_free(directory);
    return path;
}

void kaos_sdl_video_shutdown()
{
    if (g_texture) {
        SDL_DestroyTexture(g_texture);
        g_texture = nullptr;
    }
#ifdef KAOS_EDITOR
    if (g_frame_surface) {
        SDL_DestroySurface(g_frame_surface);
        g_frame_surface = nullptr;
    }
    g_window_surface = nullptr;
#endif
    if (g_renderer) {
        SDL_DestroyRenderer(g_renderer);
        g_renderer = nullptr;
    }
    if (g_window) {
        SDL_DestroyWindow(g_window);
        g_window = nullptr;
    }
    g_event_queue.clear();
    g_pending_extended_key = -1;
    g_mouse_dx = 0;
    g_mouse_dy = 0;
    g_mouse_left = false;
    g_mouse_right = false;
    if (g_video_initialized) {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        g_video_initialized = false;
    }
}

void kaos_sdl_set_palette(const byte *palette, int first, int count)
{
    if (!palette || count <= 0 || first < 0 || first >= 256)
        return;

    const int n = std::min(count, 256 - first);
    for (int i = 0; i < n; ++i) {
        const int p = (first + i) * 3;
        const byte r = palette[p + 0];
        const byte g = palette[p + 1];
        const byte b = palette[p + 2];

        g_palette[first + i] = SDL_Color{
            static_cast<Uint8>((r << 2) | (r >> 4)),
            static_cast<Uint8>((g << 2) | (g >> 4)),
            static_cast<Uint8>((b << 2) | (b >> 4)),
            255
        };
    }
}

void kaos_sdl_present(const byte *pixels)
{
    if (!g_window || !pixels)
        return;

#ifdef KAOS_EDITOR
    g_window_surface = SDL_GetWindowSurface(g_window);
    if (!g_window_surface || !g_window_surface->pixels)
        return;

    const int ww = g_window_surface->w;
    const int wh = g_window_surface->h;
    if (ww <= 0 || wh <= 0)
        return;

    const int scale_x = ww / g_width;
    const int scale_y = wh / g_height;
    const int scale = std::max(1, std::min(scale_x, scale_y));
    const int draw_w = g_width * scale;
    const int draw_h = g_height * scale;
    const int origin_x = (ww - draw_w) / 2;
    const int origin_y = (wh - draw_h) / 2;

    const SDL_PixelFormatDetails *details =
        SDL_GetPixelFormatDetails(g_window_surface->format);
    if (!details || details->bytes_per_pixel < 1 ||
        details->bytes_per_pixel > 4)
        return;

    if (SDL_MUSTLOCK(g_window_surface) && !SDL_LockSurface(g_window_surface))
        return;

     

    Uint32 mapped_palette[256];
    for (int i = 0; i < 256; ++i) {
        const SDL_Color &c = g_palette[i];
        mapped_palette[i] = SDL_MapSurfaceRGBA(
            g_window_surface, c.r, c.g, c.b, 255);
    }

    const Uint32 black = SDL_MapSurfaceRGBA(g_window_surface, 0, 0, 0, 255);
    SDL_FillSurfaceRect(g_window_surface, nullptr, black);

    const int bpp = details->bytes_per_pixel;
    const int pitch = g_window_surface->pitch;
    byte *dst_base = static_cast<byte *>(g_window_surface->pixels);

     

    for (int sy = 0; sy < g_height; ++sy) {
        const byte *src = pixels + sy * g_width;
        const int dy0 = origin_y + sy * scale;
        for (int sx = 0; sx < g_width; ++sx) {
            const Uint32 mapped = mapped_palette[src[sx]];
            const int dx0 = origin_x + sx * scale;

            for (int oy = 0; oy < scale; ++oy) {
                byte *row = dst_base + (dy0 + oy) * pitch + dx0 * bpp;
                for (int ox = 0; ox < scale; ++ox) {
                    byte *d = row + ox * bpp;
                    switch (bpp) {
                    case 1:
                        d[0] = static_cast<byte>(mapped);
                        break;
                    case 2: {
                        const std::uint16_t v = static_cast<std::uint16_t>(mapped);
                        std::memcpy(d, &v, sizeof(v));
                        break;
                    }
                    case 3:
                        std::memcpy(d, &mapped, 3);
                        break;
                    case 4:
                        std::memcpy(d, &mapped, sizeof(mapped));
                        break;
                    }
                }
            }
        }
    }

    if (SDL_MUSTLOCK(g_window_surface))
        SDL_UnlockSurface(g_window_surface);

    if (!SDL_UpdateWindowSurface(g_window)) {
        kaos_set_video_error(SDL_GetError());
    }
#else
    if (!g_renderer || !g_texture)
        return;

    const std::size_t pixel_count =
        static_cast<std::size_t>(g_width) *
        static_cast<std::size_t>(g_height);

    if (g_rgba_frame.size() != pixel_count * 4u)
        g_rgba_frame.resize(pixel_count * 4u);

    for (std::size_t i = 0; i < pixel_count; ++i) {
        const SDL_Color &c = g_palette[pixels[i]];
        Uint8 *dst = &g_rgba_frame[i * 4u];
        dst[0] = c.r;
        dst[1] = c.g;
        dst[2] = c.b;
        dst[3] = 255;
    }

    if (!SDL_UpdateTexture(g_texture, nullptr,
                           g_rgba_frame.data(), g_width * 4)) {
        kaos_set_video_error(SDL_GetError());
        return;
    }
    if (!SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255) ||
        !SDL_RenderClear(g_renderer) ||
        !SDL_RenderTexture(g_renderer, g_texture, nullptr, nullptr) ||
        !SDL_RenderPresent(g_renderer)) {
        kaos_set_video_error(SDL_GetError());
    }
#endif
}

void kaos_sdl_pump_events()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT ||
            event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
            ShutDown = 1;
            continue;
        }
        if (event.type == SDL_EVENT_KEY_DOWN ||
            event.type == SDL_EVENT_KEY_UP) {
            g_event_queue.push_back(event);
            if (g_event_queue.size() > 256)
                g_event_queue.pop_front();
        }
    }

    if (g_window) {
        float dx = 0.0f;
        float dy = 0.0f;
        const SDL_MouseButtonFlags buttons = SDL_GetRelativeMouseState(&dx, &dy);
        g_mouse_dx += static_cast<int>(dx);
        g_mouse_dy += static_cast<int>(dy);
        g_mouse_left = (buttons & SDL_BUTTON_LMASK) != 0;
        g_mouse_right = (buttons & SDL_BUTTON_RMASK) != 0;
    }
}

bool kaos_sdl_poll_event(SDL_Event &event)
{
    if (g_event_queue.empty())
        return false;
    event = g_event_queue.front();
    g_event_queue.pop_front();
    return true;
}

bool kaos_sdl_audio_init()
{
    if (!g_audio_initialized) {
        if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
            return false;
        g_audio_initialized = true;
    }
    return true;
}

void kaos_sdl_audio_shutdown()
{
    if (g_audio_initialized) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        g_audio_initialized = false;
    }
}


void kaos_sdl_get_window_size(int &width, int &height)
{
    if (g_window)
        SDL_GetWindowSize(g_window, &width, &height);
}

void kaos_sdl_warp_mouse(int x, int y)
{
    if (!g_window)
        return;

    int w = 320, h = 200;
    SDL_GetWindowSize(g_window, &w, &h);
    SDL_WarpMouseInWindow(g_window,
                          static_cast<float>(x) * w / 320.0f,
                          static_cast<float>(y) * h / 200.0f);
}

bool kaos_sdl_set_fullscreen(bool enabled)
{
    if (!g_window)
        return false;
    if (!SDL_SetWindowFullscreen(g_window, enabled))
        return false;
    g_fullscreen = enabled;
    return true;
}

bool kaos_sdl_get_fullscreen()
{
    return g_fullscreen;
}

void kaos_sdl_toggle_fullscreen()
{
    kaos_sdl_set_fullscreen(!g_fullscreen);
}

void kaos_sdl_get_mouse_state(int &dx, int &dy, bool &left, bool &right)
{
    dx = g_mouse_dx;
    dy = g_mouse_dy;
    left = g_mouse_left;
    right = g_mouse_right;
    g_mouse_dx = 0;
    g_mouse_dy = 0;
}

static int kaos_key_to_ascii(SDL_Scancode sc)
{
    switch (sc) {
    case SDL_SCANCODE_ESCAPE: return 27;
    case SDL_SCANCODE_RETURN:
    case SDL_SCANCODE_KP_ENTER: return 13;
    case SDL_SCANCODE_BACKSPACE: return 8;
    case SDL_SCANCODE_TAB: return 9;
    case SDL_SCANCODE_SPACE: return ' ';
    case SDL_SCANCODE_UP: return 0x48;
    case SDL_SCANCODE_DOWN: return 0x50;
    case SDL_SCANCODE_LEFT: return 0x4B;
    case SDL_SCANCODE_RIGHT: return 0x4D;
    case SDL_SCANCODE_A: return 'a';
    case SDL_SCANCODE_D: return 'd';
    case SDL_SCANCODE_E: return 'e';
    case SDL_SCANCODE_F: return 'f';
    case SDL_SCANCODE_H: return 'h';
    case SDL_SCANCODE_J: return 'j';
    case SDL_SCANCODE_K: return 'k';
    case SDL_SCANCODE_L: return 'l';
    case SDL_SCANCODE_M: return 'm';
    case SDL_SCANCODE_O: return 'o';
    case SDL_SCANCODE_P: return 'p';
    case SDL_SCANCODE_Q: return 'q';
    case SDL_SCANCODE_W: return 'w';
    case SDL_SCANCODE_S: return 's';
    case SDL_SCANCODE_U: return 'u';
    case SDL_SCANCODE_X: return 'x';
    case SDL_SCANCODE_Z: return 'z';
    case SDL_SCANCODE_0: return '0';
    case SDL_SCANCODE_1: return '1';
    case SDL_SCANCODE_2: return '2';
    case SDL_SCANCODE_3: return '3';
    case SDL_SCANCODE_4: return '4';
    case SDL_SCANCODE_5: return '5';
    case SDL_SCANCODE_6: return '6';
    case SDL_SCANCODE_7: return '7';
    case SDL_SCANCODE_8: return '8';
    case SDL_SCANCODE_9: return '9';
    case SDL_SCANCODE_MINUS: return '-';
    case SDL_SCANCODE_COMMA: return ',';
    case SDL_SCANCODE_PERIOD: return '.';
    case SDL_SCANCODE_SLASH: return '/';
    case SDL_SCANCODE_APOSTROPHE: return '\'';
    default: return 0;
    }
}

int kaos_kbhit()
{
     
    return (g_pending_extended_key >= 0 || !g_event_queue.empty()) ? 1 : 0;
}

int kaos_getch()
{
    if (g_pending_extended_key >= 0) {
        const int key = g_pending_extended_key;
        g_pending_extended_key = -1;
        return key;
    }

    SDL_Event event;
    while (kaos_sdl_poll_event(event)) {
        if (event.type != SDL_EVENT_KEY_DOWN)
            continue;

        switch (event.key.scancode) {
        case SDL_SCANCODE_UP:
        case SDL_SCANCODE_DOWN:
        case SDL_SCANCODE_LEFT:
        case SDL_SCANCODE_RIGHT:
            g_pending_extended_key = kaos_key_to_ascii(event.key.scancode);
            return 0;
        default:
            return kaos_key_to_ascii(event.key.scancode);
        }
    }
    return 0;
}

 


















static std::uint32_t g_borland_seed = 1;

int kaos_rand()
{
     
    g_borland_seed = g_borland_seed * 0x015A4E35u + 1u;
    return static_cast<int>((g_borland_seed >> 16) & 0x7FFFu);
}

void kaos_srand(unsigned seed)
{
    g_borland_seed = static_cast<std::uint32_t>(static_cast<std::uint16_t>(seed));
}

int kaos_random(int maximum)
{
     
    const int num = static_cast<std::int16_t>(maximum);
    if (num <= 0)
        return 0;
    const std::int32_t r = static_cast<std::int32_t>(kaos_rand());
    return static_cast<int>((r * static_cast<std::int32_t>(num)) /
                            (static_cast<std::int32_t>(0x7FFFu) + 1));
}

unsigned kaos_random_seed()
{
    return static_cast<unsigned>(g_borland_seed);
}

void kaos_randomize()
{
     


    const auto t = static_cast<std::time_t>(std::time(nullptr));
    kaos_srand(static_cast<unsigned>(t) & 0xFFFFu);
}

