#ifndef SRV_VFS_H
#define SRV_VFS_H

#include "common/def.h"
#include "kernel/syscall.h"
#include "lib/string.h"
#include "fs/fs.h"

#define VFS_BUF_MAX 256

typedef enum vfs_control_t {
    VFS_OPEN    = 0x00,
    VFS_STAT    = 0x01,
    VFS_READ    = 0x02,
    VFS_WRITE   = 0x03,
    VFS_CLOSE   = 0x04,
} vfs_control;

typedef struct vfs_command_t {
    uint8_t           command;
    char              buf[];
} __attribute__((packed)) vfs_command;

typedef struct vfs_reply_t {
    int32_t  ret;
    char     buf[];
} __attribute__((packed)) vfs_reply;

typedef struct file_stat_t {
    uint32_t  size;
    file_type type;
} file_stat;

int fstat(int fd, file_stat *stat);

int fopen(const char *path);

int fread(int fd, char *buf, uint32_t buf_size);

int fwrite(int fd, const char *buf, uint32_t buf_size);

int fclose(int fd);

#endif
