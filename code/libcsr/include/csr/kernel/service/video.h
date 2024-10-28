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
        s32 msaa_count;

        bool debug_mode;
    } xgl;
};

static void ksrv_video_conf_defaults(struct ksrv_video_conf *conf)
{
    check_ptr(conf);

    conf->window.video_mode = video_mode_create_from_preset(VIDEO_MODE_PRESET_FULL_HD);
    conf->window.fullscreen = false;
    conf->window.vsync = true;

error:
    return;
}
