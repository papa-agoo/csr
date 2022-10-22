////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core.h"
#include "../mainloop.h"

#include <csr/platform.h>
#include <csr/core/clock.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct ksrv_core
{
    bool is_initialized;

    struct log_db* log_db;

    struct clock* clock;
    struct frame_stats frame_stats;

    struct mainloop* mainloop;
    void (*kernel_tick_cb)();

    struct ksrv_core_conf *conf;
};

struct ksrv_core_init_info
{
    struct ksrv_core_conf *conf;

    struct platform_init_info *platform_info;

    void (*kernel_tick_cb)();
};

result_e ksrv_core_init(struct ksrv_core_init_info *init_info);
void ksrv_core_quit();
