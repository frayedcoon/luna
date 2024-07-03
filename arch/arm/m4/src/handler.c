#include "arch/handler.h"
#include "arch/core.h"
#include "arch/svc.h"
#include "arch/context.h"

#include "platform/usart.h"

#include "lib/string.h"

#include "kernel/thread.h"

volatile int32_t     systick_msec = 0;

extern void sv_schedule_routine(void*);

extern core_context  *core_context_cur;
extern core_context  *core_context_new;

extern int context_switch_scheduled;

__attribute__((used))
static void print_fault(uint32_t * stack, uint32_t exc) {
    printf("CPU Hard Fault.\n");
    printf("EXC_RETURN     : 0x%08x\n", exc);
    printf("SCB->HFSR      : 0x%08x\n", HFSR);
    printf("SCB->CFSR      : 0x%08x\n", CFSR);
    printf("Register Dump  :\n");
    printf("r0  = 0x%08x\n", stack[0]);
    printf("r1  = 0x%08x\n", stack[1]);
    printf("r2  = 0x%08x\n", stack[2]);
    printf("r3  = 0x%08x\n", stack[3]);
    printf("r12 = 0x%08x\n", stack[4]);
    printf("lr  = 0x%08x\n", stack[5]);
    printf("pc  = 0x%08x\n", stack[6]);
    printf("psr = 0x%08x\n", stack[7]);

    foreach_srv(srv) {
        uint8_t flags = srv->data->state.flags;

        printf("%c|%c|%c|%c|0x%x/0x%x|%s\n",
            (flags & THREAD_RUNNING) ? 'R' : '.',
            (flags & THREAD_ACTIVE) ? 'A' : '.',
            (flags & THREAD_ALIVE) ? 'L' : '.',
            (flags & THREAD_SERVICE) ? 'S' : '.',
            thread_stack_used(srv),
            thread_stack_size(srv),
            srv->name);
    }

    list_ifc *list = thread_list_get();

    const list_node *node = list->get_front(list);

    while (node) {
        thread_ctx *thread = node->ptr;

        uint8_t flags = thread->data->state.flags;

        printf("%c|%c|%c|%c|0x%x/0x%x|%s\n",
            (flags & THREAD_RUNNING) ? 'R' : '.',
            (flags & THREAD_ACTIVE) ? 'A' : '.',
            (flags & THREAD_ALIVE) ? 'L' : '.',
            (flags & THREAD_SERVICE) ? 'S' : '.',
            thread_stack_used(thread),
            thread_stack_size(thread),
            thread->name);

        node = node->nxt;
    }

    asm volatile("bkpt #00\n\t");
    while (1);
}

void systick_handler(void) {
    sv_schedule_routine(NULL);

    systick_msec++;
}

void pend_sv_handler(void) {
    register uint32_t *old_context asm ("r0") =
        core_context_cur->sw_frame;
    register void *old_stack asm ("r1") =
        core_context_cur->sp;
    register int *need_switch asm ("r2") = &context_switch_scheduled;

    asm volatile(
        "mov r3, #0                          \n\t"
        "str r3, [r2]                        \n\t"
        //! store current sofware context
        "stm    r0!, {r4 - r11}              \n\t"
        //! store current stack pointer
        "mrs    r1, psp                      \n\t"
    );

    core_context_cur->sp = (void*) old_stack;

    (void) old_context;
    (void) old_stack;
    (void) need_switch;

    register uint32_t *new_context asm ("r0") =
        core_context_new->sw_frame;
    register void *new_stack asm ("r1") =
        core_context_new->sp;

    asm volatile(
        //! restore scheduled sofware context
        "ldm      r0!, {r4 - r11}            \n\t"
        //! restore scheduled stack pointer
        "msr      psp, r1                    \n\t"
    );

    (void) new_context;
    (void) new_stack;
}


void default_handler(void) {
    printf("unhandled interrupt!!!");
    while (1);
}

void h_fault_handler(void) {
    asm volatile(
        "tst    lr, #0b0100          \n\t"
        "ite    eq                   \n\t"
        "mrseq  r0, msp              \n\t"
        "mrsne  r0, psp              \n\t"
        "mov    r1, lr               \n\t"
        "b      print_fault          \n\t"
    );
}
