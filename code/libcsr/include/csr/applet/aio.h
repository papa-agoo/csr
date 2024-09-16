////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/clock.h>
#include <csr/core/config.h>

#include <csr/kernel/log_db.h>
#include <csr/kernel/events/event.h>

#include <csr/graphics/ui.h>
#include <csr/graphics/screen.h>

////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// logging
////////////////////////////////////////////////////////////////////////////////
#define alog_info(msg, ...) _mcall(aio_log_message(LOG_LEVEL_INFO, msg, ##__VA_ARGS__))
#define alog_notice(msg, ...) _mcall(aio_log_message(LOG_LEVEL_NOTICE, msg, ##__VA_ARGS__))
#define alog_warn(msg, ...) _mcall(aio_log_message(LOG_LEVEL_WARN, msg, ##__VA_ARGS__))
#define alog_error(msg, ...) _mcall(aio_log_message(LOG_LEVEL_ERROR, msg, ##__VA_ARGS__))
#define alog_trace(msg, ...) _mcall(aio_log_message(LOG_LEVEL_TRACE, msg, ##__VA_ARGS__))

void aio_log_message(enum log_level_type level, string_cstr message, ...);


////////////////////////////////////////////////////////////////////////////////
// env vars
////////////////////////////////////////////////////////////////////////////////
struct string aio_env_get_var(string_cstr name);
string_cstr aio_env_get_var_cstr(string_cstr name);

struct string aio_env_expand_str(string_cstr str);
string_cstr aio_env_expand_str_cstr(string_cstr str);


////////////////////////////////////////////////////////////////////////////////
// memory
////////////////////////////////////////////////////////////////////////////////
struct arena* aio_get_main_arena();
struct arena* aio_get_frame_arena();


////////////////////////////////////////////////////////////////////////////////
// user config
////////////////////////////////////////////////////////////////////////////////
struct config* aio_get_config();


////////////////////////////////////////////////////////////////////////////////
// callbacks
////////////////////////////////////////////////////////////////////////////////
struct hid_callbacks* aio_get_hid_callbacks();


////////////////////////////////////////////////////////////////////////////////
// time
////////////////////////////////////////////////////////////////////////////////
struct clock* aio_get_clock();
f64 aio_time_elapsed();
f64 aio_time_elapsed_delta();


////////////////////////////////////////////////////////////////////////////////
// user interface
////////////////////////////////////////////////////////////////////////////////
result_e aio_add_ui_menu(string_cstr key, struct ui_menu *menu);
struct ui_menu* aio_get_ui_menu(string_cstr key);

result_e aio_add_ui_window(string_cstr  key, struct ui_window *window);
struct ui_window* aio_get_ui_window(string_cstr  key);


////////////////////////////////////////////////////////////////////////////////
// video
////////////////////////////////////////////////////////////////////////////////
struct screen* aio_add_screen(string_cstr key, struct screen_create_info *ci);
struct screen* aio_get_screen(string_cstr key);


////////////////////////////////////////////////////////////////////////////////
// audio
////////////////////////////////////////////////////////////////////////////////
// ...
