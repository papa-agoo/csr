////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/kernel/mainloop.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static result_e _api_run(struct mainloop* mainloop)
{
    check_ptr(mainloop);
    check_ptr(mainloop->api);
    check_ptr(mainloop->api->tick);

    while (mainloop->api->tick(mainloop));

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

static bool _api_tick(struct mainloop* mainloop)
{
    check_ptr(mainloop);

    return mainloop->is_running;

error:
    return false;
}

////////////////////////////////////////////////////////////////////////////////

struct mainloop* mainloop_create()
{
    struct mainloop* mainloop = malloc(sizeof(struct mainloop));
    check_mem(mainloop);

    mainloop->api = mainloop_api_create();
    check_ptr(mainloop->api);

    mainloop->api->run = _api_run;
    mainloop->api->tick = _api_tick;

    return mainloop;

error:

    if (mainloop) {
        free(mainloop);
    }

    return NULL;
}

void mainloop_destroy(struct mainloop* mainloop)
{
    check_ptr(mainloop);

    mainloop_api_destroy(mainloop->api);

    free(mainloop);

error:
    return;
}

result_e mainloop_run(struct mainloop* mainloop)
{
    check_ptr(mainloop);
    check_ptr(mainloop->api);
    check_ptr(mainloop->api->run);

    mainloop->is_running = true;

    return mainloop->api->run(mainloop);

error:
    return RC_FAILURE;
}

void mainloop_exit(struct mainloop* mainloop)
{
    check_ptr(mainloop);

    mainloop->is_running = false;

error:
    return;
}
