#ifndef ARCH_CONTEXT_H
#define ARCH_CONTEXT_H

#include "common/common.h"

#define MINIMAL_CORE_STACK_SIZE   (sizeof(hw_context_frame) / 4)

#define CORE_STACK_ALIGNMENT      (sizeof(uint32_t))

#define PSR_DEFAULT               (0x01000000)

#define CORE_CTX(_name, _stack_size, _worker) \
static uint32_t stack_##_name[_stack_size] = { \
    [_stack_size - 1] = PSR_DEFAULT, \
    [_stack_size - 2] = (uint32_t) _worker, \
    [_stack_size - 3] = (uint32_t) idler_task, \
}; \
static sw_context_frame sw_frame_##_name = {0};

#define CORE_CTX_INIT(_name, _stack_size) { \
    .sp = &stack_##_name[_stack_size - (sizeof(hw_context_frame) / CORE_STACK_ALIGNMENT)], \
    .sp_base = stack_##_name, \
    .sw_frame = (uint32_t *)&sw_frame_##_name, \
    .stack_size = _stack_size, \
}

/**
 * core context to be stored in thread's header
 */
typedef struct core_context_t {
    /**
     * saved stack pointer
     */
    void     *sp;
    /**
     * base of allocated stack space
     */
    void     *sp_base;
    /**
     * pointer to saved software context
     */
    uint32_t *sw_frame;
    /**
     * @brief allocated stack_size
     */
    uint32_t stack_size;
} __attribute__((aligned(4))) core_context;

/**
 * stores registers saved by hardware
 */
typedef struct hw_context_frame_t {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
} __attribute__((aligned(4))) hw_context_frame;

/**
 * stores registers saved by software
 */
typedef struct sw_context_frame_t {
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
} __attribute__((aligned(4))) sw_context_frame;

/**
 * @brief      doing nothing...nuff said
 */
void idler_task(void);

void fill_stack_watermark(uint32_t *stack, uint32_t stack_size);

/**
 * @brief      initializes core context
 *
 * @param      ctx         core context ptr
 * @param[in]  stack_size  stack size
 * @param[in]  task        context task ptr
 *
 * @return     error code
 */
int core_context_init(core_context *ctx, uint32_t stack_size, void (*task)());

/**
 * @brief check unused stack space
 *
 * @param ctx        core context ptr
 * @param stack_size stack max size
 *
 * @return unused stack in alignment units
 */
int core_context_stack_unused(core_context *ctx, uint32_t stack_size);

/**
 * @brief      deinitializes core context
 *
 * @param      ctx   core context ptr
 */
void core_context_deinit(core_context *ctx);

#endif
