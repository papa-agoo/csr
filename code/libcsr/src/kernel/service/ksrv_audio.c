////////////////////////////////////////////////////////////////////////////////////////////////////

#define KLOG_MODULE_NAME kernel

#include <csr/kernel.h>
#include <csr/kernel/kernel_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

result_e ksrv_audio_init(struct kernel_init_info *init_info)
{
    klog_info("initializing audio service ...");

    ////////////////////////////////////////

    struct ksrv_audio *srv = ksrv_audio_ptr();

    // klog_info(" - xal driver : NONE");

    ////////////////////////////////////////

    srv->is_initialized = false;

    return RC_SUCCESS;

error:
    return RC_FAILURE;
}

void ksrv_audio_quit()
{
    klog_info("quitting audio service ...");

    ////////////////////////////////////////

    struct ksrv_audio *srv = ksrv_audio_ptr();

    // ...

    ////////////////////////////////////////

    srv->is_initialized = false;
}
