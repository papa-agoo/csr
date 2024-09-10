////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base/common.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct arena;

#define make_arena() arena_create(KiloBytes(64))

////////////////////////////////////////////////////////////////////////////////

struct arena* arena_create(u64 size);
void arena_destroy(struct arena *arena);

void* arena_get_current_ptr(struct arena *arena);
u64 arena_get_current_position(struct arena *arena);

// advance the current position by size bytes and return the current address ptr
void* arena_push(struct arena *arena, u64 size);

// rewind current position by size bytes
void arena_pop(struct arena *arena, u64 size);

// rewind current position by size bytes and zero init the free storage
void arena_pop_zero(struct arena *arena, u64 size);

// rewind current position to the new position
void arena_pop_to(struct arena *arena, u64 position);

// rewind current position to the new position and zero init the free storage
void arena_pop_to_zero(struct arena *arena, u64 position);

#define arena_reset(arena) arena_pop_to(arena, 0);
#define arena_reset_zero(arena) arena_pop_to_zero(arena, 0);

////////////////////////////////////////////////////////////////////////////////

struct arena_scratch
{
    struct arena *arena;

    u64 position;
};

// struct arena_scratch arena_scratch_begin(struct arena *arena);
// void arena_scratch_end(struct arena_scratch scratch);

// struct arena_scratch scratch = arena_scratch_begin(my_arena);
// string_cstr scan_path = string_get_cstr(scratch.arena, make_string("foobar"));
// ... use cstr ...
// arena_scratch_end(scratch);
