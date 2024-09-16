////////////////////////////////////////////////////////////////////////////////////////////////////

#include "main.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

extern bool g_animate_cube;
extern bool g_update_shader_bindings;

extern u32 g_sampler_idx;
extern xgl_sampler g_samplers[3];

extern u32 g_texture_idx;
extern struct texture_info g_textures[];
extern const size_t g_textures_count;

extern struct my_cube g_cube;

////////////////////////////////////////////////////////////////////////////////

static void _draw_textures_combo_list(string_cstr name, string_cstr current_item)
{
    if (igBeginCombo(name, current_item, 0))
    {
        for (u32 i = 0; i < g_textures_count; i++)
        {
            struct texture_info *info = &g_textures[i];

            bool is_selected = (current_item == info->filename);

            if (igSelectable_Bool(info->filename, is_selected, 0, (struct ImVec2){0}))
            {
                g_texture_idx = i;
                g_cube.material.texture = g_textures[g_texture_idx].texture;

                g_update_shader_bindings = true;
            }
        }

        igEndCombo();
    }
}

static void _draw_settings_view_cb(struct ui_view *view, struct ui_style *style)
{
    csr_assert(view);
    csr_assert(style);

    struct texture_info *info = &g_textures[g_texture_idx];

    ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_DefaultOpen;

    igSeparatorText("Texture");
    {
        string_cstr current_item = g_textures[g_texture_idx].filename;

        _draw_textures_combo_list("Texture", current_item);

        string_cstr items[] = {
            "Nearest",
            "Linear",
            "Anisotropic 16x",
        };

        igNewLine();

        u32 image_width = info->image->width;
        u32 image_height = info->image->width;
        u32 image_byte_length = info->image->num_pixels * 4;

        igText(" - Dimension      : %dx%d", image_width, image_height);
        igText(" - Size           : %.1fk", image_byte_length / 1024.0);

        igNewLine();

        u32 sampler_idx = g_sampler_idx;

        if (igCombo_Str_arr("Sampler", &sampler_idx, items, 3, 3))
        {
            // FIXME xgl check for anisotropic filtering support
            g_sampler_idx = sampler_idx;
            g_cube.material.sampler = g_samplers[g_sampler_idx];

            g_update_shader_bindings = true;
        }

        igNewLine();
    }

    igSeparatorText("Options");
    {
        f32* texture_scale = &g_cube.material.shader_params.cpu.texture_scale;

        if (igInputFloat("UV Scale", texture_scale, 0.1, 1.0, "%.2f", 0))
        {
            *texture_scale = clamp(*texture_scale, 0.1f, 5.0f);
        }

        igCheckbox("Animate Cube", &g_animate_cube);
    }
}

static void _draw_view_menu_cb(struct ui_menu *menu, struct ui_style *style)
{
    csr_assert(menu);
    csr_assert(style);

    // settings window
    {
        struct ui_window *win = aio_get_ui_window("applet.win.settings");
        csr_assert(win);

        igMenuItem_BoolPtr(win->title, NULL, &win->is_opened, true);
    }
}

void register_ui()
{
    // view menu
    {
        static struct ui_menu menu = {0};
        ui_menu_init(&menu, "View");

        menu.draw_cb = _draw_view_menu_cb;

        aio_add_ui_menu("applet.menu.view", &menu);
    }

    // settings window
    {
        static struct ui_window window = {0};
        ui_window_init(&window, "Settings");

        window.view.draw_cb = _draw_settings_view_cb;
        window.is_opened = true;

        aio_add_ui_window("applet.win.settings", &window);
    }
}
