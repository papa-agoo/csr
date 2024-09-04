////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/file_io.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define _print_result_a(fn_name, in) \
    clog_info(fn_name"() => "string_fmt, string_fmt_arg(str_out))

#define _print_result_ab(fn_name, in, out) \
    clog_info(fn_name"("string_fmt") => "string_fmt, string_fmt_arg(str_in), string_fmt_arg(str_out))

result_e main()
{
    struct arena *arena = NULL;

    struct string path_abs = make_string("/opt/csr");
    struct string path_rel = make_string("resources/fonts/Consolas.ttf");

    struct string path_meh = make_string("//foo/./bar/baz/");
    struct string path_inv = make_string("invalid\0path");

    struct string str_in = {0};
    struct string str_out = {0};

    bool result = false;

    // test file save / load api
    {
        // these files must be created accordingly
        struct string file_none = make_string("/tmp/popo.txt");

        struct string file_regular = make_string("/tmp/foo.txt");
        struct string file_regular_zero = make_string("/tmp/foo.empty.txt");

        struct string file_other = make_string("/tmp/foo.symlink.txt");
        struct string file_directory = make_string("/tmp/foo.directory.txt");

        result_e r = RC_FAILURE;

        struct fio_buffer buffer = {0};

        // r = fio_load_file(file_none, &buffer);                              // RC_FAILURE, file not found
        // r = fio_load_file(file_regular, &buffer);                           // RC_SUCCESS
        // r = fio_load_file(file_regular_zero, &buffer);                      // RC_SUCCESS
        // r = fio_load_file(file_other, &buffer);                             // RC_FAILURE, not a regular file (symlink)
        // r = fio_load_file(file_directory, &buffer);                         // RC_FAILURE, not a regular file (directory)

        // r = fio_save_file(file_none, &buffer);                              // RC_SUCCESS, create new file
        // r = fio_save_file(file_regular, &buffer);                           // RC_SUCCESS, overwrite existing file
        // r = fio_save_file(file_regular_zero, &buffer);                      // RC_SUCCESS, overwrite existing file
        // r = fio_save_file(file_other, &buffer);                             // RC_FAILURE, not a regular file (symlink)
        // r = fio_save_file(file_directory, &buffer);                         // RC_FAILURE, not a regular file (directory)
    }

    // test file stream api
    {
        // ...
    }

    // test path api
    {
        // str_in = path_meh;
        // str_out = fio_fs_normalize_path(str_in, arena);
        // _print_result_ab("fio_fs_normalize_path", str_in, str_out);         // /foo/bar/baz

        result = fio_fs_is_valid_path(path_abs);                            // true
        result = fio_fs_is_valid_path(path_inv);                            // false

        result = fio_fs_is_absolute_path(path_abs);                         // true
        result = fio_fs_is_relative_path(path_abs);                         // false

        result = fio_fs_is_relative_path(path_rel);                         // true
        result = fio_fs_is_absolute_path(path_rel);                         // false

        ////////////////////////////////////////////////////////////

        str_out = fio_fs_get_current_path(arena);
        _print_result_a("fio_fs_get_current_path", str_out);                // <your shell cwd>
        
        str_in = path_abs;
        str_out = fio_fs_get_parent_path(str_in);
        _print_result_ab("fio_fs_get_parent_path", str_in, str_out);        // /opt

        str_in = path_rel;
        str_out = fio_fs_get_parent_path(str_in);
        _print_result_ab("fio_fs_get_parent_path", str_in, str_out);        // resources/fonts

        str_in = path_rel;
        str_out = fio_fs_get_absolute_path(str_in, arena);
        _print_result_ab("fio_fs_get_absolute_path", str_in, str_out);      // /opt/csr/resources/fonts/Consolas.ttf

        // str_in = str_out;
        // str_out = fio_fs_get_relative_path(str_in);
        // _print_result_ab("fio_fs_get_relative_path", str_in, str_out);      // resources/fonts/Consolas.ttf (relative to <your shell cwd>)

        ////////////////////////////////////////////////////////////

        str_in = path_rel;
        str_out = fio_fs_get_file_name(str_in);
        _print_result_ab("fio_fs_get_file_name", str_in, str_out);          // Consolas.ttf

        str_in = make_string("foo");
        str_out = fio_fs_get_file_name(str_in);
        _print_result_ab("fio_fs_get_file_name", str_in, str_out);          // foo

        str_in = make_string("/bar");
        str_out = fio_fs_get_file_name(str_in);
        _print_result_ab("fio_fs_get_file_name", str_in, str_out);          // bar

        str_in = make_string("/foo/.bar");
        str_out = fio_fs_get_file_name(str_in);
        _print_result_ab("fio_fs_get_file_name", str_in, str_out);          // .bar

        str_in = make_string(".foo.bar.png");
        str_out = fio_fs_get_file_name(str_in);
        _print_result_ab("fio_fs_get_file_name", str_in, str_out);          // .foo.bar.png

        ////////////////////////////////////////////////////////////

        str_in = path_rel;
        str_out = fio_fs_get_file_extension(str_in);
        _print_result_ab("fio_fs_get_file_extension", str_in, str_out);     // ttf

        str_in = make_string("/foo");
        str_out = fio_fs_get_file_extension(str_in);
        _print_result_ab("fio_fs_get_file_extension", str_in, str_out);     // no extension found (str_out is "")

        str_in = make_string("/foo/.bar");
        str_out = fio_fs_get_file_extension(str_in);
        _print_result_ab("fio_fs_get_file_extension", str_in, str_out);     // no extension found (str_out is "")

        str_in = make_string("/foo/.bar.png");
        str_out = fio_fs_get_file_extension(str_in);
        _print_result_ab("fio_fs_get_file_extension", str_in, str_out);     // png

        str_in = make_string(".png");
        str_out = fio_fs_get_file_extension(str_in);
        _print_result_ab("fio_fs_get_file_extension", str_in, str_out);     // no extension found (str_out is "")

        str_in = make_string(".foo.bar.png");
        str_out = fio_fs_get_file_extension(str_in);
        _print_result_ab("fio_fs_get_file_extension", str_in, str_out);     // png
    }

    // test filesystem api
    {
        result = fio_fs_exists(path_rel);                                   // true
        result = fio_fs_exists(string_chop(path_rel, 1));                   // false

        result = fio_fs_is_file(path_rel);                                  // true
        result = fio_fs_is_file(string_chop(path_rel, 1));                  // false

        result = fio_fs_is_directory(path_abs);                             // true
        result = fio_fs_is_directory(string_chop(path_abs, 1));             // false

        result = fio_fs_file_has_extension(path_rel, make_string("ttf"));   // true
        result = fio_fs_file_has_extension(path_rel, make_string("png"));   // false

        ////////////////////////////////////////////////////////////

        struct string path_base = make_string("/tmp/foo");
        struct string path_full = make_string("/tmp/foo/bar/baz.bam");

        // path_base must be deleted manually first :)
        check(!fio_fs_exists(path_base),
            "cannot run r/w tests, please delete "string_fmt" manually", string_fmt_arg(path_base));

        result_e r = RC_FAILURE;

        r = fio_fs_create_directory(path_base);                             // RC_SUCCESS
        r = fio_fs_remove(path_base);                                       // RC_SUCCESS

        r = fio_fs_create_directory(path_full);                             // RC_SUCCESS
        r = fio_fs_create_directory(path_base);                             // RC_FAILURE: path already exists

        r = fio_fs_remove(path_base);                                       // RC_SUCCESS
        r = fio_fs_remove(path_full);                                       // RC_FAILURE: path not found

        ////////////////////////////////////////////////////////////

        // directory tree listing / traversing
        // ...
    }

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}
