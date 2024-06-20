#ifndef DRIVER_DISK_H
#define DRIVER_DISK_H

#include "common/common.h"

typedef struct disk_stat_t {
    uint32_t size_mb;
    uint32_t block_size;
} disk_stat;

typedef struct disk_ifc_t {
    int (*read)(struct disk_ifc_t *iface, uint32_t blk, uint8_t *buf);
    int (*write)(struct disk_ifc_t *iface, uint32_t blk, const uint8_t *buf);
    int (*get_stat)(struct disk_ifc_t *iface, disk_stat *stat);
    void  (*destroy)(struct disk_ifc_t *iface);
} disk_ifc;

#endif