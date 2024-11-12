////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#define _GNU_SOURCE

// standard library
#include <stdlib.h>

// standard library for I/O (files, streams, formatting, ...)
#include <stdio.h>

// standard type definitions (offsetof(), size_t, ...)
#include <stddef.h>

// boolean support
#include <stdbool.h>

// float type definitions (FLT_EPSILON, ...)
#include <float.h>

// portable types support
#include <stdint.h>

// variadiac functions support (va_list, ...)
#include <stdarg.h>

// character classification (isalpha(), ...)
#include <ctype.h>

// wide character support
#include <wchar.h>

// strings and arrays support
#include <string.h>

// basic math support
#include <math.h>

// basic error handling
#include <errno.h>

////////////////////////////////////////////////////////////

#include "types.h"
#include "macros.h"
#include "result.h"
