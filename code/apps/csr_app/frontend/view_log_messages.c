////////////////////////////////////////////////////////////////////////////////////////////////////

#include <application.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

CSR_INLINE struct vec4 _get_message_color(enum log_level_type level, struct ui_style *style)
{
    check_ptr(style);

    switch (level)
    {
        case LOG_LEVEL_INFO:
            return style->color.log_info;

        case LOG_LEVEL_NOTICE:
            return style->color.log_notice;

        case LOG_LEVEL_WARN:
            return style->color.log_warn;

        case LOG_LEVEL_ERROR:
            return style->color.log_error;

        case LOG_LEVEL_TRACE:
            return style->color.log_trace;
    }

error:
    return make_vec4_one();
}

static void _on_draw_log_message(struct log_message *message, void *style)
{
    bool show_trace_messages = application_conf_ptr()->kernel.core.log_show_trace_messages;

    if (message->level == LOG_LEVEL_TRACE && !show_trace_messages) return;

    ////////////////////////////////////////

    const struct ImVec4 color = make_ImVec4_from_vec4( _get_message_color(message->level, style) );
    {
        igTableNextRow(0, 0);

        // frame.frame_ms
        igTableNextColumn();
        igTextColored(color, "[ %08d.%04d ]", message->frame, message->frame_time_ms);

        // message
        igTableNextColumn();
        igTextColored(color, "%s", message->message);

        // module
        igTableNextColumn();
        igTextColored(color, "%s", message->module);
    }
}

static void _draw_view(struct ui_view* view, struct ui_style *style)
{
    if (igCollapsingHeader_TreeNodeFlags("Settings", 0))
    {
        struct ksrv_core_conf *conf = &application_conf_ptr()->kernel.core;

        igCheckbox("Show Tace Messages", &conf->log_show_trace_messages);
    }

    ////////////////////////////////////////

    ImGuiTableFlags table_flags = 0;
    table_flags |= ImGuiTableFlags_BordersV;
    table_flags |= ImGuiTableFlags_BordersOuterH;
    table_flags |= ImGuiTableFlags_ScrollY;
    table_flags |= ImGuiTableFlags_SizingFixedFit;

    igPushStyleVar_Vec2(ImGuiStyleVar_CellPadding, make_ImVec2(8, 2));

    if (igBeginTable("application#log_window", 3, table_flags, make_ImVec2_zero(), 0))
    {
        ImGuiTableColumnFlags column_flags = 0;

        column_flags = ImGuiTableColumnFlags_WidthFixed;
        igTableSetupColumn("Timestamp", column_flags, 0, 0);

        column_flags = ImGuiTableColumnFlags_WidthStretch;
        igTableSetupColumn("Message", column_flags, 1.0, 0);

        column_flags = ImGuiTableColumnFlags_WidthFixed;
        igTableSetupColumn("Module", column_flags, 0, 0);

        // draw log entries
        log_db_traverse(kio_log_get_db(), _on_draw_log_message, style);

        // autoscroll down the view
        if (igGetScrollY() >= igGetScrollMaxY()) {
            igSetScrollHereY(1.0f);
        }

        igEndTable();
    }

    igPopStyleVar(1);
}

////////////////////////////////////////////////////////////////////////////////

void init_log_messages_view(struct ui_view* view)
{
    view->draw_cb = _draw_view;
}
