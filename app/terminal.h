#ifndef APP_TERMINAL_H
#define APP_TERMINAL_H

#include "lib/string.h"
#include "platform/usart.h"
#include "kernel/thread.h"
#include "lib/list.h"

/**
 * @brief      sctuct with command-related stuff
 */
typedef const struct terminal_command_context_t {
    /**
     * pointer to funtion command represents
     */
    int (*terminal_cmd_handler)(list_ifc *args);
    /**
     * command string to execute function
     */
    const char *terminal_command_string;
} terminal_command_context;

#define TERMINAL_CMD(_name, _handler) \
static terminal_command_context cmd_##_name __attribute__((used,section(".terminal.command."#_name))) = {\
    .terminal_command_string = #_name,\
    .terminal_cmd_handler = _handler,\
}

/**
 * @brief      initialization of usart/uart used by terminal i/o
 */
void terminal_init(void);

#endif
