#include "app/terminal.h"
#include "kernel/thread.h"
#include "lib/string.h"
#include "common/log.h"
#include "common/sys.h"
#include "kernel/syscall.h"
#include "srv/vfs.h"

#define IO_BUF_SIZE 32

static int mkdir_cmd_handler(list_ifc *args) {
    if (args->size(args) != 2) {
        return -1;
    }

    // const list_node *arg = args->get_back(args);

    // int ret = 0;

    // uint8_t io_buffer[IO_BUF_SIZE];

    // const void *conn = connect(VFS_PORT, io_buffer, IO_BUF_SIZE);
    // if (!conn) {
    //     LOG_ERR("failed to connect to vfs");
    //     return -1;
    // }

    // char buf[IO_BUF_SIZE];

    // int len = snprintf(buf, IO_BUF_SIZE - 1, "%s %s",
    //     VFS_MKDIR_REQ, arg->ptr);
    // if (len < 0 || len > IO_BUF_SIZE - 1) {
    //     LOG_ERR("failed to request vfs");
    //     goto closure;
    // }

    // ret = write(conn, buf, len + 1);
    // if (ret < 0) {
    //     LOG_ERR("failed to request vfs");
    //     goto closure;
    // }

    // ret = timed_read(conn, buf, IO_BUF_SIZE - 1, 100);
    // if (ret <= 0) {
    //     LOG_ERR("failed to read vfs reply");
    //     goto closure;
    // }

    // printf("%s\n", buf);

    // closure:
    // ret = close(conn);
    // if (ret) {
    //     LOG_ERR("failed to close vfs connection: %d", ret);
    // }

    return 0;
}

TERMINAL_CMD(mkdir, mkdir_cmd_handler);
