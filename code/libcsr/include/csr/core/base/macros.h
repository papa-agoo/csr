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

// inspired by Zed Shaw's debug macros :)
#define clog_info(M, ...) \
    fprintf(stderr, "[ INFO ] " M "\n", ##__VA_ARGS__)

#define clog_notice(M, ...) \
    fprintf(stderr, "[ NOTICE ] " M "\n", ##__VA_ARGS__)

#define clog_warn(M, ...) \
    fprintf(stderr, "[ WARN ] %s() : " M "\n", __FUNCTION__, ##__VA_ARGS__)

#define clog_error(M, ...) \
    fprintf(stderr, "[ ERROR ] (%s:%d) %s() : " M "\n", \
        __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#ifdef NDEBUG
#define clog_trace(M, ...)
#else
#define clog_trace(M, ...) \
    fprintf(stderr, "[ TRACE ] (%s:%d) %s() " M "\n", \
        __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#endif

#define check(A, M, ...) \
    if(!(A)) { clog_error(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_trace(A, M, ...) \
    if(!(A)) { clog_trace(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_mem(A) \
    check((A), "mem allocation failed : %s", #A)

#define check_ptr(A) \
    check((A) != NULL, "%s cannot be NULL", #A)

#define check_expr(A) \
    check((A), "expr. check failed : ( %s )", #A)

#define check_quiet(A) \
    if(!(A)) { errno=0; goto error; }

////////////////////////////////////////////////////////////////////////////////

#define BIT(x) (1u << (x))

#define CSR_INLINE __attribute__((always_inline)) inline static

#define CSR_OBJECT_DECLARE(_name) typedef struct { guid handle; } _name

// stolen from google chromium :)
#define COUNT_OF(x) \
    ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
