#include "app/terminal.h"
#include "srv/vfs.h"
#include "common/sys.h"
#include "common/log.h"
#include "fs/fs.h"

static int cat_cmd_handler(list_ifc *args) {
    if (args->size(args) != 2) {
        return -1;
    }

    char *path = args->get_back(args)->ptr;
    if (strsize(path) > VFS_BUF_MAX - strlen(fs_current_path_get())) {
        return -2;
    }

    char path_buf[VFS_BUF_MAX + 1];
    snprintf(path_buf, VFS_BUF_MAX, "%s/%s", fs_current_path_get(), path);

    normalize_path(path_buf);

    int fd = fopen(path_buf);
    if (fd > 0) {
        file_stat stat;
        int ret = fstat(fd, &stat);
        if (!ret) {
            if (stat.type == F_TYPE_REG) {
                do {
                    ret = fread(fd, path_buf, VFS_BUF_MAX);
                    if (ret > 0) {
                        path_buf[ret] = 0;
                        printf("%s", path_buf);
                    }
                } while (ret > 0);
                printf("\n");
                fclose(fd);
            } else {
                printf("Not a regular file\n");
            }
        } else {
            LOG_ERR("Failed to read file data");
        }

    } else {
        printf("Not found\n");
    }

    return 0;
}

TERMINAL_CMD(cat, cat_cmd_handler);
