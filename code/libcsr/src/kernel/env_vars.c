////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/hashmap.h>
#include <csr/kernel/env_vars.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct env_vars
{
    struct arena *arena;
    struct hashmap *vars_map;
};

struct env_vars* env_vars_create()
{
    struct env_vars *vars = calloc(1, sizeof(struct env_vars));
    check_mem(vars);

    vars->arena = make_arena();
    check_mem(vars->arena);

    vars->vars_map = hashmap_create();
    check_mem(vars->vars_map);

    return vars;

error:
    if (vars) free(vars);

    return NULL;
}

void env_vars_destroy(struct env_vars *vars)
{
    check_ptr(vars);
    check_ptr(vars->arena);
    check_ptr(vars->vars_map);

    hashmap_destroy(vars->vars_map);
    arena_destroy(vars->arena);

error:
    return;
}

bool env_vars_set_var(struct env_vars *vars, string_cstr name, string_cstr value)
{
    check_ptr(vars);
    check_ptr(vars->arena);
    check_ptr(vars->vars_map);

    // copy string data
    struct string str = string_copy(vars->arena, make_string_from_cstr(value));
    // struct string str = string_create_fmt(vars->arena, "%s", value);
    check_expr(string_is_valid(str));

    // copy string struct
    void* value_ptr = memcpy(arena_push(vars->arena, sizeof(struct string)), &str, sizeof(struct string));
    check_ptr(value_ptr);

    // map name to the string struct
    hashmap_set(vars->vars_map, name, value_ptr);

    return true;

error:
    return false;    
}

struct string env_vars_get_var(struct env_vars *vars, string_cstr name)
{
    check_ptr(vars);
    check_ptr(vars->vars_map);
    check_ptr(name);

    struct string *str = hashmap_get(vars->vars_map, name);
    check_ptr(str);

    return *str;

error:
    return make_string("");
}

string_cstr env_vars_get_var_cstr(struct env_vars *vars, string_cstr name)
{
    return string_get_cstr(vars->arena, env_vars_get_var(vars, name));
}

static bool _find_var_in_string(struct string str_in, struct string* str_out)
{
    s32 var_begin = string_find_at(str_in, 0, '{');
    check_quiet(var_begin != -1);

    s32 var_end = string_find_at(str_in, var_begin, '}');
    check_quiet(var_end != -1);

    *str_out = string_substr(str_in, var_begin, var_end - var_begin + 1);

    return true;

error:
    return false;
}

struct string env_vars_expand_str(struct env_vars *vars, string_cstr str)
{
    check_ptr(vars);

    struct string var = {0};
    struct string result = make_string_from_cstr(str);

    while(_find_var_in_string(result, &var))
    {
        // extract var name ({FOO} => FOO)
        struct string var_name = string_rchop(string_chop(var, 1), -1);

        // lookup var value
        struct string var_value = env_vars_get_var(vars, string_get_cstr(vars->arena, var_name)); 

        // save result
        result = string_replace(vars->arena, result, var, var_value);
    }

    return result;

error:
    return make_string_from_cstr(str);
}

string_cstr env_vars_expand_str_cstr(struct env_vars *vars, string_cstr str)
{
    return string_get_cstr(vars->arena, env_vars_expand_str(vars, str));
}
