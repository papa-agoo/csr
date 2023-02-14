////////////////////////////////////////////////////////////////////////////////////////////////////

#include <application.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

void _draw_kernel_settings_page(struct ui_view* view, struct ui_style *style);
void _draw_frontend_settings_page(struct ui_view* view, struct ui_style *style);

static struct ui_property_page g_property_pages[] = {
    { "Kernel", _draw_kernel_settings_page },
    { "Frontend", _draw_frontend_settings_page },
};

#define _get_property_page(i) (&g_property_pages[i])

////////////////////////////////////////////////////////////////////////////////////////////////////

void draw_settings_view(struct ui_view* view, struct ui_style *style)
{
    csr_assert(view);
    csr_assert(style);

    static u32 current_page = 0;

    ////////////////////////////////////////

    f32 list_width = 140 * ui_get_content_scale_factor();

    if (igBeginListBox("##application.settings_page_list", make_ImVec2(list_width, -1)))
    {
        u32 page_count = COUNT_OF(g_property_pages);

        for (u32 i = 0; i < page_count; i++)
        {
            struct ui_property_page *page = _get_property_page(i);

            if (igSelectable_Bool(page->title, current_page == i, 0, make_ImVec2_zero())) {
                current_page = i;
            }
        }

        igEndListBox();
    }

    ////////////////////////////////////////

    igSameLine(0, 5);

    if (igBeginChild_Str("##application.settings_page", make_ImVec2(-1, -1), true, 0))
    {
        struct ui_property_page *page = _get_property_page(current_page);

        if (page)
        {
            struct vec4 color = style->color.page_title;

            // page title
            igNewLine();
            igTextColored(make_ImVec4_from_vec4(color), ">>> %s", page->title);
            igNewLine();

            page->draw_cb(view, style);
        }

        igEndChild();
    }

error:
    return;
}
