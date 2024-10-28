////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct kernel_init_info
{
    struct string name;

    void (*kernel_tick_cb)();

    struct {
        struct ksrv_core_conf *core;
        struct ksrv_video_conf *video;
        struct ksrv_audio_conf *audio;
    } conf;
};

result_e kernel_init(struct kernel_init_info *init_info);
void kernel_quit();

void kernel_run();
void kernel_stop();
