////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

enum mouse_button
{
    MOUSE_BUTTON_UNKNOWN,

    MOUSE_BUTTON_1,
    MOUSE_BUTTON_2,
    MOUSE_BUTTON_3,
    MOUSE_BUTTON_4,
    MOUSE_BUTTON_5,
    MOUSE_BUTTON_6,
    MOUSE_BUTTON_7,
    MOUSE_BUTTON_8,

    MOUSE_BUTTON_LEFT = MOUSE_BUTTON_1,
    MOUSE_BUTTON_RIGHT = MOUSE_BUTTON_2,
    MOUSE_BUTTON_MIDDLE = MOUSE_BUTTON_3,

    MOUSE_BUTTON_LAST = MOUSE_BUTTON_8,
};

static string_cstr mouse_button_cstr(enum mouse_button button)
{
    switch (button)
    {
        case MOUSE_BUTTON_1:        return "MOUSE_BUTTON_1 / MOUSE_BUTTON_LEFT";
        case MOUSE_BUTTON_2:        return "MOUSE_BUTTON_2 / MOUSE_BUTTON_RIGHT";
        case MOUSE_BUTTON_3:        return "MOUSE_BUTTON_3 / MOUSE_BUTTON_MIDDLE";
        case MOUSE_BUTTON_4:        return "MOUSE_BUTTON_4";
        case MOUSE_BUTTON_5:        return "MOUSE_BUTTON_5";
        case MOUSE_BUTTON_6:        return "MOUSE_BUTTON_6";
        case MOUSE_BUTTON_7:        return "MOUSE_BUTTON_7";
        case MOUSE_BUTTON_8:        return "MOUSE_BUTTON_8";

        default:                    return "MOUSE_BUTTON_UNKNOWN";
    }
}
