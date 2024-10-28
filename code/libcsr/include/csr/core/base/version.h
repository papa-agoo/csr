////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "types.h"
#include "string.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct version
{
    s32 major;
    s32 minor;
    s32 micro;
};

#define make_version(...) \
    make_compound_literal(struct version, __VA_ARGS__)

struct string version_str(struct version v);
