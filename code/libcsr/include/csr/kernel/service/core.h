////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../log_db.h"
#include "../env_vars.h"

#include <csr/platform/platform.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct frame_stats
{
    u32 num_frames;

    f64 runtime_s;
    u64 runtime_ms;

    f32 avg_fps;
    f32 avg_frametime_ms;
};

struct ksrv_core_conf
{
    bool log_show_trace_messages;
};

static void ksrv_core_conf_defaults(struct ksrv_core_conf *conf)
{
    check_ptr(conf);

    conf->log_show_trace_messages = false;

error:
    return;
}
