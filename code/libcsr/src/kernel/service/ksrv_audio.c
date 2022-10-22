////////////////////////////////////////////////////////////////////////////////////////////////////

#define KLOG_MODULE_NAME kernel

#include <csr/kernel.h>
#include <csr/kernel/kernel_priv.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

void ksrv_audio_conf_defaults(struct ksrv_audio_conf *conf)
{
    check_ptr(conf);

    conf->volume = 100;

error:
    return;
}

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
