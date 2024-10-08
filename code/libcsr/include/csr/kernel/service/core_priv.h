////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core.h"
#include "../mainloop.h"

#include <csr/platform.h>
#include <csr/core/clock.h>
#include <csr/core/memory.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define KSRV_CORE_MAX_LOG_MESSAGES 256

struct ksrv_core
{
    bool is_initialized;

    struct log_db* log_db;
    struct env_vars* env_vars;

    struct clock* clock;
    struct frame_stats frame_stats;

    struct mainloop* mainloop;
    void (*kernel_tick_cb)();

    // allocators
    struct {
        struct arena* arena_main;
        struct arena* arena_frame;
    } allocator;

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

struct log_db* ksrv_get_log_db();
struct env_vars* ksrv_get_env_vars();
