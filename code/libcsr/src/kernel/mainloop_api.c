////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/kernel/mainloop_api.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static result_e _api_run(struct mainloop* mainloop)
{
    return RC_FAILURE;
}

static bool _api_tick(struct mainloop* mainloop)
{
    return false;
}

////////////////////////////////////////////////////////////////////////////////

struct mainloop_api* mainloop_api_create()
{
    struct mainloop_api* api = malloc(sizeof(struct mainloop_api));
    check_mem(api);

    api->run = _api_run;
    api->tick = _api_tick;

    return api;

error:
    return NULL;
}

void mainloop_api_destroy(struct mainloop_api* api)
{
    check_ptr(api);

    free(api);

error:
    return;
}
