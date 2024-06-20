#include "kernel/thread.h"
#include "lib/string.h"
#include "kernel/memory.h"
#include "arch/svc.h"
#include "common/log.h"
#include "platform/clock.h"

/**
 * NOTE: stack and idler stack size are in heap alignment units
 */
#define THREAD_DEFAULT_STACK_SIZE    (128)

typedef struct scheduler_ctx_t {
    scheduler_ifc         ifc;

    list_ifc             *threads;

    const thread_ctx     *last_thread;

    uint16_t              pid;
} __attribute__((aligned(4))) scheduler_ctx;

//! forward declarations
static
int scheduler_schedule(core_context **old_ctx, core_context **new_ctx);
static
int scheduler_add_thread(thread_ctx *thread);
static
void scheduler_destroy(void);

static scheduler_ctx             sched_ctx = {
    .ifc = {
        .schedule           = scheduler_schedule,
        .add_thread         = scheduler_add_thread,
        .destroy            = scheduler_destroy,
    },
};

scheduler_ifc            *scheduler    = NULL;

static
void deallocate_thread_ctx(void *ctx) {
    thread_ctx *thread_to_del = (thread_ctx*) ctx;

    core_context_deinit(&thread_to_del->data->context);
    cell_free((void*) thread_to_del->data);
    cell_free((void*) thread_to_del->name);
    cell_free(thread_to_del);
}

static
void delete_thread(scheduler_ctx *ctx, const list_node *thread_node) {
    thread_ctx *thread_to_del = thread_node->ptr;

    const list_node *node = ctx->threads->get_front(ctx->threads);
    while (node) {
        thread_ctx *thread = node->ptr;

        if (thread == thread_to_del) {
            ctx->threads->delete(ctx->threads, node, NULL);
        }

        node = node->nxt;
    }

    deallocate_thread_ctx(thread_to_del);
}

static const
thread_ctx *get_best_thread(void) {
    const thread_ctx *best_thread = NULL;

    int now = clock_get();

    foreach_srv (srv) {
        if ((srv->data->wakeup - now) > 0) {
            continue;
        }

        // ! keep up to date
        srv->data->wakeup = now;

        if (!(srv->data->state.flags & THREAD_ACTIVE)) {
            continue;
        }

        if (!best_thread || srv->data->state.priority > best_thread->data->state.priority) {
            best_thread = srv;
        }
    }

    const list_node      *node = sched_ctx.threads->get_front(sched_ctx.threads);
    while (node) {
        thread_ctx *thread = node->ptr;

        if (!(thread->data->state.flags & THREAD_ALIVE)) {
            delete_thread(&sched_ctx, node);
            goto next;
        }

        if ((thread->data->wakeup - now) > 0) {
            goto next;
        }

        //! keep up to date
        thread->data->wakeup = now;

        if (!(thread->data->state.flags & THREAD_ACTIVE)) {
            goto next;
        }

        if (!best_thread || thread->data->state.priority > best_thread->data->state.priority) {
            best_thread = thread;
        }

        next:

        node = node->nxt;
    }

    return best_thread;
}

static
int scheduler_schedule(core_context **old_ctx,
                       core_context **new_ctx) {
    if (!old_ctx || !new_ctx) {
        return -1;
    }

    const thread_ctx *prv_ctx = sched_ctx.last_thread;
    const thread_ctx *nxt_ctx = get_best_thread();

    if (prv_ctx) {
        prv_ctx->data->state.flags &= ~THREAD_RUNNING;
        *old_ctx = &prv_ctx->data->context;
    }

    if (nxt_ctx) {
        sched_ctx.last_thread = nxt_ctx;

        nxt_ctx->data->wakeup = clock_get();
        nxt_ctx->data->state.flags |=  THREAD_RUNNING;
        *new_ctx = &nxt_ctx->data->context;
    } else {
        //! assuming idler will be scheduled next
        sched_ctx.last_thread = NULL;
    }

    return !nxt_ctx;
}

static
int scheduler_add_thread(thread_ctx *thread) {
    if (!thread) {
        return -1;
    }

    if (!sched_ctx.threads->insert_after(sched_ctx.threads, NULL,
        thread)) {
        return -2;
    }

    return 0;
}

static
void scheduler_destroy(void) {
    sched_ctx.threads->destroy(sched_ctx.threads,
        deallocate_thread_ctx);

    scheduler = NULL;
}

int scheduler_init(void) {
    sched_ctx.threads = list_create();
    if (!sched_ctx.threads) {
        return -1;
    }

    foreach_srv (srv) {
        fill_stack_watermark(srv->data->context.sp_base, srv->data->context.stack_size -
            (sizeof(hw_context_frame) / CORE_STACK_ALIGNMENT));
    }

    sched_ctx.last_thread = NULL;

    scheduler = &sched_ctx.ifc;

    return 0;
}

int thread_stack_used(const thread_ctx *ctx) {
    if (!ctx) {
        return -1;
    }

    int stack_free = core_context_stack_unused(&ctx->data->context, THREAD_DEFAULT_STACK_SIZE);

    return (stack_free > 0) ? (THREAD_DEFAULT_STACK_SIZE - stack_free) : stack_free;
}

int thread_stack_size(const thread_ctx *ctx) {
    if (!ctx) {
        return -1;
    }

    return ctx->data->context.stack_size;
}

void thread_destroy(const thread_ctx *ctx) {
    if (!ctx || !(ctx->data->state.flags & THREAD_SERVICE)) {
        return;
    }

    ctx->data->state.flags &= ~THREAD_ALIVE;
}

const
void *thread_create(void (*task)(), const char *name, uint8_t priority) {
    if (!task || !name || !priority) {
        return NULL;
    }

    if (strnlen(name, THREAD_MAX_NAME_LENGTH) >= THREAD_MAX_NAME_LENGTH) {
        return NULL;
    }

    thread_ctx *thread = cell_alloc(sizeof(thread_ctx));
    if (!thread) {
        return NULL;
    }

    thread->data = cell_alloc(sizeof(thread_data));
    if (!thread->data) {
        cell_free(thread);
        return NULL;
    }

    memset(thread->data, 0, sizeof(thread_data));

    int ret = core_context_init(&thread->data->context, THREAD_DEFAULT_STACK_SIZE, task);
    if (ret) {
        cell_free(thread->data);
        cell_free(thread);
        return NULL;
    }

    thread->data->wakeup = clock_get();

    thread->data->state.flags    = (THREAD_ACTIVE | THREAD_ALIVE);

    thread->data->state.priority = priority;

    thread->name = strdup(name);
    if (!thread->name) {
        core_context_deinit(&thread->data->context);
        cell_free(thread->data);
        cell_free(thread);
    }

    ret = scheduler->add_thread(thread);
    if (ret) {
        cell_free((void*) thread->name);
        core_context_deinit(&thread->data->context);
        cell_free(thread->data);
        cell_free(thread);
        return NULL;
    }

    return thread;
}

void thread_delay(int32_t msec) {
    const thread_ctx *thread = thread_get(NULL);
    if (thread) {
        thread->data->wakeup = clock_get() + msec;
    }
}

void thread_yield(void) {
    thread_delay(0);
}

void thread_suspend(const thread_ctx *thread, const void *wait_for) {
    thread->data->state.flags &= ~THREAD_ACTIVE;

    thread->data->waiting = wait_for ? wait_for : thread;
}

void thread_signal(const void *object) {
    int now = clock_get();

    foreach_srv (srv) {
        if (srv->data->waiting == object) {
            srv->data->state.flags |= THREAD_ACTIVE;

            srv->data->waiting = NULL;

            srv->data->wakeup = now;
        }
    }

    const list_node *node = sched_ctx.threads->get_front(sched_ctx.threads);
    while (node) {
        thread_ctx *ctx = node->ptr;

        if (ctx->data->waiting == object) {
            ctx->data->state.flags |= THREAD_ACTIVE;

            ctx->data->waiting = NULL;

            ctx->data->wakeup = now;
        }

        node = node->nxt;
    }
}

const
thread_ctx *thread_get(const char *name) {
    if (name) {
        if (strnlen(name, THREAD_MAX_NAME_LENGTH) >= THREAD_MAX_NAME_LENGTH) {
            return NULL;
        }
    } else {
        return sched_ctx.last_thread;
    }

    foreach_srv (srv) {
        if (strcmp(srv->name, name) == 0) {
            return srv;
        }
    }

    if (scheduler) {
        const list_node *node = sched_ctx.threads->get_front(sched_ctx.threads);

        while (node) {
            thread_ctx *thread = node->ptr;

            if (strcmp(thread->name, name) == 0) {
                return thread;
            }

            node = node->nxt;
        }
    }

    return NULL;
}

int is_thread(const void *thread) {
    const list_node *node = sched_ctx.threads->get_front(sched_ctx.threads);

    while (node) {
        thread_ctx *ctx = node->ptr;

        if (thread == ctx) {
            return 1;
        }

        node = node->nxt;
    }

    foreach_srv (srv) {
        if (thread == srv) {
            return 1;
        }
    }

    return 0;
}

list_ifc *thread_list_get(void) {
    return sched_ctx.threads;
}
