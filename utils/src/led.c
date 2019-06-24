#include "app/terminal.h"
#include "lib/string.h"
#include "kernel/syscall.h"
#include "common/sys.h"
#include "common/log.h"

#define LED_CMD_ENABLE  "enable"
#define LED_CMD_DISABLE "disable"

#define IO_BUF_SIZE 16

static int led_cmd_handler(list_ifc *args) {
    if (args->size(args) != 2) {
        return -1;
    }

    const list_node *node = args->get_front(args)->nxt;

    uint8_t io_buffer[IO_BUF_SIZE];

    const void *conn =  connect(LED_PORT, io_buffer, IO_BUF_SIZE);
    if (!conn) {
        LOG_ERR("Failed to connect to led socket\n");
        return -1;
    }

    char *str = NULL;

    if (strcmp((char*) node->ptr, LED_CMD_ENABLE) == 0) {
        str = "1";
    } else if (strcmp((char*) node->ptr, LED_CMD_DISABLE) == 0) {
        str = "0";
    }

    int ret = 0;

    if (str) {
        ret = write(conn, str, strsize(str));
        if (ret != strsize(str)) {
            LOG_ERR("Error writing socket: %d", ret);
            goto exit;
        }

        int32_t reply;

        ret = timed_read(conn, (char*)&reply, sizeof(int32_t), 500);
        if (ret != sizeof(int32_t)) {
            LOG_ERR("Error reading socket: %d", ret);
            goto exit;
        }

        if (reply != 0) {
            LOG_ERR("Error reply received: %d", reply);
            goto exit;
        }

        ret = 0;
    }

    exit:

    close(conn);
    return ret;
}

TERMINAL_CMD(led, led_cmd_handler);
