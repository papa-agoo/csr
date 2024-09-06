////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct applet;

struct ui_ctx;
struct hid_callbacks;

struct applet* applet_create(struct string path_to_file);
void applet_destroy(struct applet* applet);

result_e applet_startup(struct applet* applet);
void applet_shutdown(struct applet* applet);
void applet_tick(struct applet* applet, f64 dt);

struct string applet_get_name(struct applet* applet);
struct string applet_get_description(struct applet* applet);
struct string applet_get_version_str(struct applet* applet);
struct string applet_get_filename(struct applet* applet);

struct ui_ctx* applet_get_ui_ctx(struct applet *applet);
struct hid_callbacks* applet_get_hid_callbacks(struct applet *applet);
