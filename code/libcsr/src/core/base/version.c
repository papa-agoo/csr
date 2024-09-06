////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/base/version.h>
#include <csr/core/memory/arena_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct string version_str(struct version v)
{
    struct arena* arena = _arena_priv_ptr();
    check_ptr(arena);

    return string_create_fmt(arena, "%d.%d.%d", v.major, v.minor, v.micro);

error:
    return make_string("?.?.?");
}
