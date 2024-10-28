////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/base/assert.h>
#include <csr/core/memory/arena.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define _arena_zero_mem(arena) \
    memset(arena->data + arena->position, 0, arena->size_free);

struct arena
{
    struct string name;

    u64 position;

    u64 size;
    u64 size_free;

    u8 *data;
};

struct arena* arena_create(struct string name, u64 size)
{
    check_expr(size > 0);

    struct arena *arena = calloc(1, sizeof(struct arena));
    check_mem(arena);

    arena->name = (string_is_valid(name)) ? name : make_string("<no name>");
    arena->size = size;
    arena->size_free = size;
    arena->position = 0;

    arena->data = calloc(1, size);
    check_mem(arena->data);

    return arena;

error:
    if (arena) free(arena);

    return NULL;
}

void arena_destroy(struct arena *arena)
{
    check_ptr(arena);

    if (arena->data) free(arena->data);

    free(arena);

error:
    return;
}

void* arena_get_current_ptr(struct arena *arena)
{
    check_ptr(arena);
    check_ptr(arena->data);

    return arena->data + arena->position;

error:
    return NULL;
}

u64 arena_get_current_position(struct arena *arena)
{
    check_ptr(arena);

    return arena->position;

error:
    return 0;
}

void* arena_push(struct arena *arena, u64 size)
{
    check_ptr(arena);
    csr_assert(size > 0 && size < arena->size_free);

    void* ptr = arena->data + arena->position;
    arena->position += size;
    arena->size_free -= size;

    return ptr;

error:
    return NULL;
}

void arena_pop(struct arena *arena, u64 size)
{
    check_ptr(arena);
    check_expr(size >= 0 && size <= arena->position);

    arena->position -= size;
    arena->size_free += size;

error:
    return;
}

void arena_pop_zero(struct arena *arena, u64 size)
{
    arena_pop(arena, size);

    _arena_zero_mem(arena);

error:
    return;
}

void arena_pop_to(struct arena *arena, u64 position)
{
    check_ptr(arena);
    check_expr(position >= 0 && position <= arena->position);

    arena->position = position;
    arena->size_free = arena->size - arena->position;

error:
    return;
}

void arena_pop_to_zero(struct arena *arena, u64 position)
{
    arena_pop_to(arena, position);

    _arena_zero_mem(arena);

error:
    return;
}

////////////////////////////////////////////////////////////////////////////////

// struct arena_scratch arena_scratch_begin(struct arena *arena)
// {
//     struct arena_scratch scratch = {0};
//     scratch.arena = arena;
//     scratch.position = arena->position;

//     return scratch;
// }

// void arena_scratch_end(struct arena_scratch scratch)
// {
//     arena_pop(scratch.arena, scratch.position);
// }
