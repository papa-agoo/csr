////////////////////////////////////////////////////////////////////////////////////////////////////

#include <csr/kernel/events.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

// event handler
struct event_handler
{
    enum event_type type;
    event_handler_cb_t callback;
};

// message pool
#define MAX_EVENT_MESSAGES 64

struct message_pool
{
    u32 count;

    u32 head;
    u32 tail;

    struct event buffer[MAX_EVENT_MESSAGES];
};

// handler pool
#define MAX_EVENT_HANDLERS 64

struct handler_pool
{
    u32 count;

    struct event_handler buffer[MAX_EVENT_HANDLERS];
};

////////////////////////////////////////////////////////////////////////////////

static struct message_pool g_messages = {0};
static struct handler_pool g_handlers = {0};

////////////////////////////////////////////////////////////////////////////////

const char* event_type_str(enum event_type type)
{
    switch (type)
    {
        case EVENT_NONE:
            return "EVENT_NONE";

        // window events
        case EVENT_WINDOW_CLOSE:
            return "EVENT_WINDOW_CLOSE";

        case EVENT_WINDOW_RESIZE:
            return "EVENT_WINDOW_RESIZE";

        case EVENT_WINDOW_ERROR:
            return "EVENT_WINDOW_ERROR";

        // mouse events
        case EVENT_MOUSE_MOVE:
            return "EVENT_MOUSE_MOVE";

        case EVENT_MOUSE_WHEEL:
            return "EVENT_MOUSE_WHEEL";

        case EVENT_MOUSE_BUTTON_DOWN:
            return "EVENT_MOUSE_BUTTON_DOWN";

        case EVENT_MOUSE_BUTTON_UP:
            return "EVENT_MOUSE_BUTTON_UP";

        // keyboard events
        case EVENT_KBD_KEY_DOWN:
            return "EVENT_KBD_KEY_DOWN";

        case EVENT_KBD_KEY_UP:
            return "EVENT_KBD_KEY_UP";

        // gamepad events
        case EVENT_GAMEPAD_BUTTON_DOWN:
            return "EVENT_GAMEPAD_BUTTON_DOWN";

        case EVENT_GAMEPAD_BUTTON_UP:
            return "EVENT_GAMEPAD_BUTTON_UP";

        default:
            return "EVENT TYPE UNKNOWN";
    }
}

void event_bus_register_handler(enum event_type type, event_handler_cb_t callback)
{
    check_ptr(callback);

    check_expr(type != EVENT_NONE);
    check_expr(g_handlers.count < MAX_EVENT_HANDLERS);

    clog_trace("registering handler: %s (%p)", event_type_str(type), callback);

    struct event_handler handler = {0};
    handler.type = type;
    handler.callback = callback;

    g_handlers.buffer[g_handlers.count++] = handler;

error:
    return;
}

void event_bus_push_event(struct event *event)
{
    if (g_handlers.count == 0) return;

    check_ptr(event);

    check_expr(event->type != EVENT_NONE);
    check_expr(g_messages.count < MAX_EVENT_MESSAGES)

    u32 idx_write = g_messages.tail;

    g_messages.tail = (g_messages.tail + 1) % MAX_EVENT_MESSAGES;
    g_messages.count++;

    g_messages.buffer[idx_write] = *event;

error:
    return;
}

struct event* event_bus_pop_event()
{
    if (g_messages.count == 0 ) return NULL;

    u32 idx_read = g_messages.head;

    g_messages.head = (g_messages.head + 1) % MAX_EVENT_MESSAGES;
    g_messages.count--;

    return &g_messages.buffer[idx_read];
}

void event_bus_dispatch_events()
{
    if (g_handlers.count == 0) return;

    struct event* event = NULL;
    struct event_handler* handler = NULL;

    while (event = event_bus_pop_event())
    {
        // broadcast messages
        for (u32 i = 0; i < g_handlers.count; i++)
        {
            handler = &g_handlers.buffer[i];

            if (event->type == handler->type) {
                handler->callback(event);
            }
        }
    }
}
