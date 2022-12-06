////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "service/core.h"
#include "service/video.h"
#include "service/audio.h"

////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// logging
////////////////////////////////////////////////////////////////////////////////
#ifndef KLOG_MODULE_NAME
#define KLOG_MODULE_NAME generic
#endif

#define _klog_module(m) _mstring(m)

#define _klog_msg(level, msg, ...) \
    _mcall(kio_log_message(level, _klog_module(KLOG_MODULE_NAME), msg, ##__VA_ARGS__))

#define klog_info(msg, ...) _klog_msg(LOG_LEVEL_INFO, msg, ##__VA_ARGS__)
#define klog_notice(msg, ...) _klog_msg(LOG_LEVEL_NOTICE, msg, ##__VA_ARGS__)
#define klog_warn(msg, ...) _klog_msg(LOG_LEVEL_WARN, msg, ##__VA_ARGS__)
#define klog_error(msg, ...) _klog_msg(LOG_LEVEL_ERROR, msg, ##__VA_ARGS__)
#define klog_trace(msg, ...) _klog_msg(LOG_LEVEL_TRACE, msg, ##__VA_ARGS__)

struct log_db* kio_log_get_db();
void kio_log_message(enum log_level_type level, const char* module, const char* message, ...);


////////////////////////////////////////////////////////////////////////////////
// time
////////////////////////////////////////////////////////////////////////////////
f64 kio_time_elapsed();
f64 kio_time_elapsed_delta();
u64 kio_time_elapsed_hires();


////////////////////////////////////////////////////////////////////////////////
// hid
////////////////////////////////////////////////////////////////////////////////
bool kio_hid_kbd_key_down(enum keyboard_key key);
bool kio_hid_mouse_button_down(enum mouse_button button);
void kio_hid_enable_mouse_capture(bool enable);


////////////////////////////////////////////////////////////////////////////////
// video
////////////////////////////////////////////////////////////////////////////////
void kio_video_enable_vsync(bool enable);
void kio_video_toggle_vsync();

void kio_video_enable_fullscreen(bool enable);
void kio_video_toggle_fullscreen();

struct vec2 kio_video_get_window_resolution();
void kio_video_set_window_resolution(struct vec2 resolution);

struct vec2 kio_video_get_display_resolution();

f32 kio_video_get_content_scale();

xgl_framebuffer kio_video_get_framebuffer();


////////////////////////////////////////////////////////////////////////////////
// audio
////////////////////////////////////////////////////////////////////////////////

// ...
