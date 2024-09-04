////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/base/string.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct string string_create(const u8* str, u64 length)
{
    return (struct string) {.ptr = str, .length = length};
}

struct string string_create_fmt(struct arena *arena, string_cstr fmt, ...)
{
    struct string str = {0};

    // 1. calc formatted string length
    //      - str.length = snprintf(0, 0, ...)

    // 2. allocate memory
    //      - str.ptr = arena_allocate()

    // 3. write the formatted string
    //      - snprintf(str.ptr, str.length, ...)

error:
    return make_string("");
}

struct string string_create_vfmt(struct arena *arena, string_cstr fmt, va_list args)
{
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
    check_expr(string_is_valid(pattern));

    if (str.length < pattern.length ) {
        return false;
    }

    s32 idx = 0;
    u8 delimiter = pattern.ptr[0];

    while((idx = string_find(str, delimiter)) >= 0)
    {
        // chop everything till the first matching char
        str = string_chop(str, idx);

        // check wether the remaining string starts with the wanted pattern
        if (string_starts_with(str, pattern)) {
            return true;
        }

        // advance one char for the next string_find() call
        str = string_substr(str, 1, 0);
    }

error:
    return false;
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
    check_expr(string_is_valid(str));

    for (u32 i = 0; i < str.length; i++)
    {
        if (str.ptr[i] == delimiter) {
            return i;
        }
    }

error:
    return -1;
}

s32 string_rfind(struct string str, u8 delimiter)
{
    check_expr(string_is_valid(str));

    for (s32 i = str.length - 1; i >= 0; i--)
    {
        if (str.ptr[i] == delimiter) {
            return i;
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
