////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

result_e platform_init(struct platform_init_info *init_info);
void platform_quit();

void platform_poll_events();
void platform_swap_buffers();

struct platform_callbacks* platform_get_callbacks();
const struct platform_info* platform_get_info();

f64 platform_time_elapsed();
u64 platform_time_elapsed_ms();

bool platform_hid_kbd_key_down(enum keyboard_key key);
bool platform_hid_mouse_button_down(enum mouse_button button);

void platform_win_resize(u32 width, u32 height);
void platform_win_set_vsync(bool state);
void platform_win_set_fullscreen(bool state);
void platform_win_set_mouse_capture(bool state);

string_cstr platform_get_user_home_dir();
