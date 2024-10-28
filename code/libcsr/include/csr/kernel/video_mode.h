////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/base.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

enum video_mode_preset
{
    VIDEO_MODE_PRESET_HD_READY,
    VIDEO_MODE_PRESET_FULL_HD,
    VIDEO_MODE_PRESET_QUAD_HD,
    VIDEO_MODE_PRESET_4K,
    VIDEO_MODE_PRESET_CUSTOM,

    VIDEO_MODE_PRESET_MAX,
};

void video_mode_preset_get_list(u32* count, string_cstr** list);
void video_mode_preset_get_resolution(enum video_mode_preset preset, struct vec2 *resolution);
enum video_mode_preset video_mode_preset_from_resolution(struct vec2 resolution);

struct video_mode
{
    enum video_mode_preset preset;

    struct vec2 resolution;
};

struct video_mode video_mode_create(struct vec2 resolution);
struct video_mode video_mode_create_from_preset(enum video_mode_preset preset);
