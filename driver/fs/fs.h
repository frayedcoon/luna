#ifndef FS_FS_H
#define FS_FS_H

#include "common/common.h"
#include "lib/list.h"

typedef enum file_type_t {
    F_TYPE_INV = 0x00,
    F_TYPE_REG = 0x01,
    F_TYPE_DIR = 0x02,
} file_type;

typedef struct file_desc_t {
    //! struct size + name (including '\0') + hidden fs-specific data (if any)
    uint32_t            desc_size;
    file_type           type;

    uint32_t            size;

    struct file_desc_t *link;
    list_ifc           *child;
    char                name[];
} file_desc;

typedef struct dir_entry_t {
    file_type           type;
    uint32_t            size;
    char                name[];
} dir_entry;

typedef struct fs_ifc_t {
    file_desc* (*mount)(struct fs_ifc_t *ifc);
    int (*read)(struct fs_ifc_t *ifc, file_desc *fd, uint8_t *buf, uint32_t buf_size, uint32_t offset);
    int (*write)(struct fs_ifc_t *ifc, file_desc *fd, uint8_t *buf, uint32_t buf_size, uint32_t offset);
} fs_ifc;

const char *fs_current_path_get(void);

void fs_current_path_set(const char *path);

void normalize_path(char *path);

#endif
