#ifndef FS_MBR_H
#define FS_MBR_H

#include "common/common.h"

#define MBR_PART_COUNT  4
#define MBR_SECTOR_SIZE 512

typedef struct part_info_t {
    uint32_t size_mb;
    uint32_t start_sector;
    uint8_t  type;
} part_info;

int mbr_get_part_info(void *buf, uint8_t idx, part_info *info);

#endif
