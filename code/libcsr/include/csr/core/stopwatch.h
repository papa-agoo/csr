////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef f64 (*rtc_time_elapsed_cb_t)();

struct stopwatch
{
    u32 num_splits;

    f64 start;
    f64 stop;

    f64 runtime_s;
    f32 reset_after_s;

    rtc_time_elapsed_cb_t rtc_time_elapsed_cb;
};

void stopwatch_init(struct stopwatch *watch, rtc_time_elapsed_cb_t rtc_cb, f32 reset_after_s);
void stopwatch_reset(struct stopwatch *watch);

void stopwatch_start(struct stopwatch *watch);
void stopwatch_stop(struct stopwatch *watch);

f64 stopwatch_time_elapsed(struct stopwatch *watch);
f64 stopwatch_time_elapsed_avg(struct stopwatch *watch);
f64 stopwatch_time_elapsed_total(struct stopwatch *watch);

f32 stopwatch_splits_per_second(struct stopwatch *watch);
