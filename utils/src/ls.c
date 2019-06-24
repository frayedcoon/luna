#include "app/terminal.h"
#include "srv/vfs.h"
#include "common/sys.h"
#include "common/log.h"

#define IO_BUF_SIZE 64

static int ls_cmd_handler(list_ifc *args) {
    if (args->size(args) != 1) {
        return -1;
    }

    int ret = 0;

    uint8_t io_buffer[IO_BUF_SIZE];

    const void *conn = connect(VFS_PORT, io_buffer, IO_BUF_SIZE);
    if (!conn) {
        LOG_ERR("failed to connect to vfs");
        return -1;
    }

    char buf[IO_BUF_SIZE];

    ret = write(conn, VFS_LS_REQ, strsize(VFS_LS_REQ));
    if (ret < 0) {
        LOG_ERR("failed to request vfs: %d", ret);
        goto closure;
    }

    ret = timed_read(conn, buf, IO_BUF_SIZE - 1, 100);
    if (ret <= 0) {
        LOG_ERR("failed to read vfs reply: %d", ret);
        goto closure;
    }

    printf("%s\n", buf);

    closure:
    ret = close(conn);
    if (ret) {
        LOG_ERR("failed to close vfs connection: %d", ret);
    }

    return 0;
}

TERMINAL_CMD(ls, ls_cmd_handler);
