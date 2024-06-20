#include "app/terminal.h"
#include "kernel/syscall.h"
#include "lib/string.h"

static int sleep_cmd_handler(list_ifc *args) {
    int size = args->size(args);
    if (size == 2) {
        const list_node *node = args->get_front(args);
        int seconds = atoi((char*) node->nxt->ptr);
        if (seconds > 0) {
            sleep(seconds);
            return 0;
        }
    }

    return -1;
}

TERMINAL_CMD(sleep, sleep_cmd_handler);
