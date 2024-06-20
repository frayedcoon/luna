#include "app/terminal.h"
#include "srv/vfs.h"
#include "common/sys.h"
#include "common/log.h"
#include "fs/fs.h"

static int cd_cmd_handler(list_ifc *args) {
    if (args->size(args) != 2) {
        return -1;
    }

    char *path = args->get_back(args)->ptr;
    if (strsize(path) > VFS_BUF_MAX - strlen(fs_current_path_get())) {
        return -2;
    }

    char path_buf[VFS_BUF_MAX];
    snprintf(path_buf, VFS_BUF_MAX, "%s/%s", fs_current_path_get(), path);

    normalize_path(path_buf);

    int fd = fopen(path_buf);
    if (fd <= 0) {
        printf("Not found.\n");
    } else {
        file_stat stat;
        int ret = fstat(fd, &stat);
        if (!ret && stat.type == F_TYPE_DIR) {
            fs_current_path_set(path_buf);
        } else {
            printf("Not a dir.\n");
        }
        fclose(fd);
    }

    return 0;
}

TERMINAL_CMD(cd, cd_cmd_handler);
