////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef DEG2RAD
#define DEG2RAD(a) ((f32)a * (M_PI / 180.0f))
#endif

#ifndef RAD2DEG
#define RAD2DEG(r) ((f32)r * (180.0f / M_PI))
#endif

////////////////////////////////////////////////////////////////////////////////

CSR_INLINE f32 min(f32 a, f32 b)
{
    return (a < b) ? a : b;
}

CSR_INLINE f32 max(f32 a, f32 b)
{
    return (a > b) ? a : b;
}

CSR_INLINE f32 normalize(f32 value, f32 value_min, f32 value_max)
{
    return (value - value_min) / (value_max - value_min);
}

CSR_INLINE f32 lerp(f32 value, f32 value_min, f32 value_max)
{
    return (value_max - value_min) * value + value_min;
}

// FIXME slerp()

CSR_INLINE f32 clamp(f32 value, f32 value_min, f32 value_max)
{
    return min(max(value, value_min), value_max);
}
