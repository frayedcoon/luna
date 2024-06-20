#ifndef KERNEL_THREAD_H
#define KERNEL_THREAD_H

#include "common/common.h"
#include "common/utils.h"
#include "arch/context.h"
#include "lib/list.h"

#define THREAD_MAX_NAME_LENGTH 32

typedef enum thread_priority_t {
    PRIORITY_LOWEST  = 0x01,
    PRIORITY_HIGHEST = 0xff,
} thread_priority;

typedef enum thread_flags_t {
    THREAD_RUNNING          = (1 << 0),
    THREAD_ACTIVE           = (1 << 1),
    THREAD_ALIVE            = (1 << 2),
    THREAD_SERVICE          = (1 << 3),
} thread_flags;

typedef struct thread_state_t {
    uint8_t       flags;
    uint8_t       priority;
} __attribute__((aligned(4))) thread_state;

typedef struct thread_data_t {
    core_context          context;

    int                   wakeup;

    const void           *waiting;

    thread_state          state;
} __attribute__((aligned(4))) thread_data;

typedef struct thread_ctx_t {
    const char           *name;

    thread_data          *data;
} __attribute__((aligned(4))) thread_ctx;

extern thread_ctx _srv_start;
extern thread_ctx _srv_end;

#define foreach_srv(_srv) for(const thread_ctx *_srv = &_srv_start; _srv < &_srv_end; _srv++)

#define KERNEL_SRV(_name, _worker, _stack_size, _priority) \
CORE_CTX(_name, _stack_size, _worker); \
static thread_data srv_data_##_name = { \
    .context = CORE_CTX_INIT(_name, _stack_size), \
    .wakeup = 0, \
    .waiting = NULL, \
    .state = { \
        .priority = _priority, \
        .flags = THREAD_ALIVE | THREAD_ACTIVE | THREAD_SERVICE, \
    }, \
}; \
static const thread_ctx srv_##_name __attribute__((used,section(".kernel.srv."#_name))) = { \
    .name = #_name, \
    .data = &srv_data_##_name, \
}

typedef struct scheduler_ifc_t {
    int            (*schedule)   (core_context **old_ctx,
                                  core_context **new_ctx);
    int            (*add_thread) (thread_ctx *thread);
    void           (*destroy)    (void);
} __attribute__((aligned(4))) scheduler_ifc;

int scheduler_init(void);

const
void *thread_create(void (*task)(), const char *name, uint8_t priority);

void thread_delay(int32_t msec);

void thread_yield(void);

void thread_suspend(const thread_ctx *thread, const void *wait_for);

void thread_signal(const void *object);

const
thread_ctx *thread_get(const char *name);

int is_thread(const void *thread);

list_ifc *thread_list_get(void);

int thread_stack_used(const thread_ctx *ctx);

int thread_stack_size(const thread_ctx *ctx);

void thread_destroy(const thread_ctx *ctx);

#endif
