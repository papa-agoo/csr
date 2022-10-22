////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "video.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct ksrv_video
{
    bool is_initialized;

    xgl_swapchain swapchain;

    struct ksrv_video_conf *conf;
};

struct ksrv_video_init_info
{
    struct ksrv_video_conf *conf;
};

result_e ksrv_video_init(struct ksrv_video_init_info *init_info);
void ksrv_video_quit();

void ksrv_video_prepare_framebuffer();
void ksrv_video_present_framebuffer();

result_e ksrv_video_create_swapchain(u32 width, u32 height, xgl_swapchain *out);
