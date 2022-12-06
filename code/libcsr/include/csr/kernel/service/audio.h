////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct ksrv_audio_conf
{
    s32 volume;
};

static void ksrv_audio_conf_defaults(struct ksrv_audio_conf *conf)
{
    check_ptr(conf);

    conf->volume = 100;

error:
    return;
}
