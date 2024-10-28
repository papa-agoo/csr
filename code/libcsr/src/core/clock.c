////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/clock.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct clock
{
    struct string name;

    f64 elapsed_s;
    f64 elapsed_delta_s;

    f32 time_scale;
};

struct clock* clock_create(struct string name)
{
    struct clock* clock = malloc(sizeof(struct clock));
    check_mem(clock);

    clock->name = string_is_valid(name) ? name : make_string("<no name>");
    clock->time_scale = 1.0f;

    clock_init(clock, 0.0f);

    return clock;

error:
    return NULL;
}

void clock_destroy(struct clock* clock)
{
    check_ptr(clock);

    free(clock);

error:
    return;
}

void clock_init(struct clock* clock, f64 elapsed_s)
{
    check_ptr(clock);

    clock->elapsed_s = elapsed_s;
    clock->elapsed_delta_s = 0.0f;

error:
    return;
}

void clock_tick(struct clock* clock, f64 dt_s)
{
    check_ptr(clock);

    dt_s *= clock->time_scale;

    clock->elapsed_s += dt_s;
    clock->elapsed_delta_s = dt_s;

error:
    return;
}

struct string clock_get_name(struct clock* clock)
{
    check_ptr(clock);

    return clock->name;

error:
    return make_string("unknown");
}

f32 clock_get_scale(struct clock* clock)
{
    check_ptr(clock);

    return clock->time_scale;

error:
    return 0;
}

void clock_set_scale(struct clock* clock, f32 scale)
{
    check_ptr(clock);

    clock->time_scale = scale;

error:
    return;
}

f64 clock_time_elapsed(struct clock* clock)
{
    check_ptr(clock);

    return clock->elapsed_s;

error:
    return 0;
}

f64 clock_time_elapsed_delta(struct clock* clock)
{
    check_ptr(clock);

    return clock->elapsed_delta_s;

error:
    return 0;
}

void clock_get_h_m_s(struct clock* clock, u32* h, u32* m, u32* s)
{
    check_ptr(clock);

    check_ptr(h);
    check_ptr(m);
    check_ptr(s);

    u32 time_elapsed_s = clock_time_elapsed(clock);
    *s = time_elapsed_s % 60;

    u32 time_elapsed_m = time_elapsed_s / 60.0f;
    *m = time_elapsed_m % 60;

    *h = time_elapsed_m / 60.0f;

    return;

error:

    *h = 0;
    *m = 0;
    *s = 0;

    return;
}
