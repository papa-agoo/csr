////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/config.h>
#include <csr/core/file_io.h>

#define KLOG_MODULE_NAME application
#include <csr/kernel.h>

#include <csr/graphics/ui.h>

#include <csr/applet/applet_mgr.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define ENV_APP_NAME                "CSR"
#define ENV_APP_VERSION             "0.0.4"

#define ENV_APPLET_DIR              "./applets"
#define ENV_RESOURCE_DIR            "./resources"

#define ENV_APP_HOME_DIR            "{USER_HOME_DIR}/.csr"
#define ENV_APP_CONFIG_DIR          "{APP_HOME_DIR}/config"
#define ENV_APPLET_CONFIG_DIR       "{APP_CONFIG_DIR}/applet"

#define ENV_APP_INI_FILE            "{APP_CONFIG_DIR}/application.ini"
#define ENV_IMGUI_INI_FILE          "{APP_CONFIG_DIR}/imgui.ini"

// #define ENV_APPLET_PLUGIN_FILE      "{APPLET_DIR}/{applet_name}.so
// #define ENV_APPLET_CONFIG_FILE      "{APPLET_CONFIG_DIR}/{applet_name}.ini

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

////////////////////////////////////////////////////////////

result_e application_init();
void application_quit();

void application_run();
void application_stop();

void application_request_restart(); // FIXME message system
bool application_has_input_focus();

struct application* application_ptr();

#define application_conf_ptr() (&application_ptr()->conf)
#define application_callbacks_ptr() (&application_ptr()->callbacks)
