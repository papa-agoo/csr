////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/stopwatch.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

void stopwatch_init(struct stopwatch *watch, rtc_time_elapsed_cb_t rtc_cb, f32 reset_after_s)
{
    check_ptr(watch);
    check_ptr(rtc_cb);

    watch->reset_after_s = reset_after_s;
    watch->rtc_time_elapsed_cb = rtc_cb;

    stopwatch_reset(watch);

error:
    return;
}

void stopwatch_reset(struct stopwatch *watch)
{
    check_ptr(watch);

    watch->num_splits = 0;

    watch->start = 0;
    watch->stop = 0;

    watch->runtime_s = 0;

error:
    return;
}

void stopwatch_start(struct stopwatch *watch)
{
    check_ptr(watch);

    if (watch->reset_after_s > 0 && watch->runtime_s >= watch->reset_after_s) {
        stopwatch_reset(watch);
    }

    watch->start = watch->rtc_time_elapsed_cb();

error:
    return;
}

void stopwatch_stop(struct stopwatch *watch)
{
    check_ptr(watch);


    watch->num_splits++;

    watch->stop = watch->rtc_time_elapsed_cb();

    watch->runtime_s += stopwatch_time_elapsed(watch);

error:
    return;
}

f64 stopwatch_time_elapsed(struct stopwatch *watch)
{
    check_ptr(watch);

    return watch->stop - watch->start;

error:
    return 0;
}

f64 stopwatch_time_elapsed_avg(struct stopwatch *watch)
{
    check_ptr(watch);

    check_quiet(watch->num_splits > 0);

    return watch->runtime_s / watch->num_splits;

error:
    return 0;
}

f64 stopwatch_time_elapsed_total(struct stopwatch *watch)
{
    check_ptr(watch);

    return watch->runtime_s;

error:
    return 0;
}

f32 stopwatch_splits_per_second(struct stopwatch *watch)
{
    check_ptr(watch);

    check_quiet(watch->runtime_s > 0);

    return watch->num_splits / watch->runtime_s;

error:
    return 0;
}
