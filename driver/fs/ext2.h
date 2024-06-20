#ifndef FS_EXT2_H
#define FS_EXT2_H

#include "common/common.h"
#include "fs/fs.h"
#include "driver/disk.h"

fs_ifc *ext2_init(disk_ifc *disk, uint32_t disk_blk);

#endif
