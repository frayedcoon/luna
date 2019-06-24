#include "app/terminal.h"
#include "kernel/thread.h"
#include "lib/string.h"
#include "common/log.h"
#include "common/sys.h"
#include "kernel/syscall.h"
#include "kernel/mutex.h"
#include "platform/clock.h"

static mutex m;

static void foo(void) {
    while (1) {
        int before = clock_get();
        mutex_lock(&m);
        printf("foo %d\n", clock_get() - before);
        sleep(777);
        mutex_unlock(&m);
        sleep(777);
    }
}

static void bar(void) {
    while (1) {
        int32_t before = clock_get();
        mutex_lock(&m);
        printf("bar %d\n", clock_get() - before);
        sleep(130);
        mutex_unlock(&m);
        sleep(130);
    }
}

static void baz(void) {
    while (1) {
        int32_t before = clock_get();
        mutex_lock(&m);
        printf("baz %d\n", clock_get() - before);
        sleep(420);
        mutex_unlock(&m);
        sleep(420);
    }
}

static int help_cmd_handler(list_ifc *args) {
    (void) args;

    create_thread(foo, "foo", 7);
    create_thread(bar, "bar", 7);
    create_thread(baz, "baz", 7);

    return 0;
}

TERMINAL_CMD(help, help_cmd_handler);
