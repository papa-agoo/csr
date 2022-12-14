////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "audio.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct ksrv_audio
{
    bool is_initialized;

    struct ksrv_audio_conf *conf;
};

struct kernel_init_info;

result_e ksrv_audio_init(struct kernel_init_info *init_info);
void ksrv_audio_quit();
