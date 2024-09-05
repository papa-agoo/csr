////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base/common.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

// - how to determine initial size?
//      - scratch: (ie 1kb), user: arena_create(MegaBytes(1))
//
// - how to grow properly without realloc?
//      - virtual address space? (stable pointers)
//      - growing stack: struct arena_storage (block)
//
// - alignment handling?

#define make_arena() arena_create(KiloBytes(64))

////////////////////////////////////////////////////////////////////////////////////////////////////

struct arena;

struct arena* arena_create(u64 size);
void arena_destroy(struct arena *arena);

void* arena_push(struct arena *arena, u64 size);
void arena_pop(struct arena *arena, u64 position);

#define arena_reset(arena) arena_pop(arena, 0);

////////////////////////////////////////////////////////////////////////////////

// struct arena_scratch
// {
//     struct arena *arena;

//     u64 position;
// };

// struct arena_scratch arena_scratch_begin(struct arena *arena);
// void arena_scratch_end(struct arena_scratch scratch);

// struct arena_scratch scratch = arena_scratch_begin(my_arena);
// string_cstr scan_path = cstr_from_string(scratch.arena, make_string("foobar"));
// ... use cstr ...
// arena_scratch_end(scratch);
