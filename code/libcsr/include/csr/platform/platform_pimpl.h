////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

result_e platform_init_impl(struct platform_init_info *init_info, struct platform_callbacks *callbacks);
void platform_quit_impl();

void platform_get_info_impl(struct platform_info *info);

void platform_swap_buffers_impl();
void platform_poll_events_impl();

f64 platform_time_elapsed_impl();

bool platform_keyboard_key_down_impl(enum keyboard_key key);
bool platform_mouse_button_down_impl(enum mouse_button button);

void platform_win_resize_impl(u32 width, u32 height);
void platform_win_enable_vsync_impl(bool enable);
void platform_win_enable_fullscreen_impl(bool enable);
void platform_win_enable_mouse_capture_impl(bool enable);
