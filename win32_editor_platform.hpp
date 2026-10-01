#pragma once

#include "std.hpp"

#if !defined(_WIN32)
#error "win32_editor_platform.hpp is Windows-only"
#endif

namespace kaos_editor_win32 {

bool init(int width, int height, const char *title);
void shutdown();
void set_title(const char *title);
void set_palette(const byte *palette, int first, int count);
void present(const byte *pixels);
void pump_events();
int kbhit();
int getch();
void get_mouse(int &x, int &y, byte &buttons);
void warp_mouse(int x, int y);
const char *executable_directory();

}  
