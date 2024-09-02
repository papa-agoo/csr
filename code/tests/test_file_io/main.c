////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/file_io.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

result_e main()
{
    struct string path_abs = make_string("/opt/csr");
    struct string path_rel = make_string("resources/fonts/Consolas.ttf");

    struct string path_meh = make_string("//foo/./bar/baz/");
    struct string path_inv = make_string("invalid\0path");

    ////////////////////////////////////////////////////////////

    struct string str = fio_fs_normalize_path(path_meh);                // (FIXME) /foo/bar/baz

    bool result = false;

    result = fio_fs_is_valid_path(path_abs);                            // true
    result = fio_fs_is_valid_path(path_inv);                            // false

    result = fio_fs_is_absolute_path(path_abs);                         // true
    result = fio_fs_is_relative_path(path_abs);                         // false

    result = fio_fs_is_relative_path(path_rel);                         // true
    result = fio_fs_is_absolute_path(path_rel);                         // false

    ////////////////////////////////////////////////////////////

    str = fio_fs_get_current_path();                                    // <your shell cwd>

    str = fio_fs_get_parent_path(path_abs);                             // (FIXME) /opt
    str = fio_fs_get_parent_path(path_rel);                             // (FIXME) resources/fonts

    str = fio_fs_get_absolute_path(path_rel);                           // /opt/csr/resources/fonts/Consolas.ttf
    str = fio_fs_get_relative_path(str);                                // (FIXME) resources/fonts/Consolas.ttf (relative to <your shell cwd>)

    str = fio_fs_get_file_name(path_rel);                               // Consolas.ttf
    str = fio_fs_get_file_name(make_string("foo"));                     // foo
    str = fio_fs_get_file_name(make_string("/bar"));                    // bar
    str = fio_fs_get_file_name(make_string("/foo/.bar"));               // .bar
    str = fio_fs_get_file_name(make_string(".foo.bar.png"));            // .foo.bar.png

    str = fio_fs_get_file_extension(path_rel);                          // ttf
    str = fio_fs_get_file_extension(make_string("/foo"));               // no extension found (str is "")
    str = fio_fs_get_file_extension(make_string("/foo/.bar"));          // no extension found (str is "")
    str = fio_fs_get_file_extension(make_string("/foo/.bar.png"));      // png
    str = fio_fs_get_file_extension(make_string(".png"));               // no extension found (str is "")
    str = fio_fs_get_file_extension(make_string(".foo.bar.png"));       // png

    ////////////////////////////////////////////////////////////

    result = fio_fs_exists(path_rel);                                   // (FIXME) true
    result = fio_fs_exists(string_rchop(path_rel, 1));                  // (FIXME) false

    result = fio_fs_is_file(path_rel);                                  // (FIXME) true
    result = fio_fs_is_file(string_rchop(path_rel, 1));                 // (FIXME) false

    result = fio_fs_is_directory(path_abs);                             // (FIXME) true
    result = fio_fs_is_directory(string_rchop(path_abs, 1));            // (FIXME) false

    result = fio_fs_file_has_extension(path_rel, make_string("ttf"));   // true
    result = fio_fs_file_has_extension(path_rel, make_string("png"));   // false

    ////////////////////////////////////////////////////////////

error:
    return RC_SUCCESS;
}
