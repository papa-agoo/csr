////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "kernel.h"

#include "service/core_priv.h"
#include "service/video_priv.h"
#include "service/audio_priv.h"
#include "service/vfs_priv.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct kernel
{
    bool is_running;
    bool is_initialized;

    struct {
        struct ksrv_core core;
        struct ksrv_video video;
        struct ksrv_audio audio;
        struct ksrv_vfs vfs;
    } service;
};

struct kernel* kernel_ptr();

#define ksrv_ptr() (&kernel_ptr()->service)
#define ksrv_core_ptr() (&ksrv_ptr()->core)
#define ksrv_video_ptr() (&ksrv_ptr()->video)
#define ksrv_audio_ptr() (&ksrv_ptr()->audio)
#define ksrv_vfs_ptr() (&ksrv_ptr()->vfs)
