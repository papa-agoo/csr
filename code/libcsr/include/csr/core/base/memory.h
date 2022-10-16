////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

// https://github.com/Theldus/safemalloc

#define csr_malloc(s) malloc(s)
#define csr_calloc(c, s) calloc(c, s)
#define csr_realloc(p, s) realloc(c, s)
#define csr_free(p) free(p)

// p == null
//  warning

// p != null
//  free(p), p = 0
