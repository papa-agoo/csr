////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct guid { u64 id; } guid;

guid guid_create(u32 value);

CSR_INLINE u32 guid_get_magic(guid _guid) {
    return _guid.id >> 32;
}

CSR_INLINE u32 guid_get_value(guid _guid) {
    return _guid.id & 0xFFFFFFFF;
}

CSR_INLINE bool guid_valid(guid _guid) {
    return _guid.id > 0;
}

#define make_guid(v) (struct guid) { .id = v }
#define check_guid(g) check(guid_valid(g), "GUID invalid : %ld", (g).id)
