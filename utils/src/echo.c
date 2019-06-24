#include "app/terminal.h"
#include "lib/string.h"

static int echo_cmd_handler(list_ifc *args) {
    int size = args->size(args);
    if (size != 2) {
        return -1;
    }

    const list_node *node = args->get_front(args);

    printf("%s\n", node->nxt->ptr);

    return 0;
}

TERMINAL_CMD(echo, echo_cmd_handler);
