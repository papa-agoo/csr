////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "mainloop_api.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct mainloop
{
    bool is_running;

    struct mainloop_api* api;
};

struct mainloop* mainloop_create();
void mainloop_destroy(struct mainloop* mainloop);

result_e mainloop_run(struct mainloop* mainloop);
void mainloop_exit(struct mainloop* mainloop);
