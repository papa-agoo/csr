////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/core/base/string.h>

#include <printf.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static s32 print_string(FILE *stream, const struct printf_info *info, const void *const *args)
{
	const struct string *str = *((const struct string **)(args[0]));

	return fprintf(stream, "%.*s", str->length, str->ptr);
}

static s32 print_string_arginfo(const struct printf_info *info, size_t n, int *argtypes, int *sizes)
{
	if (n > 0) {
		argtypes[0] = PA_POINTER;
	}

	return 1;
}

__attribute__((constructor)) static void _register_string_printf()
{
    clog_trace("registering printf specifier: S");

    register_printf_specifier('S', print_string, print_string_arginfo);
}
