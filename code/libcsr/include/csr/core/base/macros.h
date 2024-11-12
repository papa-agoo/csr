////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

// generic helpers
#define _mstring(s) #s
#define _mexpand(s) s

#define _mcall(x, ...) do { x ; } while (0)
#define _mvoid(x, ...) _mcall((void)0)

#define make_compound_literal(T, ...) ((T) {__VA_ARGS__})

////////////////////////////////////////////////////////////////////////////////

// shell color escape codes
#define _escape_code(C, S) "\e[" C "m" S "\e[0m"

#define EC_RED(S)       _escape_code("31", S)
#define EC_REDB(S)      _escape_code("1;31", S)

#define EC_GREEN(S)     _escape_code("32", S)
#define EC_GREENB(S)    _escape_code("1;32", S)

#define EC_YELLOW(S)    _escape_code("33", S)
#define EC_YELLOWB(S)   _escape_code("1;33", S)

#define EC_BLUE(S)      _escape_code("34", S)
#define EC_BLUEB(S)     _escape_code("1;34", S)

#define EC_PURPLE(S)    _escape_code("35", S)
#define EC_PURPLEB(S)   _escape_code("1;35", S)

#define EC_CYAN(S)      _escape_code("36", S)
#define EC_CYANB(S)     _escape_code("1;36", S)

// "inspired" by Zed Shaw's debug macros :)
#define clog_newline() fprintf(stderr, "\n")

#define clog_info(M, ...) \
    fprintf(stderr, "[ "EC_GREENB("INFO")" ] " M "\n", ##__VA_ARGS__)

#define clog_notice(M, ...) \
    fprintf(stderr, "[ "EC_BLUEB("NOTICE")" ] " M "\n", ##__VA_ARGS__)

#define clog_hint(T, M, ...) \
    fprintf(stderr, "[ " T " ] "EC_CYANB("%s")"() : " M "\n", __FUNCTION__, ##__VA_ARGS__)

#define clog_warn(M, ...) \
    clog_hint(EC_YELLOWB("WARN"), M, ##__VA_ARGS__)

#define clog_error(M, ...) \
    fprintf(stderr, "[ "EC_REDB("ERROR")" ] (%s:%d) "EC_CYANB("%s")"() : " M "\n", \
        __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#ifdef NDEBUG
#define clog_trace(M, ...)
#else
#define clog_trace(M, ...) \
    fprintf(stderr, "[ "EC_PURPLEB("TRACE")" ] (%s:%d) "EC_CYANB("%s")"() " M "\n", \
        __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#endif

////////////////////////////////////////////////////////////////////////////////

#define check(A, M, ...) \
    if(!(A)) { clog_error(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_trace(A, M, ...) \
    if(!(A)) { clog_trace(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_mem(A) \
    check((A), "mem allocation failed : %s", EC_RED(#A))

#define check_ptr(A) \
    check((A) != NULL, "%s cannot be NULL", EC_RED(#A))

#define check_expr(A) \
    check((A), EC_YELLOW("expr. check failed : ( %s )"), EC_RED(#A))

#define check_quiet(A) \
    if(!(A)) { errno=0; goto error; }

////////////////////////////////////////////////////////////////////////////////

#define BIT(x) (1u << (x))

#define CSR_INLINE __attribute__((always_inline)) inline static

#define CSR_OBJECT_DECLARE(_name) typedef struct { guid handle; } _name

// stolen from google chromium :)
#define COUNT_OF(x) \
    ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

////////////////////////////////////////////////////////////////////////////////

#define KiloBytes(n) (1024 * n)
#define MegaBytes(n) (1024 * KiloBytes(n))
#define GigaBytes(n) (1024 * MegaBytes(n))

// checks float equality with some error tolerance (stolen from spartan engine :))
#define f32_equal(a, b) (a + FLT_EPSILON >= b && a - FLT_EPSILON <= b)

// cut float precision to 1m digits (ie: 1000 => 1.000)
#define f32_precision(n, m) ((roundf(m * n) / m) + 0.0)
