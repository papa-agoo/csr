////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct env_vars;

struct env_vars* env_vars_create();
void env_vars_destroy(struct env_vars *vars);

bool env_vars_set_var(struct env_vars *vars, string_cstr name, string_cstr value);

struct string env_vars_get_var(struct env_vars *vars, string_cstr name);
string_cstr env_vars_get_var_cstr(struct env_vars *vars, string_cstr name);

struct string env_vars_expand_str(struct env_vars *vars, string_cstr str);
string_cstr env_vars_expand_str_cstr(struct env_vars *vars, string_cstr str);
