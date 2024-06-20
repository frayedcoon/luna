#include "arch/context.h"
#include "common/def.h"
#include "kernel/memory.h"
#include "common/log.h"

#define STACK_MAGIC                    (0xDEADBEEF)

static
uint32_t      idler_stack[sizeof(hw_context_frame) / CORE_STACK_ALIGNMENT] =
              {0, 0, 0, 0,
               0, (uint32_t) idler_task, (uint32_t) idler_task, PSR_DEFAULT};

static
uint32_t      idler_frame[sizeof(sw_context_frame) / CORE_STACK_ALIGNMENT] =
              {0, 0, 0, 0,
               0, 0, 0, 0};

core_context  idler_core_context = {
    .sp           = &idler_stack[0],
    .sp_base      = &idler_stack[0],
    .sw_frame     = &idler_frame[0],
};

static
uint32_t      dummy_frame[sizeof(sw_context_frame) / CORE_STACK_ALIGNMENT] =
              {0, 0, 0, 0,
               0, 0, 0, 0};

core_context dummy_core_context = {
    .sp           = NULL,
    .sp_base      = NULL,
    .sw_frame     = &dummy_frame[0],
};

core_context *core_context_cur                          = &idler_core_context;
core_context *core_context_new                          = &idler_core_context;

void idler_task(void) {
    while (1) {
        asm volatile("wfi");
    }
}

void fill_stack_watermark(uint32_t *stack, uint32_t stack_size) {
    for (uint32_t *magic = stack; magic < stack + stack_size; magic++) {
        *magic = STACK_MAGIC;
    }
}

int core_context_init(core_context *ctx, uint32_t stack_size, void (*task)()) {
    if (!ctx || stack_size < MINIMAL_CORE_STACK_SIZE) {
        return -1;
    }

    ctx->sp_base = cell_alloc(stack_size * CORE_STACK_ALIGNMENT);
    if (!ctx->sp_base) {
        return -1;
    }

    ctx->sw_frame = cell_alloc(sizeof(sw_context_frame));
    if (!ctx->sw_frame) {
        cell_free(ctx->sp_base);
        return -1;
    }

    uint32_t *stack = (uint32_t*) ctx->sp_base;

    fill_stack_watermark(stack, stack_size);

    stack[stack_size - 1] = PSR_DEFAULT;
    stack[stack_size - 2] = (uint32_t) task;
    stack[stack_size - 3] = (uint32_t) idler_task;

    ctx->sp = &stack[stack_size -
              (sizeof(hw_context_frame) / CORE_STACK_ALIGNMENT)];

    ctx->stack_size = stack_size;

    return 0;
}

int core_context_stack_unused(core_context *ctx, uint32_t stack_size) {
    if (!ctx || stack_size < MINIMAL_CORE_STACK_SIZE) {
        return -1;
    }

    uint32_t *stack = (uint32_t*) ctx->sp_base;
    for (uint32_t *magic = stack; magic < stack + stack_size; magic++) {
        if (*magic != STACK_MAGIC) {
            return (magic - stack);
        }
    }

    return 0;
}

void core_context_deinit(core_context *ctx) {
    if (!ctx) {
        return;
    }

    cell_free(ctx->sp_base);
    cell_free(ctx->sw_frame);
}
