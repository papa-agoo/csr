////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/clock.h>
#include <csr/core/config.h>

#include <csr/kernel/events/event.h>

#include <csr/graphics/screen.h>
#include <csr/graphics/ui/ctx.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct arena;

struct applet_state
{
    struct arena *arena;

    struct clock *clock;
    struct config *config;

    struct ui_ctx *ui;

    struct {
        struct hid_callbacks hid;
    } callbacks;
};

void applet_state_init(struct applet *applet);
void applet_state_quit(struct applet *applet);
