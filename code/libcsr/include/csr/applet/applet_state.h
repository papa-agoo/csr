////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <csr/core/clock.h>
#include <csr/core/config.h>

#include <csr/core/hashmap.h>
#include <csr/kernel/events/event.h>

#include <csr/graphics/ui/ctx.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

struct applet_state
{
    struct clock *clock;
    struct config *config;

    struct ui_ctx *ui;
    struct hashmap *screens;

    struct {
        struct hid_callbacks hid;
    } callbacks;
};

void applet_state_init(struct applet *applet);
void applet_state_quit(struct applet *applet);
