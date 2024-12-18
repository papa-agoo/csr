////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/ctx.h>

#include <csr/core/list.h>
#include <csr/core/hashmap.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct ui_ctx
{
    struct string name;

    struct list* menus;
    struct list* windows;

    struct hashmap* ui_items;
};

////////////////////////////////////////////////////////////////////////////////

struct ui_ctx* ui_ctx_create(struct string name)
{
    check_expr(string_is_valid(name));

    struct ui_ctx* ctx = calloc(1, sizeof(struct ui_ctx));
    check_mem(ctx);

    ctx->name = name;

    ctx->menus = list_create();
    check_ptr(ctx->menus);

    ctx->windows = list_create();
    check_ptr(ctx->windows);

    ctx->ui_items = hashmap_create();
    check_ptr(ctx->ui_items);

    return ctx;

error:
    ui_ctx_destroy(ctx);

    return NULL;
}

void ui_ctx_destroy(struct ui_ctx* ctx)
{
    check_ptr(ctx);

    list_destroy(ctx->menus, NULL);
    list_destroy(ctx->windows, NULL);

    hashmap_destroy(ctx->ui_items);

    free(ctx);

error:
    return;
}

struct string ui_ctx_get_name(struct ui_ctx* ctx)
{
    check_ptr(ctx);

    return ctx->name;

error:
    return make_string_invalid();
}


////////////////////////////////////////////////////////////////////////////////
// menus
////////////////////////////////////////////////////////////////////////////////
u32 ui_ctx_get_menu_count(struct ui_ctx *ctx)
{
    check_ptr(ctx);

    return list_count(ctx->menus);

error:
    return 0;
}

result_e ui_ctx_add_menu(struct ui_ctx *ctx, string_cstr key, struct ui_menu *menu)
{
    check_ptr(ctx);
    check_ptr(key);
    check_ptr(menu);

    check_expr(!hashmap_has(ctx->ui_items, key));

    menu->key = strdup(key);

    list_push_back(ctx->menus, menu);

    hashmap_set(ctx->ui_items, key, menu);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

struct ui_menu *ui_ctx_get_menu(struct ui_ctx *ctx, string_cstr key)
{
    return hashmap_get(ctx->ui_items, key);
}

void ui_ctx_remove_menu(struct ui_ctx *ctx, struct ui_menu *menu)
{
    check_ptr(ctx);
    check_ptr(menu);

    list_remove_by_value(ctx->menus, menu);

    hashmap_set(ctx->ui_items, menu->key, NULL);

error:
    return;
}

void ui_ctx_remove_menu_by_key(struct ui_ctx *ctx, string_cstr key)
{
    ui_ctx_remove_menu(ctx, ui_ctx_get_menu(ctx, key));
}

void ui_ctx_traverse_menus(struct ui_ctx* ctx, ui_ctx_traverse_menus_cb_t callback, void *data)
{
    check_ptr(ctx);
    check_ptr(callback);

    LIST_FOREACH(ctx->menus, first, next, current) {
        callback(current->value, data);
    }

error:
    return;
}

struct ui_menu *ui_ctx_find_menu(struct ui_ctx *ctx, ui_ctx_find_menu_cb_t callback, void *data)
{
    check_ptr(ctx);
    check_ptr(callback);

    LIST_FOREACH(ctx->menus, first, next, current)
    {
        struct ui_menu *menu = current->value;

        if (callback(menu, data)) {
            return menu;
        }
    }

error:
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////
// windows
////////////////////////////////////////////////////////////////////////////////
u32 ui_ctx_get_window_count(struct ui_ctx *ctx)
{
    check_ptr(ctx);

    return list_count(ctx->windows);

error:
    return 0;
}

result_e ui_ctx_add_window(struct ui_ctx *ctx, string_cstr key, struct ui_window *window)
{
    check_ptr(ctx);
    check_ptr(key);
    check_ptr(window);

    check_expr(!hashmap_has(ctx->ui_items, key));

    window->key = strdup(key);

    list_push_back(ctx->windows, window);

    hashmap_set(ctx->ui_items, key, window);

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

struct ui_window *ui_ctx_get_window(struct ui_ctx *ctx, string_cstr key)
{
    return hashmap_get(ctx->ui_items, key);
}

void ui_ctx_remove_window(struct ui_ctx *ctx, struct ui_window *window)
{
    check_ptr(ctx);
    check_ptr(window);

    list_remove_by_value(ctx->windows, window);

    hashmap_set(ctx->ui_items, window->key, NULL);

error:
    return;
}

void ui_ctx_remove_window_by_key(struct ui_ctx *ctx, string_cstr key)
{
    ui_ctx_remove_window(ctx, ui_ctx_get_window(ctx, key));
}

void ui_ctx_traverse_windows(struct ui_ctx* ctx, ui_ctx_traverse_windows_cb_t callback, void *data)
{
    check_ptr(ctx);
    check_ptr(callback);

    LIST_FOREACH(ctx->windows, first, next, current) {
        callback(current->value, data);
    }

error:
    return;
}

struct ui_window *ui_ctx_find_window(struct ui_ctx *ctx, ui_ctx_find_window_cb_t callback, void *data)
{
    check_ptr(ctx);
    check_ptr(callback);

    LIST_FOREACH(ctx->windows, first, next, current)
    {
        struct ui_window *window = current->value;

        if (callback(window, data)) {
            return window;
        }
    }

error:
    return NULL;
}
