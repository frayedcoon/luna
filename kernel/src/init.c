#include "arch/context.h"
#include "arch/svc.h"
#include "arch/systick.h"
#include "platform/clock.h"
#include "kernel/init.h"
#include "kernel/memory.h"
#include "kernel/thread.h"
#include "app/terminal.h"
#include "common/log.h"

/**
 * @brief      basic threads initialization and user mode switch
 */
static
int init_threading(void) {
    int ret = scheduler_init();
    if (ret) {
        return ret;
    }

    //! enter the user mode
    sv_call(SVC_USER_MODE, NULL);

    //! unreachable
    return 0;
}

void init_subsystems(void) {
    systick_init();
    clock_init();
    heap_init();
    gpio_init();
    terminal_init();

    int ret = init_threading();
    if (ret) {
        LOG_ERR("Failed to init threading");
    }

    while (1);
}
