////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/stopwatch.h>

#include "screen.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct screen
{
    struct string name;

    f32 scale_factor;
    enum screen_scale_policy scale_policy;

    bool keep_aspect_ratio;
    enum screen_resize_policy resize_policy;

    struct screen_stats stats;
    struct stopwatch stopwatch;

    struct screen_surface *surface;
};
