////////////////////////////////////////////////////////////////////////////////////////////////////

#include <application.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static void _on_draw_applet_entry(struct applet_db_entry *entry, void *data)
{
    // FIXME arena_scratch_begin()
    struct arena_scratch scratch = {0};
    scratch.arena = kio_mem_get_frame_arena();
    scratch.position = arena_get_current_position(scratch.arena);

    ////////////////////////////////////////

    igTableNextRow(0, 0);

    // name
    igTableNextColumn();
    igText(string_fmt, string_fmt_arg(entry->name));

    // description
    igTableNextColumn();
    igText(string_fmt, string_fmt_arg(entry->description));

    // version string
    igTableNextColumn();
    igText(string_fmt, string_fmt_arg(entry->version_str));

    // applet filename (unique id)
    igTableNextColumn();
    {
        ImGuiSelectableFlags flags = ImGuiSelectableFlags_SpanAllColumns;

        if (igSelectable_Bool(string_get_cstr(scratch.arena, entry->filename), false, flags, make_ImVec2_zero()))
        {
            result_e result = applet_mgr_load_applet(entry->filename);

            // close window on successfull load
            struct ui_view *view = data;

            if (R_SUCCESS(result))
            {
                struct ui_window *window = view->parent;
                window->is_opened = false;
            }
        }
    }

    ////////////////////////////////////////

    // FIXME arena_scratch_end()
    arena_pop_to(scratch.arena, scratch.position);

error:
    return;
}

void draw_applet_db_view(struct ui_view* view, struct ui_style *style)
{
    csr_assert(view);
    csr_assert(style);

    ////////////////////////////////////////

    struct applet_db *db = applet_mgr_get_applet_db();

    if (!db)
    {
        igText("applet db not initialiazed yet");

        return;
    }

    ////////////////////////////////////////

    ImGuiTableFlags table_flags = 0;
    table_flags |= ImGuiTableFlags_Borders;
    table_flags |= ImGuiTableFlags_RowBg;
    table_flags |= ImGuiTableFlags_ScrollY;
    table_flags |= ImGuiTableFlags_SizingFixedFit;
    table_flags |= ImGuiTableFlags_Hideable;
    table_flags |= ImGuiTableFlags_Sortable;

    // resize table so the "Update Database" button is visible
    struct ImVec2 table_size = {0, -igGetFrameHeightWithSpacing()};

    if (igBeginTable("application#applet_db", 4, table_flags, table_size, 0))
    {
        ImGuiTableColumnFlags column_flags = 0;

        column_flags = ImGuiTableColumnFlags_WidthFixed;
        igTableSetupColumn("Name", column_flags, 0, 0);

        column_flags = ImGuiTableColumnFlags_WidthStretch;
        igTableSetupColumn("Description", column_flags, 0, 0);

        column_flags = ImGuiTableColumnFlags_WidthFixed;
        igTableSetupColumn("Version", column_flags, 0, 0);

        column_flags = ImGuiTableColumnFlags_WidthFixed;
        igTableSetupColumn("Filename", column_flags, 0, 0);

        igTableHeadersRow();

        applet_db_traverse(db, _on_draw_applet_entry, view);

        igEndTable();
    }

    ////////////////////////////////////////

    if (igButton("Update Database", make_ImVec2_zero())) {
        applet_mgr_update_db();
    }
}
