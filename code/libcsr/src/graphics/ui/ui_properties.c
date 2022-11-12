////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/graphics/ui/ui_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static void _draw_tooltip(const char *text)
{
    igTextDisabled("(?)");

    if (igIsItemHovered(0))
    {
        igBeginTooltip();
        igText(text);
        igEndTooltip();
    }
}

void ui_property_draw(struct ui_property *property)
{
    igTableNextRow(0, 0);

    ////////////////////////////////////////

    igTableNextColumn();

    if (property->name) {
        igText("%s", property->name);
    }

    if (property->tooltip)
    {
        igSameLine(0, 5);
        _draw_tooltip(property->tooltip);
    }

    ////////////////////////////////////////

    igTableNextColumn();
    property->draw_cb();
}

void ui_property_set_draw(struct ui_property_set *set)
{
    struct ImVec4 color_topic = {0.14f, 0.75f, 0.55f, 1.00f}; // FIXME get color from theme

    igNewLine();
    igTextColored(color_topic, ":: %s", set->name);
    igSeparator();

    if (set->property_count == 0)
    {
        igText("no properties available");

        return;
    }

    ////////////////////////////////////////

    ImGuiTableFlags table_flags = ImGuiTableFlags_NoBordersInBody;

    if (igBeginTable(set->name, 2, table_flags, make_ImVec2_zero(), 0))
    {
        ImGuiTableColumnFlags column_flags = 0;

        column_flags = ImGuiTableColumnFlags_WidthStretch;
        igTableSetupColumn("Description", column_flags, 0, 0);

        column_flags = ImGuiTableColumnFlags_WidthStretch;
        igTableSetupColumn("Value", column_flags, 0, 0);

        ////////////////////////////////////////

        for (u32 i = 0; i < set->property_count; i++)
        {
            struct ui_property *property = &set->properties[i];

            // property is hidden, place a newline instead
            if (property->is_hidden) { igNewLine(); continue; }

            // skip property if condition fails
            if (property->draw_cond_cb && !property->draw_cond_cb()) continue;

            ui_property_draw(property);
        }

        ////////////////////////////////////////

        igEndTable();
    }
}

void ui_property_set_group_draw(struct ui_property_set_group *group)
{
    ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_DefaultOpen;

    if (igCollapsingHeader_TreeNodeFlags(group->name, header_flags))
    {
        for (u32 i = 0; i < group->set_count; i++)
        {
            struct ui_property_set *set = group->sets[i];

            ui_property_set_draw(set);
        }

        igNewLine();
    }
}
