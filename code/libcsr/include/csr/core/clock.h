////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct clock;

struct clock* clock_create(const char* name);
void clock_destroy(struct clock* clock);

const char* clock_get_name(struct clock* clock);

void clock_set_scale(struct clock* clock, f32 scale);
f32 clock_get_scale(struct clock* clock);

void clock_init(struct clock* clock, f64 elapsed_s);
void clock_update(struct clock* clock, f64 delta_s);

f64 clock_time_elapsed(struct clock* clock);
f64 clock_time_elapsed_delta(struct clock* clock);

void clock_get_h_m_s(struct clock* clock, u32* h, u32* m, u32* s);
