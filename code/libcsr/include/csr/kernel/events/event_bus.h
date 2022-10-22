////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "event.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void (*event_handler_cb_t)(struct event* event);

void event_bus_register_handler(enum event_type type, event_handler_cb_t callback);

void event_bus_push_event(struct event* event);
struct event* event_bus_pop_event();

void event_bus_dispatch_events();
