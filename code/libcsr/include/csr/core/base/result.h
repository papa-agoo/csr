////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

enum result_codes
{
    RC_SUCCESS = 0,
    RC_FAILURE = 1,

    RC_INIT_FAILED,
    RC_FILE_NOT_FOUND,
};

typedef enum result_codes result_e;

#define R_SUCCESS(rc) ((rc) == RC_SUCCESS)
#define R_FAILURE(rc) ((rc) != RC_SUCCESS)
#define R_FAILSTATE(expr) ((expr) ? RC_SUCCESS : RC_FAILURE)

#define check_result(A) check_expr(A == RC_SUCCESS)
