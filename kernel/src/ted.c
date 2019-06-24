#include "kernel/ted.h"
#include "kernel/memory.h"
#include "platform/clock.h"
#include "lib/list.h"

typedef struct ted_event_ctx_t {
    ted_event     handler;
    void         *ctx;

    int32_t      time;
} ted_event_ctx;

static
list_ifc     *ted_event_queue = NULL;

static
int ted_init(void) {
    if (ted_event_queue) {
        return -1;
    }

    ted_event_queue = list_create();
    if (!ted_event_queue) {
        return -2;
    }

    return 0;
}

static const
list_node *find_suitable_node(int32_t time) {
    const list_node *node = ted_event_queue->get_front(ted_event_queue);

    while (node) {
        ted_event_ctx *ctx = node->ptr;

        if (ctx->time > time) {
            break;
        }

        if (!node->nxt) {
            return NULL;
        }

        node = node->nxt;
    }

    return node;
}

int ted_schedule(ted_event event, void *event_ctx, uint32_t msecs) {
    if (!ted_event_queue && ted_init()) {
        return -1;
    }

    ted_event_ctx *ctx = cell_alloc(sizeof(ted_event_ctx));
    if (!ctx) {
        return -2;
    }

    ctx->handler = event;
    ctx->time    = clock_get() + msecs;
    ctx->ctx     = event_ctx;

    const list_node *node = find_suitable_node(ctx->time);

    if (node) {
        if (!ted_event_queue->insert_before(ted_event_queue,
            node, ctx)) {
            return -3;
        }
    } else {
        if (!ted_event_queue->insert_after(ted_event_queue,
            node, ctx)) {
            return -3;
        }
    }



    return 0;
}

void ted_dispatch(void) {
    if (!ted_event_queue && ted_init()) {
        return;
    }

    const list_node *node = ted_event_queue->get_front(ted_event_queue);

    ted_event_ctx *ctx = (node) ? node->ptr : NULL;

    int32_t now = clock_get();

    while (node && (now - ctx->time) < 0) {
        ctx = node->ptr;

        ctx->handler(ctx->ctx);

        ted_event_queue->delete(ted_event_queue, node, cell_free);

        node = ted_event_queue->get_front(ted_event_queue);
    }
}
