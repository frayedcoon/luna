#include "app/terminal.h"
#include "srv/vfs.h"
#include "common/sys.h"
#include "common/log.h"

#define IO_BUF_SIZE 64

static int ls_cmd_handler(list_ifc *args) {
    if (args->size(args) != 1) {
        return -1;
    }

    const char *curr_path = strdup(fs_current_path_get());
    if (!curr_path) {
        return -2;
    }

    char path_buf[VFS_BUF_MAX];

    int fd = fopen(curr_path);
    if (fd > 0) {
        file_stat stat;
        int ret = fstat(fd, &stat);
        if (!ret) {
            if (stat.type == F_TYPE_DIR) {
                ret = fread(fd, path_buf, VFS_BUF_MAX - 1);

                uint32_t displacement = 0;
                while (ret > 0) {
                    dir_entry *entry = (void *) (path_buf + displacement);
                    printf("%s%s\n", entry->name, entry->type == F_TYPE_DIR ? "/" : "");

                    displacement += sizeof(dir_entry) + strsize(entry->name);

                    ret -= sizeof(dir_entry) + strsize(entry->name);
                }
                fclose(fd);
            } else {
                printf("Not a dir\n");
            }
        } else {
            LOG_ERR("Failed to read file data");
        }

    } else {
        printf("Not found\n");
    }

    return 0;
}

TERMINAL_CMD(ls, ls_cmd_handler);
