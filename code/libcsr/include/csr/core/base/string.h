////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/memory/arena.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct string
{
    const u8 *ptr;
    u64 length;
};

struct string_pair
{
    struct string left;
    struct string right;
};

typedef const char* string_cstr;

#define make_string(str)            string_create(str, sizeof(str) - 1)
#define make_string_from_cstr(str)  string_create(str, strlen(str))

////////////////////////////////////////////////////////////////////////////////

string_cstr cstr_from_string(struct arena *arena, struct string str);

struct string string_create(const u8* str, u64 length);
struct string string_create_fmt(struct arena *arena, string_cstr fmt, ...);
struct string string_create_vfmt(struct arena *arena, string_cstr fmt, va_list args);

struct string string_copy(struct arena *arena, struct string str_in);

bool string_is_valid(struct string str);
bool string_equals(struct string str, struct string other);
bool string_contains(struct string str, struct string pattern);
bool string_starts_with(struct string str, struct string pattern);
bool string_ends_with(struct string str, struct string pattern);

struct string string_substr(struct string str, s32 offset, s32 length);

s32 string_find(struct string str, u8 delimiter);
s32 string_rfind(struct string str, u8 delimiter);

s32 string_find_at(struct string str, u32 position, u8 delimiter);
s32 string_rfind_at(struct string str, u32 position, u8 delimiter);

struct string string_trim(struct string str);
struct string string_rtrim(struct string str);

struct string string_chop(struct string str, u32 length);
struct string string_rchop(struct string str, u32 length);

struct string_pair string_cut(struct string str, u32 position);

// string lists
// void string_join();
// void string_split();
// void string_replace();

////////////////////////////////////////////////////////////////////////////////

// printf helpers
#define string_fmt "%.*s"
#define string_fmt_arg(str) (s32)(str).length, (str).ptr
