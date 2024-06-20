#include "fs/mbr.h"

#define BOOTSTRAP_LEN  446

#define MAGIC_1        0x55
#define MAGIC_2        0xAA

typedef struct chs_t {
    uint8_t     head;
    uint16_t    sector:6;
    uint16_t    cylinder:10;
} __attribute__((packed)) chs;

typedef struct partition_t {
    uint8_t     active;
    chs         start;
    uint8_t     type;
    chs         end;
    uint32_t    start_sector;
    uint32_t    sector_num;
} __attribute__((packed)) partition;

typedef struct mbr_header_t {
    uint8_t     bootstrap[BOOTSTRAP_LEN];
    partition   partition[MBR_PART_COUNT];
    uint8_t     magic[2];
} __attribute__((packed)) mbr_header;

int mbr_get_part_info(void *buf, uint8_t idx, part_info *info) {
    mbr_header *header = (mbr_header *) buf;
    if (!header || !info) {
        return -1;
    }

    if (idx >= MBR_PART_COUNT) {
        return -2;
    }

    if (header->magic[0] != MAGIC_1 || header->magic[1] != MAGIC_2) {
        return -3;
    }

    if (!header->partition[idx].type ||
        !header->partition[idx].sector_num ||
        !header->partition[idx].start_sector) {
        return -4;
    }

    info->size_mb      = header->partition[idx].sector_num >> 11;
    info->start_sector = header->partition[idx].start_sector;
    info->type         = header->partition[idx].type;

    return 0;
}
