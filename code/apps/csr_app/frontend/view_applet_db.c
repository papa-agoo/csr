////////////////////////////////////////////////////////////////////////////////////////////////////

#include <application.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static void _on_draw_applet_entry(struct applet_db_entry *entry, void *data)
{
    igTableNextRow(0, 0);

    igTableNextColumn();
    igText(entry->name);

    igTableNextColumn();
    igText(entry->description);

    igTableNextColumn();
    igText(entry->version_str);

    // applet filename is unique id
    igTableNextColumn();
    {
        ImGuiSelectableFlags flags = ImGuiSelectableFlags_SpanAllColumns;

        if (igSelectable_Bool(entry->filename, false, flags, make_ImVec2_zero()))
        {
            result_e result = applet_mgr_load_applet(entry->filename);

            // close window on successfull load
            struct ui_view *view = data;

            if (R_SUCCESS(result)) {
                view->parent->is_opened = false;
            }
        }
    }
}

static void _draw_view(struct ui_view* view, struct ui_style *style)
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

    struct ImVec2 table_size = {0, -igGetFrameHeightWithSpacing()}; // FIXME ???

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

////////////////////////////////////////////////////////////////////////////////

void init_applet_db_view(struct ui_view* view)
{
    view->draw_cb = _draw_view;
}
