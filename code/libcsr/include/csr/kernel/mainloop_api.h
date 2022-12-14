////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct mainloop;

struct mainloop_api
{
    result_e (*run)(struct mainloop* mainloop);
    bool (*tick)(struct mainloop* mainloop);
};

struct mainloop_api* mainloop_api_create();
void mainloop_api_destroy(struct mainloop_api* api);
