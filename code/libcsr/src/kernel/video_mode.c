////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/kernel/video_mode.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

void video_mode_preset_get_list(u32* count, string_cstr** list)
{
    check_ptr(count);
    check_ptr(list);

    static string_cstr presets[VIDEO_MODE_PRESET_MAX] =
    {
        [VIDEO_MODE_PRESET_HD_READY]    = "HD Ready (1280x720)",
        [VIDEO_MODE_PRESET_FULL_HD]     = "Full HD (1920x1080)",
        [VIDEO_MODE_PRESET_QUAD_HD]     = "Quad HD (2560x1440)",
        [VIDEO_MODE_PRESET_4K]          = "4K (3840x2160)",
        [VIDEO_MODE_PRESET_CUSTOM]      = "Custom",
    };

    *count = COUNT_OF(presets);
    *list = presets;

error:
    return;
}

void video_mode_preset_get_resolution(enum video_mode_preset preset, struct vec2 *resolution)
{
    check_expr(preset <= VIDEO_MODE_PRESET_MAX);
    check_ptr(resolution);

    switch (preset)
    {
        case VIDEO_MODE_PRESET_HD_READY:
            *resolution = make_vec2(1280, 720);
            break;

        case VIDEO_MODE_PRESET_FULL_HD:
            *resolution = make_vec2(1920, 1080);
            break;

        case VIDEO_MODE_PRESET_QUAD_HD:
            *resolution = make_vec2(2560, 1440);
            break;

        case VIDEO_MODE_PRESET_4K:
            *resolution = make_vec2(3840, 2160);
            break;
    }

error:
    return;
}

enum video_mode_preset video_mode_preset_from_resolution(struct vec2 resolution)
{
    if (resolution.w == 1280 && resolution.h == 720)
        return VIDEO_MODE_PRESET_HD_READY;

    if (resolution.w == 1920 && resolution.h == 1080)
        return VIDEO_MODE_PRESET_FULL_HD;

    if (resolution.w == 2560 && resolution.h == 1440)
        return VIDEO_MODE_PRESET_QUAD_HD;

    if (resolution.w == 3840 && resolution.h == 2160)
        return VIDEO_MODE_PRESET_4K;

error:
    return VIDEO_MODE_PRESET_CUSTOM;
}

struct video_mode video_mode_create(struct vec2 resolution)
{
    struct video_mode vm = {0};
    vm.preset = video_mode_preset_from_resolution(resolution);
    vm.resolution = resolution;

    return vm;
}

struct video_mode video_mode_create_from_preset(enum video_mode_preset preset)
{
    struct video_mode vm = {0};
    vm.preset = preset;

    video_mode_preset_get_resolution(preset, &vm.resolution);

    return vm;
}
