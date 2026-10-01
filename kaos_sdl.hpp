#pragma once
#ifndef KAOS_SDL_HPP
#define KAOS_SDL_HPP

union SDL_Event;


#include "std.hpp"

bool kaos_sdl_video_init(int width, int height);
bool kaos_sdl_set_window_title(const char *title);
const char *kaos_sdl_video_error();
void kaos_sdl_video_shutdown();
void kaos_sdl_present(const byte *pixels);
void kaos_sdl_set_palette(const byte *palette, int first, int count);
void kaos_sdl_pump_events();
bool kaos_sdl_poll_event(SDL_Event &event);
void kaos_sdl_get_window_size(int &width, int &height);
void kaos_sdl_warp_mouse(int x, int y);
bool kaos_sdl_set_fullscreen(bool enabled);
bool kaos_sdl_get_fullscreen();
void kaos_sdl_toggle_fullscreen();
void kaos_sdl_get_mouse_state(int &dx, int &dy, bool &left, bool &right);
char *kaos_sdl_get_user_path(const char *filename);

bool kaos_sdl_audio_init();
void kaos_sdl_audio_shutdown();

#endif

int kaos_kbhit();
int kaos_getch();
int kaos_rand();
int kaos_random(int maximum);
void kaos_randomize();
void kaos_srand(unsigned seed);
unsigned kaos_random_seed();

void kaos_sdl_start_sync();
void kaos_sdl_enable_sync();
void kaos_sdl_disable_sync();
void kaos_sdl_stop_sync();
