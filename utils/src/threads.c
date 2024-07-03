#include "app/terminal.h"
#include "lib/string.h"
#include "kernel/thread.h"
#include "lib/syscall.h"
#include "lib/list.h"
#include "platform/clock.h"

static int threads_cmd_handler(list_ifc *args) {
    int size = args->size(args);
    if (size == 1) {
        foreach_srv(srv) {
            uint8_t flags = srv->data->state.flags;

            printf("%c|%c|%c|%c|0x%x/0x%x|%s %dms\n",
                (flags & THREAD_RUNNING) ? 'R' : '.',
                (flags & THREAD_ACTIVE) ? 'A' : '.',
                (flags & THREAD_ALIVE) ? 'L' : '.',
                (flags & THREAD_SERVICE) ? 'S' : '.',
                thread_stack_used(srv),
                thread_stack_size(srv),
                srv->name,
                srv->data->wakeup - clock_get());
        }

        list_ifc *list = thread_list_get();

        const list_node *node = list->get_front(list);

        while (node) {
            thread_ctx *thread = node->ptr;

            uint8_t flags = thread->data->state.flags;

            printf("%c|%c|%c|%c|0x%x/0x%x|%s %dms\n",
                (flags & THREAD_RUNNING) ? 'R' : '.',
                (flags & THREAD_ACTIVE) ? 'A' : '.',
                (flags & THREAD_ALIVE) ? 'L' : '.',
                (flags & THREAD_SERVICE) ? 'S' : '.',
                thread_stack_used(thread),
                thread_stack_size(thread),
                thread->name,
                thread->data->wakeup - clock_get());

            node = node->nxt;
        }

        return 0;
    }

    return -1;
}

TERMINAL_CMD(threads, threads_cmd_handler);
