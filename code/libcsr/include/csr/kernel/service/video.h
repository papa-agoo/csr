////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../video_mode.h"
#include <csr/graphics/xgl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct ksrv_video_conf
{
    struct {
        struct video_mode video_mode;

        bool fullscreen;
        bool vsync;
    } window;

    struct {
        u32 msaa_count;

        bool debug_mode;
    } xgl;
};

void ksrv_video_conf_defaults(struct ksrv_video_conf *conf);
