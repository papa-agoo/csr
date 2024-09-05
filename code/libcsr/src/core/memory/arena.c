////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/memory/arena.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct arena
{
    u64 size;
    u64 position;

    u8 *data;
};

struct arena* arena_create(u64 size)
{
    check_expr(size > 0);

    struct arena *arena = calloc(1, sizeof(struct arena));
    check_mem(arena);

    arena->size = size;
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

void* arena_push(struct arena *arena, u64 size)
{
    check_ptr(arena);
    check_expr(size > 0 && size < (arena->size - arena->position));

    void* ptr = arena->data + arena->position;
    arena->position += size;

    return ptr;

error:
    return NULL;
}

void arena_pop(struct arena *arena, u64 position)
{
    check_ptr(arena);
    check_expr(position >= 0 && position <= arena->position);

    arena->position = position;

    // FIXME memset() zero for arena_reset() ?

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
