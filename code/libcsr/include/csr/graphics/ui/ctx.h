////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_ctx;

struct ui_ctx* ui_ctx_create(const char* name);
void ui_ctx_destroy(struct ui_ctx* ctx);

const char* ui_ctx_get_name(struct ui_ctx* ctx);

// menus
result_e ui_ctx_add_menu(struct ui_ctx *ctx, const char *key, struct ui_menu *menu);
struct ui_menu *ui_ctx_get_menu(struct ui_ctx *ctx, const char *key);

void ui_ctx_remove_menu(struct ui_ctx *ctx, struct ui_menu *menu);
void ui_ctx_remove_menu_by_key(struct ui_ctx *ctx, const char *key);

typedef void (*ui_ctx_traverse_menus_cb_t)(struct ui_menu *menu, void *data);
void ui_ctx_traverse_menus(struct ui_ctx* ctx, ui_ctx_traverse_menus_cb_t callback, void *data);

// windows
result_e ui_ctx_add_window(struct ui_ctx *ctx, const char *key, struct ui_window *window);
struct ui_window *ui_ctx_get_window(struct ui_ctx *ctx, const char *key);

void ui_ctx_remove_window(struct ui_ctx *ctx, struct ui_window *window);
void ui_ctx_remove_window_by_key(struct ui_ctx *ctx, const char *key);

typedef void (*ui_ctx_traverse_windows_cb_t)(struct ui_window *window, void *data);
void ui_ctx_traverse_windows(struct ui_ctx* ctx, ui_ctx_traverse_windows_cb_t callback, void *data);
