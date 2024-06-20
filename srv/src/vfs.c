#include "srv/vfs.h"
#include "common/log.h"
#include "common/sys.h"
#include "lib/parser.h"
#include "kernel/socket.h"
#include "platform/clock.h"
#include "fs/fs.h"
#include "fs/mbr.h"
#include "fs/ext2.h"
#include "platform/spi.h"
#include "driver/sd_spi.h"
#include "app/terminal.h"

#define VFS_CONNECTIONS_MAX        0x08

#define VFS_SOCK_BUFFER_SIZE       (sizeof(vfs_command) + VFS_BUF_MAX)

#define VFS_REPL_BUFFER_SIZE       (sizeof(vfs_reply) + VFS_BUF_MAX)

typedef int (*vfs_handler)(const void *conn, vfs_command *command, file_desc *root, fs_ifc *fs);

typedef struct vfs_fd_t {
    int        fd;
    file_desc *file;
    uint32_t   fpos;
} vfs_fd;

typedef struct vfs_rw_req_t {
    int      fd;
    uint32_t size;
    char     buf[];
} vfs_rw_req;

typedef struct vfs_fd_req_t {
    int fd;
} vfs_fd_req;

static list_ifc *open_fd = NULL;

static int current_fd = 0;

static const spi_config cfg = {
    .prescaler = SPI_PRESCALER_4,
    .mode      = SPI_MODE_MASTER,
    .cpol      = 0,
    .cpha      = 0,

    .port      = SPI2_GPIO_PORT,
    .cs_pin    = SPI2_CS_PIN,
    .sck_pin   = SPI2_CSK_PIN,
    .miso_pin  = SPI2_MISO_PIN,
    .mosi_pin  = SPI2_MOSI_PIN,
};

static int fd_find(int fd) {
    const list_node *node = open_fd->get_front(open_fd);
    while (node) {
        vfs_fd *vfs_fd = node->ptr;
        if (fd == vfs_fd->fd) {
            return fd;
        }

        node = node->nxt;
    }

    return 0;
}

static inline int gen_fd(void) {
    do {
        current_fd++;
        if (current_fd <= 0) {
            current_fd = 1;
        }
    } while (fd_find(current_fd));

    return current_fd;
}

static int vfs_send_reply(const void *conn, int ret_code, const char *reply_buf, uint32_t buf_sz) {
    vfs_reply * reply = malloc(sizeof(vfs_reply) + buf_sz);
    if (!reply) {
        return -1;
    }

    reply->ret = ret_code;

    if (buf_sz) {
        memcpy(reply->buf, reply_buf, buf_sz);
    }

    int ret = 0;
    uint32_t offset = 0;
    char *buf = (void *) reply;
    do {
        ret =  write(conn, buf + offset, sizeof(vfs_reply) + buf_sz - offset);
        offset += ret;
        sleep(1);
    } while (ret >= 0 && offset < sizeof(vfs_reply) + buf_sz);

    free(reply);

    return ret;
}

static const list_node *fd_get(int fd) {
    const list_node *node = open_fd->get_front(open_fd);
    while (node) {
        vfs_fd *vfs_fd = node->ptr;
        if (fd == vfs_fd->fd) {
            return node;
        }

        node = node->nxt;
    }

    return NULL;
}

static const list_node *fd_get_by_desc(file_desc *file) {
    const list_node *node = open_fd->get_front(open_fd);
    while (node) {
        vfs_fd *fd = node->ptr;
        if (file == fd->file) {
            return node;
        }

        node = node->nxt;
    }

    return NULL;
}

static file_desc *resolve_helper(list_ifc *entries, file_desc *root) {
    file_desc *to_find = NULL;

    if (!entries->size(entries)) {
        return root;
    }

    const list_node *entry  = entries->get_front(entries);
    while (entry) {
        const char *entry_str = entry->ptr;

        to_find = NULL;

        const list_node *dirent = root->child->get_front(root->child);
        while (dirent) {
            file_desc *desc = dirent->ptr;

            if (strlen(entry_str) == strlen(desc->name) && !strcmp(desc->name, entry_str)) {
                to_find = desc;

                if (desc->type == F_TYPE_DIR) {
                    if (desc->link) {
                        if (desc->link->child) {
                            entry = entry->nxt;
                            entry_str = entry->ptr;
                            dirent = desc->link->child->get_front(desc->link->child);
                            continue;
                        } else if (entry->nxt) {
                            to_find = NULL;
                            break;
                        }
                    }
                    root = desc;
                } else if (desc->type == F_TYPE_REG) {
                    if (entry->nxt) {
                        //! file should be last
                        dirent = dirent->nxt;
                        to_find = NULL;
                        continue;
                    }
                } else {
                    //! smth wrong
                    goto exit;
                }

                break;
            }

            dirent = dirent->nxt;
        }

        if (!to_find) {
            return NULL;
        }

        entry = entry->nxt;
    }

    exit:

    return to_find;
}

static int vfs_open(const void *conn, vfs_command *command, file_desc *root, fs_ifc *fs) {
    (void) fs;
    file_desc *to_open = NULL;
    int ret = -1;

    list_ifc *entries = parse_buffer(command->buf, '/', 0);
    if (!entries) {
        goto reply;
    }

    to_open = resolve_helper(entries, root);
    if (to_open) {
        if (!fd_get_by_desc(to_open)) {
            vfs_fd *fd = malloc(sizeof(vfs_fd));
            if (fd) {
                fd->fd = gen_fd();
                fd->file = to_open;
                fd->fpos = 0;

                if (!open_fd->insert_after(open_fd, NULL, fd)) {
                    free(fd);
                } else {
                    ret = fd->fd;
                }
            }
        }
    }

    reply:

    if (entries) {
        entries->destroy(entries, free);
    }

    return vfs_send_reply(conn, ret, NULL, 0);
}

static int vfs_stat(const void *conn, vfs_command *command, file_desc *root, fs_ifc *fs) {
    (void) root;
    (void) fs;

    int ret = -1;

    vfs_fd_req *req = (void *) &command->buf;

    file_stat *stat = (void *) (req + 1);

    const list_node *node = fd_get(req->fd);
    if (node) {
        vfs_fd *fd = node->ptr;

        stat->type = fd->file->type;
        stat->size = fd->file->size;

        ret = 0;
    }

    return vfs_send_reply(conn, ret, (char *) stat, sizeof(file_stat));
}

static int vfs_read(const void *conn, vfs_command *command, file_desc *root, fs_ifc *fs) {
    (void) root;

    int ret = -1;

    vfs_rw_req *req = (void *) &command->buf;

    uint8_t read_buf[VFS_BUF_MAX];

    int read = 0;

    uint32_t read_sz = req->size;

    const list_node *node = fd_get(req->fd);
    if (node) {
        vfs_fd *fd = node->ptr;
        read = fs->read(fs, fd->file, read_buf, read_sz, fd->fpos);
        if (read < 0) {
            read = 0;
        } else {
            fd->fpos += fd->file->type == F_TYPE_DIR ? 1 : read;

            ret = read;
        }
    }

    return vfs_send_reply(conn, ret, (char *) read_buf, read);
}

static int vfs_write(const void *conn, vfs_command *command, file_desc *root, fs_ifc *fs) {
    (void) fs;
    (void) command;
    (void) root;
    return vfs_send_reply(conn, -1, NULL, 0);
}

static int vfs_close(const void *conn, vfs_command *command, file_desc *root, fs_ifc *fs) {
    (void) root;
    (void) fs;
    int ret = -1;

    vfs_fd_req *req = (void *) &command->buf;

    const list_node *node = fd_get(req->fd);
    if (node) {
        open_fd->delete(open_fd, node, free);
        ret = 0;
    }

    return vfs_send_reply(conn, ret, NULL, 0);
}

static vfs_handler handlers[] = {
    [VFS_OPEN] = vfs_open,
    [VFS_STAT] = vfs_stat,
    [VFS_READ] = vfs_read,
    [VFS_WRITE] = vfs_write,
    [VFS_CLOSE] = vfs_close,
};

int fstat(int fd, file_stat *stat) {
    if (!stat) {
        return -1;
    }

    char cmd_buf[sizeof(vfs_command) + sizeof(vfs_fd_req)];
    char reply_buf[sizeof(vfs_reply) + sizeof(file_stat)];


    const void *conn = connect(VFS_PORT, sizeof(vfs_reply) + sizeof(file_stat));
    if (!conn) {
        LOG_ERR("failed to connect to vfs");
        return -2;
    }

    vfs_command *cmd   = (void *) cmd_buf;
    vfs_reply   *reply = (void *) reply_buf;

    cmd->command = VFS_STAT;

    vfs_fd_req *req = (void *) (cmd + 1);
    req->fd         = fd;

    int ret = write(conn, cmd_buf, sizeof(vfs_command) + sizeof(vfs_fd_req));
    if (ret < 0) {
        LOG_ERR("failed to request vfs");
        goto closure;
    }

    ret = timed_read(conn, reply_buf, sizeof(vfs_reply) + sizeof(file_stat), 100);
    if (ret <= 0) {
        LOG_ERR("failed to read vfs reply");
        goto closure;
    }

    if (ret == sizeof(vfs_reply) + sizeof(file_stat)) {
        memcpy(stat, reply->buf, sizeof(file_stat));
    }

    closure:
    close(conn);

    return (ret == sizeof(vfs_reply) + sizeof(file_stat)) ? reply->ret : -3;
}

int fopen(const char *path) {
    if (!path || strsize(path) > VFS_BUF_MAX) {
        return -1;
    }

    const void *conn = connect(VFS_PORT, sizeof(vfs_command) + strsize(path));
    if (!conn) {
        LOG_ERR("failed to connect to vfs");
        return -2;
    }

    char cmd_buf[sizeof(vfs_command) + strsize(path)];
    char reply_buf[sizeof(vfs_reply) + sizeof(int)];

    vfs_command *cmd   = (void *) cmd_buf;
    vfs_reply   *reply = (void *) reply_buf;

    cmd->command = VFS_OPEN;

    strcpy(cmd->buf, path);

    int ret = write(conn, cmd_buf, sizeof(vfs_command) + strsize(path));
    if (ret < 0) {
        LOG_ERR("failed to request vfs %d", ret);
        goto closure;
    }

    ret = timed_read(conn, reply_buf, sizeof(vfs_reply), 100);
    if (ret <= 0) {
        LOG_ERR("failed to read vfs reply");
        goto closure;
    }

    closure:
    close(conn);

    return (ret >= (int) sizeof(vfs_reply)) ? reply->ret : -3;
}

int fread(int fd, char *buf, uint32_t buf_size) {
    if (!buf || !buf_size) {
        return -1;
    }

    const void *conn = connect(VFS_PORT, sizeof(vfs_command) + buf_size);
    if (!conn) {
        LOG_ERR("failed to connect to vfs");
        return -2;
    }

    char cmd_buf[sizeof(vfs_command) + sizeof(vfs_rw_req)];

    vfs_command *cmd   = (void *) cmd_buf;
    vfs_reply    reply = {0};

    cmd->command = VFS_READ;

    vfs_rw_req *req = (void *) (cmd + 1);

    req->fd   = fd;
    req->size = buf_size;

    uint32_t offset = 0;

    int ret = write(conn, cmd_buf, sizeof(vfs_command) + sizeof(vfs_rw_req));
    if (ret < 0) {
        LOG_ERR("failed to request vfs");
        goto closure;
    }

    ret = timed_read(conn, (char *) &reply, sizeof(vfs_reply), 100);
    if (ret < (int) sizeof(vfs_reply) || reply.ret < 0) {
        LOG_ERR("failed to read vfs reply");
        goto closure;
    }

    while ((int) offset < reply.ret) {
        ret = timed_read(conn, buf + offset, buf_size - offset, 100);
        if (ret > 0) {
            offset += ret;
        }
    }

    closure:
    close(conn);

    return (ret < 0) ? ret : (int) offset;
}

int fwrite(int fd, const char *buf, uint32_t buf_size) {
    (void) fd;
    (void) buf;
    (void) buf_size;
    return -1;
}

int fclose(int fd) {
    const void *conn = connect(VFS_PORT, sizeof(vfs_command) + sizeof(vfs_fd_req));
    if (!conn) {
        LOG_ERR("failed to connect to vfs");
        return -2;
    }

    char cmd_buf[sizeof(vfs_command) + sizeof(vfs_fd_req)];
    char reply_buf[sizeof(vfs_reply)];

    vfs_command *cmd   = (void *) cmd_buf;
    vfs_reply   *reply = (void *) reply_buf;

    cmd->command = VFS_CLOSE;

    vfs_fd_req *req = (void *) (cmd + 1);

    req->fd = fd;

    int ret = write(conn, cmd_buf, sizeof(vfs_command) + sizeof(vfs_fd_req));
    if (ret < 0) {
        LOG_ERR("failed to request vfs");
        goto closure;
    }

    ret = timed_read(conn, (char *) reply, sizeof(vfs_reply), 100);
    if (ret <= 0) {
        LOG_ERR("failed to read vfs reply");
        goto closure;
    }

    closure:
    close(conn);

    return (ret == (int) sizeof(vfs_reply)) ? reply->ret : -3;
}

void vfs_loop(void) {
    restart:

    const void * client = NULL;
    const void * connection = NULL;

    fs_ifc *fs = NULL;

    disk_ifc *disk = sd_spi_init(&cfg);

    file_desc *root = NULL;

    part_info info;

    if (disk) {
        disk_stat stat;
        uint8_t buf[512];
        if (!disk->read(disk, 0, buf)) {
            disk->get_stat(disk, &stat);
            for (unsigned i = 0; i < MBR_PART_COUNT; ++i) {
                if (mbr_get_part_info(buf, i, &info)) {
                    continue;
                }

                fs = ext2_init(disk, info.start_sector);
                if (fs) {
                    root = fs->mount(fs);
                    break;
                }
            }
        }
    }

    if (!root) {
        sleep(1000);
        goto restart;
    }

    int ret = socket(VFS_PORT);
    ASSERT(!ret);

    open_fd = list_create();
    ASSERT(open_fd);

    printf("\nrootfs mounted\nsize: %u MB\n", info.size_mb);

    fs_current_path_set(root->name);

    while (1) {
        sleep(1);

        client = listen(VFS_PORT);
        if (client) {
            const void * conn = accept(VFS_PORT, client, VFS_REPL_BUFFER_SIZE);
            if (!conn) {
                LOG_DBG("Failed to accept sock connection");
            }
        }

        connection = select(VFS_PORT);
        if (connection) {
            char io_buffer[VFS_SOCK_BUFFER_SIZE];

            const void *connection_peer = peer(connection);
            if (!connection_peer) {
                LOG_ERR("sock peer not found");
                continue;
            }

            int ret = timed_read(connection, io_buffer, VFS_SOCK_BUFFER_SIZE, 100);
            if (ret > (int) sizeof(vfs_command)) {
                vfs_command *command = (void *) io_buffer;
                if (command->command < sizeof(handlers)) {
                    int ret = handlers[command->command](connection, command, root, fs);
                    if (ret < 0) {
                        LOG_ERR("Failed to respond control signal");
                    }
                }
            } else {
                LOG_ERR("Junk command");
            }
        }
    }
}

KERNEL_SRV(vfs, vfs_loop, 1024, 15);
