////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/base.h>

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
}

void _test_string_trim_chop()
{
    // trim
    {
        struct string str = make_string(" whitespace string   ");
        clog_info(">> [" string_fmt "]", string_fmt_arg(str));

        clog_info("trim() => [" string_fmt "]", string_fmt_arg(string_trim(str)));
        clog_info("rtrim() => [" string_fmt "]", string_fmt_arg(string_rtrim(str)));
        clog_info("rtrim(trim()) => [" string_fmt "]", string_fmt_arg(string_rtrim(string_trim(str))));
        clog_info("trim(rtrim()) => [" string_fmt "]", string_fmt_arg(string_trim(string_rtrim(str))));
    }

    // chop
    {
        struct string str = make_string("/foo/bar.png");
        clog_info(">> [" string_fmt "]", string_fmt_arg(str));

        clog_info("chop(1) => [" string_fmt "]", string_fmt_arg(string_chop(str, 1)));      // foo/bar.png
        clog_info("chop(-1) => [" string_fmt "]", string_fmt_arg(string_chop(str, -1)));      // g

        clog_info("chop(4) => [" string_fmt "]", string_fmt_arg(string_chop(str, 4)));      // /bar.png
        clog_info("chop(-4) => [" string_fmt "]", string_fmt_arg(string_chop(str, -4)));      // .png

        clog_info("rchop(1) => [" string_fmt "]", string_fmt_arg(string_rchop(str, 1)));    // /
        clog_info("rchop(-1) => [" string_fmt "]", string_fmt_arg(string_rchop(str, -1)));    // /foo/bar.pn

        clog_info("rchop(4) => [" string_fmt "]", string_fmt_arg(string_rchop(str, 4)));    // /foo
        clog_info("rchop(-4) => [" string_fmt "]", string_fmt_arg(string_rchop(str, -4)));    // /foo/bar
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

    // find + cut
    {
        struct string path = make_string("{FOO}/applets/model_viewer.so");
        clog_info(">> [" string_fmt "] (length: %ld chars)", string_fmt_arg(path), path.length);

        struct string_pair parts = {0};

        parts = string_cut(path, string_find(path, '/'));
        clog_info("cut(find('/')) => ["string_fmt"], ["string_fmt"]", string_fmt_arg(parts.left), string_fmt_arg(parts.right)); // [{FOO}], [applets/model_viewer.so]

        parts = string_cut(path, string_rfind(path, '/'));
        clog_info("cut(rfind('/')) => ["string_fmt"], ["string_fmt"]", string_fmt_arg(parts.left), string_fmt_arg(parts.right));// [{FOO}/applets], [model_viewer.so]

        parts = string_cut(path, string_rfind(path, '.'));
        clog_info("cut(rfind('.')) => ["string_fmt"], ["string_fmt"]", string_fmt_arg(parts.left), string_fmt_arg(parts.right));// [{FOO}/applets/model_viewer], [so]
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

result_e main()
{
    _test_string_checks();
    _test_string_substr();
    _test_string_trim_chop();

    ////////////////////////////////////////

    // mutable storage (allocators needed)
    {
        // struct arena *arena = NULL;

        // struct string foo = make_string("/foo/%s/baz", "bar")
        // clog_info(foo);

        // struct string baz = string_replace(bar, ".so", ".ini")
        // clog_info(baz);
    }

    ////////////////////////////////////////

    // string lists
    {
        // struct string path = make_string("/path/to/foo/bar.png")
    }

    return RC_SUCCESS;
}
