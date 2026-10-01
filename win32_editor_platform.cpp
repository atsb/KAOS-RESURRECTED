#include "win32_editor_platform.hpp"

#define NOMINMAX
#include <windows.h>
#include <windowsx.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace kaos_editor_win32 {
namespace {

constexpr int kWidth = 320;
constexpr int kHeight = 200;
constexpr int kScale = 3;
constexpr wchar_t kClassName[] = L"KAOS_Editor_320x200";

HWND g_hwnd = nullptr;
HINSTANCE g_instance = nullptr;
BITMAPINFO g_bmi{};
std::vector<std::uint8_t> g_bgra;
std::array<std::uint8_t, 768> g_palette{};
std::array<std::uint8_t, 1024> g_key_queue{};
std::size_t g_key_head = 0;
std::size_t g_key_tail = 0;
std::string g_exe_dir;
bool g_initialized = false;


bool queue_byte(std::uint8_t value)
{
    const std::size_t next = (g_key_tail + 1u) % g_key_queue.size();
    if (next == g_key_head)
        return false;
    g_key_queue[g_key_tail] = value;
    g_key_tail = next;
    return true;
}

bool dequeue_byte(std::uint8_t &value)
{
    if (g_key_head == g_key_tail)
        return false;
    value = g_key_queue[g_key_head];
    g_key_head = (g_key_head + 1u) % g_key_queue.size();
    return true;
}

bool is_extended_vk(WPARAM vk, std::uint8_t &scan)
{
    switch (vk) {
    case VK_UP:     scan = 72; return true;
    case VK_DOWN:   scan = 80; return true;
    case VK_LEFT:   scan = 75; return true;
    case VK_RIGHT:  scan = 77; return true;
    case VK_HOME:   scan = 71; return true;
    case VK_END:    scan = 79; return true;
    case VK_PRIOR:  scan = 73; return true;
    case VK_NEXT:   scan = 81; return true;
    case VK_INSERT: scan = 82; return true;
    case VK_DELETE: scan = 83; return true;
    case VK_F1:     scan = 59; return true;
    case VK_F2:     scan = 60; return true;
    case VK_F3:     scan = 61; return true;
    case VK_F4:     scan = 62; return true;
    case VK_F5:     scan = 63; return true;
    case VK_F6:     scan = 64; return true;
    case VK_F7:     scan = 65; return true;
    case VK_F8:     scan = 66; return true;
    case VK_F9:     scan = 67; return true;
    case VK_F10:    scan = 68; return true;
    case VK_F11:    scan = 133; return true;
    case VK_F12:    scan = 134; return true;
    default: return false;
    }
}

void update_dib()
{
    g_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    g_bmi.bmiHeader.biWidth = kWidth;
    g_bmi.bmiHeader.biHeight = -kHeight;  
    g_bmi.bmiHeader.biPlanes = 1;
    g_bmi.bmiHeader.biBitCount = 32;
    g_bmi.bmiHeader.biCompression = BI_RGB;
    g_bmi.bmiHeader.biSizeImage = static_cast<DWORD>(kWidth * kHeight * 4);
}

void build_frame(const byte *pixels)
{
    if (!pixels || g_bgra.size() != static_cast<std::size_t>(kWidth * kHeight * 4))
        return;

    for (int i = 0; i < kWidth * kHeight; ++i) {
        const int p = static_cast<int>(pixels[i]) * 3;
        const std::uint8_t r = static_cast<std::uint8_t>(g_palette[p + 0] << 2);
        const std::uint8_t g = static_cast<std::uint8_t>(g_palette[p + 1] << 2);
        const std::uint8_t b = static_cast<std::uint8_t>(g_palette[p + 2] << 2);
        const std::size_t d = static_cast<std::size_t>(i) * 4u;
        g_bgra[d + 0] = b;
        g_bgra[d + 1] = g;
        g_bgra[d + 2] = r;
        g_bgra[d + 3] = 0;
    }
}

void paint_window(HDC dc)
{
    if (!dc || g_bgra.empty())
        return;

    RECT client{};
    GetClientRect(g_hwnd, &client);
    const int width = std::max(1, static_cast<int>(client.right - client.left));
    const int height = std::max(1, static_cast<int>(client.bottom - client.top));

    SetStretchBltMode(dc, COLORONCOLOR);
    StretchDIBits(dc,
                  0, 0, width, height,
                  0, 0, kWidth, kHeight,
                  g_bgra.data(), &g_bmi,
                  DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_ERASEBKGND:
        return 1;  

    case WM_SETCURSOR:
        if (LOWORD(lParam) == HTCLIENT) {
            SetCursor(nullptr);  
            return TRUE;
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps{};
        HDC dc = BeginPaint(hwnd, &ps);
        paint_window(dc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_KEYDOWN: {
        std::uint8_t scan = 0;
        if (wParam == VK_ESCAPE) {
            queue_byte(27);
        } else if (is_extended_vk(wParam, scan)) {
            queue_byte(0);
            queue_byte(scan);
        }
        return 0;
    }

    case WM_CHAR:
         

        if (wParam <= 0xffu && wParam != 27u)
            queue_byte(static_cast<std::uint8_t>(wParam));
        return 0;

    case WM_CLOSE:
        ShutDown = 1;
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        ShutDown = 1;
        PostQuitMessage(0);
        return 0;

    default:
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

}  

bool init(int width, int height, const char *title)
{
    if (g_initialized)
        return true;
    if (width != kWidth || height != kHeight)
        return false;

     


    SetProcessDPIAware();

    g_instance = GetModuleHandleA(nullptr);
    if (!g_instance)
        return false;

    WNDCLASSW wc{};
    wc.lpfnWndProc = window_proc;
    wc.hInstance = g_instance;
    wc.lpszClassName = kClassName;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        return false;

    RECT wanted{0, 0, width * kScale, height * kScale};
    if (!AdjustWindowRectEx(&wanted,
                           WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                           FALSE, 0))
        return false;

    const int win_w = wanted.right - wanted.left;
    const int win_h = wanted.bottom - wanted.top;
    g_hwnd = CreateWindowExW(0, kClassName, L"KAOS Level Editor",
                             WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                             CW_USEDEFAULT, CW_USEDEFAULT, win_w, win_h,
                             nullptr, nullptr, g_instance, nullptr);
    if (!g_hwnd)
        return false;

    if (title)
        set_title(title);

    g_bgra.assign(static_cast<std::size_t>(width * height * 4), 0);
    g_palette.fill(0);
    update_dib();
    g_key_head = g_key_tail = 0;
    g_exe_dir.clear();

    char exe[32768]{};
    const DWORD n = GetModuleFileNameA(g_instance, exe, static_cast<DWORD>(sizeof(exe)));
    if (n) {
        std::string path(exe, exe + n);
        const std::string::size_type slash = path.find_last_of("\\/");
        g_exe_dir = (slash == std::string::npos) ? std::string(".") : path.substr(0, slash);
    } else {
        g_exe_dir = ".";
    }

    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);
    SetForegroundWindow(g_hwnd);
    SetFocus(g_hwnd);

    g_initialized = true;
    return true;
}

void shutdown()
{
    if (g_hwnd) {
        DestroyWindow(g_hwnd);
        g_hwnd = nullptr;
    }
    g_bgra.clear();
    g_key_head = g_key_tail = 0;
    g_initialized = false;
}

void set_title(const char *title)
{
    if (!g_hwnd || !title)
        return;
    SetWindowTextA(g_hwnd, title);
}

void set_palette(const byte *palette, int first, int count)
{
    if (!palette || first < 0 || first >= 256 || count <= 0)
        return;
    const int n = std::min(count, 256 - first);
    std::memcpy(g_palette.data() + static_cast<std::size_t>(first) * 3u,
                palette + static_cast<std::size_t>(first) * 3u,
                static_cast<std::size_t>(n) * 3u);
}

void present(const byte *pixels)
{
    if (!g_hwnd || !pixels)
        return;
    build_frame(pixels);
    InvalidateRect(g_hwnd, nullptr, FALSE);
    UpdateWindow(g_hwnd);
}

void pump_events()
{
    MSG msg{};
    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            ShutDown = 1;
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

int kbhit()
{
    pump_events();
    return g_key_head != g_key_tail ? 1 : 0;
}

int getch()
{
    for (;;) {
        pump_events();
        std::uint8_t value = 0;
        if (dequeue_byte(value))
            return value;
        if (ShutDown)
            return 27;
        WaitMessage();
    }
}

void get_mouse(int &x, int &y, byte &buttons)
{
    x = 160;
    y = 100;
    buttons = 0;

    if (!g_hwnd)
        return;

    POINT pt{};
    if (!GetCursorPos(&pt) || !ScreenToClient(g_hwnd, &pt))
        return;

    RECT client{};
    GetClientRect(g_hwnd, &client);
    const int cw = std::max(1, static_cast<int>(client.right - client.left));
    const int ch = std::max(1, static_cast<int>(client.bottom - client.top));
    const int logical_x = static_cast<int>((pt.x * kWidth) / cw);
    const int logical_y = static_cast<int>((pt.y * kHeight) / ch);
    x = std::clamp(logical_x, 0, kWidth - 1);
    y = std::clamp(logical_y, 0, kHeight - 1);

    if (GetForegroundWindow() == g_hwnd) {
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) buttons |= 0x01;
        if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) buttons |= 0x02;
        if (GetAsyncKeyState(VK_MBUTTON) & 0x8000) buttons |= 0x04;
    }
}

void warp_mouse(int x, int y)
{
    if (!g_hwnd)
        return;

    x = std::clamp(x, 0, kWidth - 1);
    y = std::clamp(y, 0, kHeight - 1);

    RECT client{};
    GetClientRect(g_hwnd, &client);
    const int cw = std::max(1, static_cast<int>(client.right - client.left));
    const int ch = std::max(1, static_cast<int>(client.bottom - client.top));
    POINT pt{
        (x * cw) / kWidth,
        (y * ch) / kHeight
    };
    ClientToScreen(g_hwnd, &pt);
    SetCursorPos(pt.x, pt.y);
}

const char *executable_directory()
{
    return g_exe_dir.c_str();
}

}  

int kaos_kbhit()
{
    return kaos_editor_win32::kbhit();
}

int kaos_getch()
{
    return kaos_editor_win32::getch();
}
