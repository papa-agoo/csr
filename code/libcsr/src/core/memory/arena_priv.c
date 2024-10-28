////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/memory/arena_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct arena* g_arena_priv = NULL;

struct arena* _arena_priv_ptr()
{
    return g_arena_priv;
}

__attribute__((constructor)) static void _construct_arena()
{
    g_arena_priv = make_arena("_priv");
}

__attribute__((destructor)) static void _destruct_arena()
{
    arena_destroy(g_arena_priv);
}
