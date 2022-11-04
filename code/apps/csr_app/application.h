////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/path.h>
#include <csr/core/config.h>

#define KLOG_MODULE_NAME application
#include <csr/kernel.h>

#include <csr/graphics/ui.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define ENV_APP_NAME                "CSR"
#define ENV_APP_VERSION             "0.0.2"

#define ENV_APP_HOME_DIR            "{USER_HOME_DIR}/.csr"
#define ENV_APP_CONFIG_DIR          "{APP_HOME_DIR}/config"
#define ENV_APPLET_CONFIG_DIR       "{APP_CONFIG_DIR}/applet"

#define ENV_APPLET_DB_SCAN_PATH     "./applets"
#define ENV_RESOURCE_PATH_FONTS     "./resources/fonts"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct application_conf
{
    struct config *user;

    struct {
        struct ksrv_core_conf core;
        struct ksrv_video_conf video;
        struct ksrv_audio_conf audio;
    } kernel;

    struct ui_conf ui;
};

struct application
{
    bool is_running;
    bool is_initialized;

    bool needs_restart;
    bool has_input_focus;

    struct {
        struct hid_callbacks hid;
    } callbacks;

    struct application_conf conf;
};

struct application* application_ptr();

#define application_conf_ptr() (&application_ptr()->conf)
#define application_callbacks_ptr() (&application_ptr()->callbacks)
