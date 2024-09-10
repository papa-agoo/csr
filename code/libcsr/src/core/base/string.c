////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/base/string.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

string_cstr string_get_cstr(struct arena *arena, struct string str)
{
    check_ptr(arena);
    check_expr(string_is_valid(str));

    void* ptr = arena_push(arena, str.length + 1);
    check_ptr(ptr);

    s32 bytes_written = snprintf(ptr, str.length + 1, string_fmt, string_fmt_arg(str));
    check_expr(bytes_written == str.length);

    return ptr;

error:
    return "";
}

struct string string_create(const u8* str, u64 length)
{
    return (struct string) {.ptr = str, .length = length};
}

struct string string_create_fmt(struct arena *arena, string_cstr fmt, ...)
{
    check_ptr(arena);
    check_ptr(fmt);

    ////////////////////////////////////////

    va_list args;
    va_start(args, fmt);

    struct string str = string_create_vfmt(arena, fmt, args);

    va_end(args);

    ////////////////////////////////////////

    return str;

error:
    return make_string("");
}

struct string string_create_vfmt(struct arena *arena, string_cstr fmt, va_list args)
{
    check_ptr(arena);
    check_ptr(fmt);
    check_ptr(args);

    ////////////////////////////////////////

    va_list args_new;
    va_copy(args_new, args);

    // 1. calc formatted string length (including null terminator)
    u64 str_length = vsnprintf(NULL, 0, fmt, args) + 1;
    check_quiet(str_length > 1);

    // 2. request memory address
    void* str_ptr = arena_push(arena, str_length);
    check_ptr(str_ptr);

    // 3. write the formatted string (including null terminator)
    s32 bytes_written = vsnprintf(str_ptr, str_length, fmt, args_new);
    check_expr(bytes_written == str_length - 1);

    va_end(args_new);

    ////////////////////////////////////////

    // exclude null terminator for str_length
    return (struct string) {.length = str_length - 1, .ptr = str_ptr};

error:
    return make_string("");
}

struct string string_replace(struct arena *arena, struct string str, struct string old, struct string new)
{
    check_ptr(arena);
    check_expr(string_is_valid(new));

    s32 position_old = -1;

    while ((position_old = string_find_str(str, old)) != -1)
    {
        // calc offsets
        s32 old_begin = position_old;
        s32 old_end = old_begin + old.length;

        // extract left + right sides
        struct string left = (old_begin > 0) ? string_substr(str, 0, old_begin) : make_string("");
        struct string right = (old_end < str.length) ? string_substr(str, old_end, 0) : make_string("");

        // merge left + middle (new) + right
        str = string_create_fmt(arena, string_fmt""string_fmt""string_fmt,
            string_fmt_arg(left), string_fmt_arg(new), string_fmt_arg(right));
    }

error:
    return str;
}

struct string string_copy(struct arena *arena, struct string str_in)
{
    check_ptr(arena);
    check_expr(string_is_valid(str_in));

    void* data = memcpy(arena_push(arena, str_in.length), str_in.ptr, str_in.length);
    check_ptr(data);

    struct string str_out = {0};
    str_out.length = str_in.length;
    str_out.ptr = data;

    return str_out;

error:
    return make_string("");
}

bool string_is_valid(struct string str)
{
    return str.ptr != NULL && str.length > 0;
}

bool string_equals(struct string str, struct string other)
{
    check_expr(string_is_valid(str));
    check_expr(string_is_valid(other));

    if (str.length != other.length ) {
        return false;
    }

    return memcmp(str.ptr, other.ptr, other.length) == 0;

error:
    return false;
}

bool string_contains(struct string str, struct string pattern)
{
    return string_find_str(str, pattern) != -1;
}

bool string_starts_with(struct string str, struct string pattern)
{
    if (str.length < pattern.length ) {
        return false;
    }

    return string_equals(string_substr(str, 0, pattern.length), pattern);
}

bool string_ends_with(struct string str, struct string pattern)
{
    if (str.length < pattern.length ) {
        return false;
    }

    return string_equals(string_substr(str, str.length - pattern.length, 0), pattern);
}

struct string string_substr(struct string str, s32 offset, s32 length)
{
    check_expr(string_is_valid(str));

    ////////////////////////////////////////

    // calculate start position
    //    - 0. zero offset      : offset
    //    - 1. positive offset  : offset
    //    - 2. negative offset  : str.length - abs(offset)
    u32 start_pos = (offset >= 0) ? offset : str.length + offset;
    check_expr(start_pos < str.length);

    ////////////////////////////////////////

    // calculate end position
    //    - 0. zero length      : str.length
    //    - 1. positive length  : start_pos + length
    //    - 2. negative length  : str.length - abs(length)
    u32 end_pos = str.length;

    if (length != 0) {
        end_pos = (length < 0) ? str.length + length : start_pos + length;
    }

    check_expr(end_pos <= str.length);

    ////////////////////////////////////////

    // update offset and length
    check_expr(start_pos < end_pos);

    str.ptr += start_pos;
    str.length = end_pos - start_pos;

error:
    return str;
}

s32 string_find(struct string str, u8 delimiter)
{
    return string_find_at(str, 0, delimiter);
}

s32 string_rfind(struct string str, u8 delimiter)
{
    return string_rfind_at(str, str.length - 1, delimiter);
}

s32 string_find_at(struct string str, u32 position, u8 delimiter)
{
    check_expr(string_is_valid(str));
    check_expr(position >= 0 && position < str.length);

    // left to right
    for (u32 i = position; i < str.length; i++)
    {
        if (str.ptr[i] == delimiter) {
            return i;
        }
    }

error:
    return -1;
}

s32 string_rfind_at(struct string str, u32 position, u8 delimiter)
{
    check_expr(string_is_valid(str));
    check_expr(position >= 0 && position < str.length);

    // right to left
    for (s32 i = position; i >= 0; i--)
    {
        if (str.ptr[i] == delimiter) {
            return i;
        }
    }

error:
    return -1;
}

s32 string_find_str(struct string str, struct string pattern)
{
    check_expr(string_is_valid(pattern));

    if (str.length < pattern.length) {
        return -1;
    }

    s32 position = -1;
    u8 delimiter = pattern.ptr[0];

    while((position = string_find_at(str, position + 1, delimiter)) != -1)
    {
        if (string_starts_with(string_chop(str, position), pattern)) {
            return position;
        }
    }

error:
    return -1;
}

struct string string_trim(struct string str)
{
    check_expr(string_is_valid(str));

    for (s32 i = 0; i < str.length; i++)
    {
        if (!isspace(str.ptr[i])) {
            return string_substr(str, i, str.length - i);
        }
    }

error:
    return str;
}

struct string string_rtrim(struct string str)
{
    check_expr(string_is_valid(str));

    for (s32 i = str.length - 1; i >= 0; i--)
    {
        if (!isspace(str.ptr[i])) {
            return string_substr(str, 0, i + 1);
        }
    }

error:
    return str;
}

struct string string_chop(struct string str, u32 length)
{
    return string_substr(str, length, 0);
}

struct string string_rchop(struct string str, u32 length)
{
    return string_substr(str, 0, length);
}

struct string_pair string_cut(struct string str, u32 position)
{
    check_expr(string_is_valid(str));

    struct string_pair pair = {0};
    pair.left = string_rchop(str, position);
    pair.right = string_chop(str, position + 1);

    return pair;

error:
    return (struct string_pair) {0};
}
