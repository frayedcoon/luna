#include "app/terminal.h"
#include "target/cfg.h"
#include "common/log.h"
#include "kernel/syscall.h"
#include "lib/parser.h"
#include "fs/fs.h"

#define TERMINAL_INPUT_BUFFER_SIZE  0x40

#define PROMPT_PREFIX  "luna "
#define PROMPT_POSTFIX ">"

#define foreach_cmd(_cmd) for(terminal_command_context *_cmd = &_terminal_start; _cmd < &_terminal_end; _cmd++)

extern terminal_command_context _terminal_start;
extern terminal_command_context _terminal_end;

static const usart_config stdio_config = {
    .baud_rate   = DEFAULT_BAUD_RATE,
    .buffer_size = TERMINAL_INPUT_BUFFER_SIZE,
    .port        = USART2_PORT,
    .tx_pin      = USART2_TX_PIN,
    .rx_pin      = USART2_RX_PIN,
};

static usart_iface *stdio = NULL;

/**
 * @brief      prepare terminal before input
 */
static void terminal_new_cmd(void) {
    stdio->puts(stdio, PROMPT_PREFIX);
    stdio->puts(stdio, fs_current_path_get());
    stdio->puts(stdio, PROMPT_POSTFIX);
}

/**
 * @brief      command-line parser/executor
 *
 * @param[in]  buffer  command buffer to parse
 *
 * @return     succes/failure of execution
 */
static int terminal_process_command(const char *buffer) {
    //! error flag
    int ret = -1;
    //! create arguments list
    list_ifc *command = parse_buffer(buffer, ' ', 1);
    if (!command) {
        return ret;
    }

    const list_node *arg = command->get_front(command);

    foreach_cmd(cmd) {
        if (strcmp(arg->ptr, cmd->terminal_command_string) == 0) {

            ret = cmd->terminal_cmd_handler(command);

            break;
        }
    }

    //! free memory allocated for argument list
    command->destroy(command, free);
    return ret;
}

void terminal_init(void) {
    //! initialize STDIO
    usart_init(STDIO, &stdio_config);
    stdio = usart_iface_get(STDIO);
}

static void terminal_loop(void) {
    stdio->puts(stdio, "\n");
    terminal_new_cmd();
    while (1) {
        const char *cmd_buffer = stdio->buffer_drain(stdio);
        if (strlen(cmd_buffer) > 0 &&
            terminal_process_command(cmd_buffer) != 0) {
            LOG_ERR("Invalid command/arguments. "
                    "Type \"help\" for list of available commands.");
        }
        terminal_new_cmd();
        stdio->set_buffer_locked(stdio, 0);
    }
}

KERNEL_SRV(terminal, terminal_loop, 512, 1);
