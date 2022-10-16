////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "types.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct version
{
    u32 major;
    u32 minor;
    u32 micro;
};

#define make_version(...) \
    make_compound_literal(struct version, __VA_ARGS__)

CSR_INLINE const char* version_str(struct version v)
{
    // FIXME need custom string impl.
    const u32 buf_size = 128;
    char str_buf[buf_size];

    snprintf(str_buf, buf_size, "%d.%d.%d", v.major, v.minor, v.micro);

    return strdup(str_buf);
}
