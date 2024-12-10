////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/base.h>
#include <csr/core/memory.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

void _test_string_checks()
{
    struct string str = make_string("/foo.so");

    bool equals = string_equals(str, str);                              // true
    equals = string_equals(str, make_string("/foo.so"));                // true
    equals = string_equals(make_string("/foo.so"), str);                // true
    equals = string_equals(make_string("popo"), make_string("popo"));   // true
    equals = string_equals(make_string("popo"), make_string("pipi"));   // false

    bool starts_with = string_starts_with(str, make_string("/"));       // true
    starts_with = string_starts_with(str, make_string("/foo"));         // true
    starts_with = string_starts_with(str, make_string("/fooo"));        // false

    bool ends_with = string_ends_with(str, make_string(".so"));         // true
    ends_with = string_ends_with(str, make_string(".sa"));              // false
    ends_with = string_ends_with(str, make_string("o"));                // true

    bool contains = string_contains(str, make_string("foo"));           // true
    contains = string_contains(str, make_string("/"));                  // true
    contains = string_contains(str, make_string("xoo.so/"));            // false
    contains = string_contains(str, make_string("o.s"));                // true
    contains = string_contains(str, make_string("fo"));                 // true

    bool has_suffix = string_has_suffix(str, make_string("so"));        // true
    has_suffix = string_has_suffix(str, make_string(".so"));            // true
    has_suffix = string_has_suffix_cstr(str, "so");                     // true
    has_suffix = string_has_suffix_cstr(str, ".so");                    // true
    has_suffix = string_has_suffix_cstr(str, ".s");                     // false
    has_suffix = string_has_suffix_cstr(str, ".soo");                   // false
}

void _test_string_find()
{
    s32 idx = 0;
    struct string str = make_string("{FOO}/applets/model_viewer.so");

    // find
    {
        // FIXME
    }

    // find at
    {
        // FIXME
    }

    // find_str
    {
        idx = string_find_str(str, make_string(""));                    // -1 (string_is_valid() check triggers an error)
        idx = string_find_str(str, make_string("{Foo}"));               // -1
        idx = string_find_str(str, make_string("apples"));              // -1
        idx = string_find_str(str, make_string("Model_Viewer.so"));     // -1

        idx = string_find_str(str, make_string("{FOO}"));               // 0
        idx = string_find_str(str, make_string("applets"));             // 6
        idx = string_find_str(str, make_string("model_viewer.so"));     // 14

        str = make_string("{FOO}/{BAR}/{BAZ}");
        idx = string_find_str(str, make_string("{FOO}"));               // 0
        idx = string_find_str(str, make_string("{BAR}"));               // 6
        idx = string_find_str(str, make_string("{BAZ}"));               // 12
    }
}

void _test_string_trim_chop()
{
    // trim + strip
    {
        struct string str = make_string(" \n\r dirty string  \r\r\n  ");
        clog_info(">> [" string_fmt "]", string_fmt_arg(str));

        clog_info("trim() => [" string_fmt "]", string_fmt_arg(string_trim(str)));
        clog_info("rtrim() => [" string_fmt "]", string_fmt_arg(string_rtrim(str)));
        clog_info("rtrim(trim()) => [" string_fmt "]", string_fmt_arg(string_rtrim(string_trim(str))));
        clog_info("trim(rtrim()) => [" string_fmt "]", string_fmt_arg(string_trim(string_rtrim(str))));

        clog_info("strip() => [" string_fmt "]", string_fmt_arg(string_strip(str)));
    }

    // chop
    {
        struct string str = make_string("/foo/bar.png");
        clog_info(">> [" string_fmt "]", string_fmt_arg(str));

        clog_info("chop(1) => [" string_fmt "]", string_fmt_arg(string_chop(str, 1)));      // foo/bar.png
        clog_info("chop(-1) => [" string_fmt "]", string_fmt_arg(string_chop(str, -1)));    // g

        clog_info("chop(4) => [" string_fmt "]", string_fmt_arg(string_chop(str, 4)));      // /bar.png
        clog_info("chop(-4) => [" string_fmt "]", string_fmt_arg(string_chop(str, -4)));    // .png

        clog_info("rchop(1) => [" string_fmt "]", string_fmt_arg(string_rchop(str, 1)));    // /
        clog_info("rchop(-1) => [" string_fmt "]", string_fmt_arg(string_rchop(str, -1)));  // /foo/bar.pn

        clog_info("rchop(4) => [" string_fmt "]", string_fmt_arg(string_rchop(str, 4)));    // /foo
        clog_info("rchop(-4) => [" string_fmt "]", string_fmt_arg(string_rchop(str, -4)));  // /foo/bar
    }

    // find + chop
    {
        struct string str = make_string("{FOO}/applets/model_viewer.so");
        clog_info(">> [" string_fmt "] (length: %ld chars)", string_fmt_arg(str), str.length);


        clog_info("find('#')) => %d", string_find(str, '#'));       // -1
        clog_info("rfind('*')) => %d", string_rfind(str, '*'));     // -1

        clog_info("find('{')) => %d", string_find(str, '{'));       // 0
        clog_info("rfind('}')) => %d", string_rfind(str, '}'));     // 4

        clog_info("find('/')) => %d", string_find(str, '/'));       // 5
        clog_info("rfind('/')) => %d", string_rfind(str, '/'));     // 13

        clog_info("find('.')) => %d", string_find(str, '.'));       // 26
        clog_info("rfind('.')) => %d", string_rfind(str, '.'));     // 26


        clog_info("chop(find('.')) => [" string_fmt "]", string_fmt_arg(string_chop(str, string_find(str, '.'))));              // .so
        clog_info("chop(find('/') + 1) => [" string_fmt "]", string_fmt_arg(string_chop(str, string_find(str, '/') + 1)));      // applets/model_viewer.so

        clog_info("chop(rfind('.')) => [" string_fmt "]", string_fmt_arg(string_chop(str, string_rfind(str, '.'))));            // .so
        clog_info("chop(rfind('/') + 1) => [" string_fmt "]", string_fmt_arg(string_chop(str, string_rfind(str, '/') + 1)));    // model_viewer.so

        clog_info("rchop(find('/')) => [" string_fmt "]", string_fmt_arg(string_rchop(str, string_find(str, '/'))));            // {FOO}
        clog_info("rchop(find('}') + 1) => [" string_fmt "]", string_fmt_arg(string_rchop(str, string_find(str, '}') + 1)));    // {FOO}
    }

    // find + cut + cut_dir_file
    {
        struct string path = make_string("{FOO}/applets/model_viewer.so");
        clog_info(">> [" string_fmt "] (length: %ld chars)", string_fmt_arg(path), path.length);

        struct string_pair parts = {0};

        // find + cut
        parts = string_cut(path, string_find(path, '/'));
        clog_info("cut(find('/')) => ["string_fmt"], ["string_fmt"]", string_fmt_arg(parts.left), string_fmt_arg(parts.right)); // [{FOO}], [applets/model_viewer.so]

        parts = string_cut(path, string_rfind(path, '/'));
        clog_info("cut(rfind('/')) => ["string_fmt"], ["string_fmt"]", string_fmt_arg(parts.left), string_fmt_arg(parts.right));// [{FOO}/applets], [model_viewer.so]

        parts = string_cut(path, string_rfind(path, '.'));
        clog_info("cut(rfind('.')) => ["string_fmt"], ["string_fmt"]", string_fmt_arg(parts.left), string_fmt_arg(parts.right));// [{FOO}/applets/model_viewer], [so]

        // cut_dir_file
        parts = string_cut_dir_file(path);
        clog_info("cut_dir_file() => dir : [%S], file : [%S]", &parts.directory, &parts.filename);
    }
}

void _test_string_substr()
{
    // path tests
    {
        struct string str = make_string("/foo/bar.so");
        clog_info(">> [" string_fmt "]", string_fmt_arg(str));

        struct string str_out = {0};

        str_out = string_substr(str, 0, 0);
        clog_info("substr(0, 0) => "string_fmt, string_fmt_arg(str_out));      // /foo/bar.so

        str_out = string_substr(str, 1, 0);
        clog_info("substr(1, 0) => "string_fmt, string_fmt_arg(str_out));       // foo/bar.so

        str_out = string_substr(str, 0, -1);
        clog_info("substr(0, -1) => "string_fmt, string_fmt_arg(str_out));      // /foo/bar.s

        str_out = string_substr(str, 1, -1);
        clog_info("substr(1, -1) => "string_fmt, string_fmt_arg(str_out));      // foo/bar.s

        str_out = string_substr(str, 0, 5);
        clog_info("substr(0, 5) => "string_fmt, string_fmt_arg(str_out));       // /foo/

        str_out = string_substr(str, 1, -7);
        clog_info("substr(1, -7) => "string_fmt, string_fmt_arg(str_out));      // foo

        str_out = string_substr(str, 5, -3);
        clog_info("substr(5, -3) => "string_fmt, string_fmt_arg(str_out));      // bar

        str_out = string_substr(str, 5, 0);
        clog_info("substr(5, 0) => "string_fmt, string_fmt_arg(str_out));       // bar.so

        str_out = string_substr(str, -6, 3);
        clog_info("substr(-6, 3) => "string_fmt, string_fmt_arg(str_out));      // bar

        str_out = string_substr(str, -3, 0);
        clog_info("substr(-3, 0) => "string_fmt, string_fmt_arg(str_out));      // .so

        // out of range tests. all of them will trigger an error (returned substr equals to input str)
        str_out = string_substr(str, 99, 0);
        clog_info("substr(99, 0) => "string_fmt, string_fmt_arg(str_out));      // /foo/bar.so

        str_out = string_substr(str, -99, 0);
        clog_info("substr(-99, 0) => "string_fmt, string_fmt_arg(str_out));     // /foo/bar.so

        str_out = string_substr(str, 0, 99);
        clog_info("substr(0, 99) => "string_fmt, string_fmt_arg(str_out));      // /foo/bar.so

        str_out = string_substr(str, 0, -99);
        clog_info("substr(0, -99) => "string_fmt, string_fmt_arg(str_out));     // /foo/bar.so
    }

    // perl tests
    {
        struct string str = make_string("The black cat climbed the green tree");

        clog_info("color: [" string_fmt "]", string_fmt_arg(string_substr(str, 4, 5)));
        clog_info("middle: [" string_fmt "]", string_fmt_arg(string_substr(str, 4, -11)));
        clog_info("end: [" string_fmt "]", string_fmt_arg(string_substr(str, 14, 0)));
        clog_info("tail: [" string_fmt "]", string_fmt_arg(string_substr(str, -4, 0)));
        clog_info("z: [" string_fmt "]", string_fmt_arg(string_substr(str, -4, 2)));
    }
}

void _test_mutable_api()
{
    struct arena *arena = make_arena("strings");
    check_ptr(arena);

    // string to cstr
    {
        string_cstr cstr = "";

        cstr = string_get_cstr(arena, make_string(""));                         // string_is_valid() triggers an error
        clog_info("get_cstr: %s", cstr);                                        // <empty>

        cstr = string_get_cstr(arena, make_string("hallo popo"));
        clog_info("get_cstr: %s", cstr);                                        // hallo popo
    }

    // format
    {
        struct string str = {0};

        str = string_create_fmt(arena, "%s", "");
        clog_info("create_fmt: "string_fmt, string_fmt_arg(str));               // <empty>

        str = string_create_fmt(arena, "%s", "foo");
        clog_info("create_fmt: "string_fmt, string_fmt_arg(str));               // foo

        str = string_create_fmt(arena, "%s/%s.%s", "{FOO}", "bar/baz", "so");
        clog_info("create_fmt: "string_fmt, string_fmt_arg(str));               // {FOO}/bar/baz.so
    }

    // replace
    {
        struct string var_work_dir = make_string("/opt/csr");
        struct string var_applet_name = make_string("model_viewer.so");

        struct string str = make_string("{WORK_DIR}/applets/{APPLET_NAME}");
        clog_info("str: "string_fmt, string_fmt_arg(str));                                  // {WORK_DIR}/applets/{APPLET_NAME}

        str = string_replace(arena, str, make_string("{WORK_DIR}"), var_work_dir);
        clog_info("str: "string_fmt, string_fmt_arg(str));                                  // /opt/csr/applets/{APPLET_NAME}

        str = string_replace(arena, str, make_string("{APPLET_NAME}"), var_applet_name);
        clog_info("str: "string_fmt, string_fmt_arg(str));                                  // /opt/csr/applets/model_viewer.so

        ////////////////////////////////////////

        struct string var_foo = make_string("a");
        struct string var_bar = make_string("b");

        str = make_string("{FOO}/{BAR}/{FOO}/{BAR}");
        clog_info("str: "string_fmt, string_fmt_arg(str));                                  // {FOO}/{BAR}/{FOO}/{BAR}

        str = string_replace(arena, str, make_string("{FOO}"), var_foo);
        clog_info("str: "string_fmt, string_fmt_arg(str));                                  // a/{BAR}/a/{BAR}

        str = string_replace(arena, str, make_string("{BAR}"), var_bar);
        clog_info("str: "string_fmt, string_fmt_arg(str));                                  // a/b/a/b

        str = string_replace(arena, str, make_string("{BAZ}"), var_bar);
        clog_info("str: "string_fmt, string_fmt_arg(str));                                  // a/b/a/b (nothing replaced)
    }

    // copy
    {
        struct string copy = string_copy(arena, make_string(""));   // string_is_valid() triggers an error
        bool equals = string_equals(copy, make_string(""));         // false, string_is_valid() triggers an error

        copy = string_copy(arena, make_string("foo"));
        equals = string_equals(copy, make_string("foo"));           // true

        copy = string_copy(arena, make_string("Foo"));
        equals = string_equals(copy, make_string("foo"));           // false

    }

    arena_destroy(arena);

error:
    return;
}

void _test_string_lists()
{
    // struct string path = make_string("/path/to/foo/bar.png")
}

result_e main()
{
    _test_string_checks();
    _test_string_find();
    _test_string_substr();
    _test_string_trim_chop();
    _test_mutable_api();
    // _test_string_lists();

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}
