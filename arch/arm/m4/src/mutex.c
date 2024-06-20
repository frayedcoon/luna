#include "arch/mutex.h"

__attribute__((naked))
void arch_mutex_lock(const void *mutex_handle, mutex_hook wait, const void *mutex_var) {
    asm volatile(
        "mov r3, #1                          \n\t"
        "try_lock:                           \n\t"
        "ldrex r4, [r2]                      \n\t"
        "cmp r4, r3                          \n\t"
        "beq wait_label                      \n\t"
        "itte ne                             \n\t"
        "strexne r4, r3, [r2]                \n\t"
        "cmpne r4, #1                        \n\t"
        "beq try_lock                        \n\t"
        "dmb                                 \n\t"
        "bx lr                               \n\t"

        //! wait either using `wait` function or immediately try again
        "wait_label:                         \n\t"
        "cmp r1, #0                          \n\t"
        "push {r0, r1, r2, r3, r4, lr}       \n\t"
        "it ne                               \n\t"
        "blxne r1                            \n\t"
        "pop {r0, r1, r2, r3, r4, lr}        \n\t"
        "b try_lock                          \n\t"
        : [mutex_var] "=r" (mutex_var)
        : [wait] "r" (wait),
          [mutex_handle] "r" (mutex_handle)
    );
}



__attribute__((naked))
void arch_mutex_unlock(const void *mutex_handle, mutex_hook signal, const void *mutex_var) {
    asm volatile(
        "mov r3, #0                          \n\t"
        "dmb                                 \n\t"
        "str r3, [r0]                        \n\t"
        "cmp r1, #0                          \n\t"
        "push {r0, r1, r2, r3, r4, lr}       \n\t"
        "it ne                               \n\t"
        "blxne r1                            \n\t"
        "pop {r0, r1, r2, r3, r4, pc}        \n\t"
        : [mutex_var] "=r" (mutex_var)
        : [signal] "r" (signal),
          [mutex_handle] "r" (mutex_handle)
    );
}
