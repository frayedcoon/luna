#include "fs/ext2.h"
#include "kernel/syscall.h"
#include "lib/string.h"

#define SB_OFFSET           0x400
#define SB_SIZE             0x400

#define LOG_SIZE_BASE       0x400

#define EXT2_MAGIC          0xef53

#define ROOT_INODE          2

#define INODE_DIRECT_BLOCKS 12
#define INODE_BLOCKS_TOTAL  15

#define REV0_INODE_SIZE  128
#define REV0_INODE_FIRST 11

#define UUID_P "%02x%02x%02x%02x-%02x%02x%02x%02x-%02x%02x%02x%02x-%02x%02x%02x%02x\n"
#define UUID(_u) _u[0],  _u[1],  _u[2],  _u[3], \
                 _u[4],  _u[5],  _u[6],  _u[7], \
                 _u[8],  _u[9],  _u[10], _u[11],\
                 _u[12], _u[13], _u[14], _u[15] \

typedef enum inode_file_format_t {
    EXT2_S_IFSOCK = 0xC000,
    EXT2_S_IFLNK  = 0xA000,
    EXT2_S_IFREG  = 0x8000,
    EXT2_S_IFBLK  = 0x6000,
    EXT2_S_IFDIR  = 0x4000,
    EXT2_S_IFCHR  = 0x2000,
    EXT2_S_IFIFO  = 0x1000,
} inode_file_format;

typedef enum inode_process_execution_t {
    EXT2_S_ISUID  = 0x0800,
    EXT2_S_ISGID  = 0x0400,
    EXT2_S_ISVTX  = 0x0200,
} inode_process_execution;

typedef enum inode_access_rights_t {
    EXT2_S_IRUSR  = 0x0100,
    EXT2_S_IWUSR  = 0x0080,
    EXT2_S_IXUSR  = 0x0040,
    EXT2_S_IRGRP  = 0x0020,
    EXT2_S_IWGRP  = 0x0010,
    EXT2_S_IXGRP  = 0x0008,
    EXT2_S_IROTH  = 0x0004,
    EXT2_S_IWOTH  = 0x0002,
    EXT2_S_IXOTH  = 0x0001,
} inode_access_rights;

typedef struct sb_perf_hints_t {
    uint8_t          preallocated_blocks_count;
    uint8_t          preallocated_dir_blocks_count;
    uint8_t          padding[2];
} __attribute__((packed)) sb_perf_hints;

typedef struct sb_journal_t {
    uint8_t          uuid[16];
    uint32_t         inode_number;
    uint32_t         device;
    uint32_t         last_orphan;
} __attribute__((packed)) sb_journal;

typedef struct sb_dir_index_t {
    uint8_t          hash_seed[16];
    uint8_t          def_hash_version;
    uint8_t          padding[3];
} __attribute__((packed)) sb_dir_index;

typedef struct sb_opts_t {
    uint32_t         default_mount_option;
    uint32_t         first_meta_bg;
} __attribute__((packed)) sb_opts;

typedef struct sb_dynamic_t {
    uint32_t         first_inode;
    uint16_t         inode_structure_size;
    uint16_t         block_group_number;
    uint32_t         compatible_feature_flags;
    uint32_t         incompatible_feature_flags;
    uint32_t         read_only_feature_flags;
    uint8_t          uuid[16];
    char             volume_name[16];
    char             last_mounted_path[64];
    uint32_t         algorithm_usage_bitmap;
    sb_perf_hints    perf_hints;
    sb_journal       journal;
    sb_dir_index     dir_index;
    sb_opts          opts;
} __attribute__((packed)) sb_dynamic;

typedef struct ext2_superblock_t {
    uint32_t         inode_count;
    uint32_t         block_count;
    uint32_t         reserved_block_count;
    uint32_t         free_block_count;
    uint32_t         free_inode_count;
    uint32_t         first_data_block;
    uint32_t         log_block_size;
    uint32_t         log_fragmentation_size;
    uint32_t         block_per_group;
    uint32_t         fragmentation_per_group;
    uint32_t         inode_per_group;
    uint32_t         mtime;
    uint32_t         wtime;
    uint16_t         mount_count;
    uint16_t         max_mount_count;
    uint16_t         magic;
    uint16_t         state;
    uint16_t         errors;
    uint16_t         minor_version;
    uint32_t         last_consistency_check_time;
    uint32_t         check_interval;
    uint32_t         creator_os;
    uint32_t         rev_level;
    uint16_t         uid_that_can_use_reserved_blocks;
    uint16_t         gid_that_can_use_reserved_blocks;
    sb_dynamic       dynamic;
    //! 760 reserved bytes
} __attribute__((packed)) ext2_superblock;

typedef struct block_group_desc_table_t {
    uint32_t         block_bitmap;
    uint32_t         inode_bitmap;
    uint32_t         inode_table;
    uint16_t         free_blocks_count;
    uint16_t         free_inodes_count;
    uint16_t         used_dirs_count;
    uint8_t          padding[14];
} __attribute__((packed)) block_group_desc_table;

typedef struct inode_t {
    uint16_t         mode;
    uint16_t         uid;
    uint32_t         size;
    uint32_t         atime;
    uint32_t         ctime;
    uint32_t         mtime;
    uint32_t         dtime;
    uint16_t         gid;
    uint16_t         links_count;
    uint32_t         blocks;
    uint32_t         flags;
    uint32_t         osd1;
    uint32_t         block[INODE_BLOCKS_TOTAL];
    uint32_t         generation;
    uint32_t         file_acl;
    uint32_t         dir_acl;
    uint32_t         faddr;
    uint32_t         osd2[3];
} __attribute__((packed)) inode;

typedef struct dir_t {
    uint32_t         inode;
    uint16_t         rec_len;
    uint8_t          name_len;
    uint8_t          file_type;
    char             name[];
} __attribute__((packed)) dir;

typedef struct ext2_ctx_t {
    fs_ifc             ifc;
    ext2_superblock    sb;
    disk_ifc          *disk;
    uint32_t           disk_offset;
    uint32_t           grp_count;
    uint32_t           block_desc_block;
    uint32_t           block_size;
    uint32_t           inode_size;
    uint32_t           first_inode;
    uint8_t            block[];
} ext2_ctx;

typedef enum dir_entry_type_t {
    EXT2_FT_UNKNOWN  = 0x00,
    EXT2_FT_REG_FILE = 0x01,
    EXT2_FT_DIR      = 0x02,
    EXT2_FT_CHRDEV   = 0x03,
    EXT2_FT_BLKDEV   = 0x04,
    EXT2_FT_FIFO     = 0x05,
    EXT2_FT_SOCK     = 0x06,
    EXT2_FT_SYMLINK  = 0x07,
} dir_entry_type;

typedef struct file_read_opt_t {
    uint32_t  offset;
    uint32_t  block_offset;
    uint8_t  *buf;
    uint32_t  buf_size;
    uint32_t  block_num;
} file_read_opt;

typedef struct ext2_desc_priv_data_t {
    uint32_t inode;
} ext2_desc_priv_data;

#if 0
static void sb_dump(ext2_superblock *sb) {
    printf("inode_count                      = %u\n", sb->inode_count);
    printf("block_count                      = %u\n", sb->block_count);
    printf("reserved_block_count             = %u\n", sb->reserved_block_count);
    printf("free_block_count                 = %u\n", sb->free_block_count);
    printf("free_inode_count                 = %u\n", sb->free_inode_count);
    printf("first_data_block                 = %u\n", sb->first_data_block);
    printf("log_block_size                   = %u\n", sb->log_block_size);
    printf("log_fragmentation_size           = %u\n", sb->log_fragmentation_size);
    printf("block_per_group                  = %u\n", sb->block_per_group);
    printf("fragmentation_per_group          = %u\n", sb->fragmentation_per_group);
    printf("inode_per_group                  = %u\n", sb->inode_per_group);
    printf("mtime                            = %u\n", sb->mtime);
    printf("wtime                            = %u\n", sb->wtime);
    printf("mount_count                      = %u\n", sb->mount_count);
    printf("max_mount_count                  = %u\n", sb->max_mount_count);
    printf("magic_signature                  = 0x%04x\n", sb->magic);
    printf("state                            = %u\n", sb->state);
    printf("errors                           = %u\n", sb->errors);
    printf("minor_version                    = %u\n", sb->minor_version);
    printf("last_consistency_check_time      = %u\n", sb->last_consistency_check_time);
    printf("check_interval                   = %u\n", sb->check_interval);
    printf("creator_os                       = %u\n", sb->creator_os);
    printf("uid_that_can_use_reserved_blocks = %u\n", sb->uid_that_can_use_reserved_blocks);
    printf("gid_that_can_use_reserved_blocks = %u\n", sb->gid_that_can_use_reserved_blocks);

    printf("first_inode                      = %u\n", sb->dynamic.first_inode);
    printf("inode_structure_size             = %u\n", sb->dynamic.inode_structure_size);
    printf("block_group_number               = %u\n", sb->dynamic.block_group_number);
    printf("compatible_feature_flags         = %u\n", sb->dynamic.compatible_feature_flags);
    printf("incompatible_feature_flags       = %u\n", sb->dynamic.incompatible_feature_flags);
    printf("read_only_feature_flags          = %u\n", sb->dynamic.read_only_feature_flags);
    printf("uuid                             = "UUID_P, UUID(sb->dynamic.uuid));
    printf("volume_name                      = %s\n", sb->dynamic.volume_name);
    printf("last_mounted_path                = %s\n", sb->dynamic.last_mounted_path);
    printf("algorithm_usage_bitmap           = %u\n", sb->dynamic.algorithm_usage_bitmap);
}
#endif

static int disk_read_block(ext2_ctx *ctx, uint32_t block) {
    disk_stat disk_stat;
    ctx->disk->get_stat(ctx->disk, &disk_stat);

    uint32_t to_read = ctx->block_size;
    uint8_t *ptr = ctx->block;
    uint32_t read_blk = ctx->disk_offset + (block * (ctx->block_size / disk_stat.block_size));
    while (to_read) {
        if (ctx->disk->read(ctx->disk, read_blk, ptr)) {
            return -1;
        }
        read_blk++;
        ptr += disk_stat.block_size;
        to_read -= disk_stat.block_size;
    }

    return 0;
}

static int read_direct_block(ext2_ctx *ctx, file_read_opt *opt, uint32_t block) {
    opt->block_num++;
    if (opt->offset >= (opt->block_num * ctx->block_size)) {
        //! didn't reach offset yet, NOT an error
        return 0;
    }
    if (disk_read_block(ctx, block)) {
        return -1;
    }
    uint32_t to_read = opt->buf_size;
    if (to_read > ctx->block_size) {
        to_read = ctx->block_size;
    }
    memcpy(opt->buf, ctx->block + opt->block_offset, to_read);
    opt->buf += to_read;
    opt->buf_size -= to_read;

    opt->block_offset = 0;

    opt->block_num++;

    //! stop reading, buffer filled
    return (opt->buf_size ? 0 : -2);
}

static int read_indirect_block(ext2_ctx *ctx, file_read_opt *opt, uint32_t block, uint8_t indirection) {
    int ret = 0;
    uint32_t *cached_block = NULL;

    if (disk_read_block(ctx, block)) {
        ret = -1;
        goto end;
    }
    cached_block = malloc(ctx->block_size);
    if (!cached_block) {
        ret = -2;
        goto end;
    }
    memcpy(cached_block, ctx->block, ctx->block_size);
    for (uint32_t i = 0; i < ctx->block_size / sizeof(uint32_t); ++i) {
        if (!cached_block[i]) {
            ret = -3;
            goto end;
        }
        if (!indirection) {
            if (read_direct_block(ctx, opt, cached_block[i])) {
                ret = -4;
                goto end;
            }
        } else {
            if (read_indirect_block(ctx, opt, cached_block[i], indirection - 1)) {
                ret = -5;
                goto end;
            }
        }
    }

    end:

    if (cached_block) {
        free(cached_block);
    }

    return ret;
}

static uint32_t read_file(ext2_ctx *ctx, uint8_t *buf, uint32_t buf_size,
    uint32_t offset, inode *file) {
    if (file->size < offset + buf_size) {
        buf_size = file->size - offset;
    }
    file_read_opt opt = {
        .offset       = offset,
        .block_offset = offset % ctx->block_size,
        .buf          = buf,
        .buf_size     = buf_size,
        .block_num    = 0,
    };
    for (unsigned i = 0; i < INODE_BLOCKS_TOTAL; i++) {
        if (!file->block[i]) {
            break;
        }
        if (i < INODE_DIRECT_BLOCKS) {
            if (read_direct_block(ctx, &opt, file->block[i])) {
                break;
            }
        } else {
            uint8_t indirection = i - INODE_DIRECT_BLOCKS;
            if (read_indirect_block(ctx, &opt, file->block[i], indirection)) {
                break;
            }
        }
    }

    return (opt.buf - buf);
}

static inode *find_inode(ext2_ctx *ctx, uint32_t idx) {
    uint32_t inode_group = (idx - 1) / ctx->sb.inode_per_group;
    uint32_t inode_group_block = inode_group / (ctx->block_size / sizeof(block_group_desc_table));

    disk_read_block(ctx, ctx->block_desc_block + inode_group_block);

    block_group_desc_table *desc = (void *) ctx->block;
    desc += inode_group % (ctx->block_size / sizeof(block_group_desc_table));

    uint32_t used_inodes = ctx->sb.inode_per_group - desc->free_inodes_count;
    uint32_t inode_table = desc->inode_table;
    if (used_inodes) {
        uint32_t index        = (idx - 1) % ctx->sb.inode_per_group;
        uint32_t block        = (index * ctx->inode_size) / ctx->block_size;

        disk_read_block(ctx, inode_table + block);

        inode *inode = (void *) (ctx->block + (index * ctx->inode_size));

        return inode;
    }

    return NULL;
}

static list_ifc *dir_attach_children(ext2_ctx *ctx, inode *ind, file_desc *parent) {
    list_ifc *list = NULL;

    if (disk_read_block(ctx, ind->block[0])) {
        return NULL;
    }

    uint8_t *cached_block = malloc(ctx->block_size);
    if (!cached_block) {
        goto end;
    }
    memcpy(cached_block, ctx->block, ctx->block_size);


    dir *dir = (void *) cached_block;
    uint32_t displacement = 0;

    while (displacement < ctx->block_size) {
        if (!list) {
            list = list_create();
            if (!list) {
                goto end;
            }
        }

        file_type type = F_TYPE_INV;
        switch (dir->file_type) {
            case EXT2_FT_REG_FILE:
                type = F_TYPE_REG;
                break;
            case EXT2_FT_DIR:
                type = F_TYPE_DIR;
                break;
            default:
                break;
        }

        if (type != F_TYPE_INV) {
            uint32_t name_size  = dir->name_len + 1;
            uint32_t total_size = sizeof(file_desc) + name_size + sizeof(ext2_desc_priv_data);

            file_desc *node = malloc(total_size);
            if (!node || !list->insert_after(list, NULL, node)) {
                list->destroy(list, free);
                goto end;
            }

            ext2_desc_priv_data *priv = (ext2_desc_priv_data *) (node->name + name_size);

            priv->inode = dir->inode;

            memcpy(node->name, dir->name, dir->name_len);
            node->name[dir->name_len] = 0;

            node->desc_size = total_size;

            node->type = type;

            node->size = 0;

            node->link = NULL;

            node->child = NULL;

            inode *entry = find_inode(ctx, dir->inode);
            if (entry) {
                int link = 0;
                if (dir->name_len == 1 && !memcmp(dir->name, ".", dir->name_len)) {
                    link = 1;
                    //! self link
                    node->link = node;
                }
                if (dir->name_len == 2 && !memcmp(dir->name, "..", dir->name_len)) {
                    link = 1;
                    //! parent link
                    node->link = parent;
                }
                if (type == F_TYPE_DIR && !link) {
                    node->child = dir_attach_children(ctx, entry, node);
                } else if (type == F_TYPE_REG) {
                    node->size = entry->size;
                }
            }
        }

        displacement += dir->rec_len;

        dir = (void *) (cached_block + displacement);
    }

    end:

    if (cached_block) {
        free(cached_block);
    }

    return list;
}

static file_desc *ext2_mount(fs_ifc *ifc) {
    ext2_ctx *ctx = (ext2_ctx *) ifc;
    if (!ctx) {
        return NULL;
    }

    inode *inode = find_inode(ctx, ROOT_INODE);
    if (!inode) {
        return NULL;
    }

    uint32_t name_size  = strsize("/");
    uint32_t total_size = sizeof(file_desc) + name_size + sizeof(ext2_desc_priv_data);

    file_desc *root = malloc(total_size);
    if (!root) {
        return NULL;
    }

    ext2_desc_priv_data *priv = (ext2_desc_priv_data *) (root->name + name_size);

    priv->inode = ROOT_INODE;

    strcpy(root->name, "/");
    root->desc_size = total_size;
    root->type      = F_TYPE_DIR;
    root->size      = 0;
    root->child     = dir_attach_children(ctx, inode, root);

    return root;
}

static int ext2_read_file(ext2_ctx *ctx, file_desc *file, uint8_t *buf, uint32_t buf_size, uint32_t offset) {
    uint32_t name_size = strsize(file->name);
    if (file->desc_size != sizeof(file_desc) + name_size + sizeof(ext2_desc_priv_data)) {
        //! doesn't seem to be valid
        return -4;
    }

    ext2_desc_priv_data *priv = (ext2_desc_priv_data *) (file->name + name_size);

    inode *entry = find_inode(ctx, priv->inode);
    if (!entry) {
        return -5;
    }

    inode saved_inode;
    memcpy(&saved_inode, entry, sizeof(inode));

    return read_file(ctx, buf, buf_size, offset, &saved_inode);
}

static int ext2_read_dir(file_desc *file, uint8_t *buf, uint32_t buf_size, uint32_t offset) {
    int displacement = -1;

    dir_entry *entry = (void *) buf;

    list_ifc *child = file->child;
    if (child) {
        displacement = 0;

        const list_node *node = child->get_front(child);
        while (node) {
            file_desc *entry_desc = node->ptr;
            if (offset) {
                offset--;
            } else {
                uint32_t entry_size = sizeof(dir_entry) + strsize(entry_desc->name);
                if (entry_size + displacement > buf_size) {
                    break;
                }

                displacement += entry_size;

                entry->type = entry_desc->type;
                entry->size = entry_desc->size;

                strcpy(entry->name, entry_desc->name);

                entry = (void *) (buf + displacement);
            }

            node = node->nxt;
        }
    }

    return displacement;
}

static int ext2_read(fs_ifc *ifc, file_desc *file, uint8_t *buf, uint32_t buf_size, uint32_t offset) {
    ext2_ctx *ctx = (ext2_ctx *) ifc;
    if (!ctx) {
        return -1;
    }

    if (!file) {
        return -2;
    }

    if (!buf || !buf_size) {
        return -3;
    }

    switch (file->type) {
        case F_TYPE_REG:
            return ext2_read_file(ctx, file, buf, buf_size, offset);
        case F_TYPE_DIR:
            return ext2_read_dir(file, buf, buf_size, offset);
        case F_TYPE_INV:
            break;
    }

    return -4;
}

fs_ifc *ext2_init(disk_ifc *disk, uint32_t disk_blk) {
    if (!disk) {
        return 0;
    }

    disk_stat disk_stat;
    disk->get_stat(disk, &disk_stat);

    uint32_t to_read = SB_SIZE * 2;
    uint8_t *sb_buf = malloc(to_read);
    if (!sb_buf) {
        return NULL;
    }

    uint32_t disk_offset = disk_blk;

    uint8_t *ptr = sb_buf;
    while (to_read) {
        if (disk->read(disk, disk_blk, ptr)) {
            free(sb_buf);
            return NULL;
        }
        disk_blk++;
        ptr += disk_stat.block_size;
        to_read -= disk_stat.block_size;
    }

    ext2_superblock *sb_data = (ext2_superblock *) (sb_buf + SB_OFFSET);

    if (sb_data->magic != EXT2_MAGIC) {
        free(sb_buf);
        return NULL;
    }

    uint32_t group_count_blocks = sb_data->block_count / sb_data->block_per_group;
    if (sb_data->block_count % sb_data->block_per_group) {
        group_count_blocks++;
    }

    uint32_t group_count_inodes = sb_data->inode_count / sb_data->inode_per_group;
    if (sb_data->inode_count % sb_data->inode_per_group) {
        group_count_inodes++;
    }

    if (group_count_blocks != group_count_inodes) {
        free(sb_buf);
        return NULL;
    }

    uint32_t block_size = (LOG_SIZE_BASE << sb_data->log_block_size);

    free(sb_buf);

    ext2_ctx *ctx = malloc(sizeof(ext2_ctx) + block_size);
    if (!ctx) {
        return NULL;
    }

    ctx->disk = disk;

    ctx->disk_offset = disk_offset;

    ctx->block_size = block_size;

    memcpy(&ctx->sb, sb_data, sizeof(ext2_superblock));

    ctx->grp_count = group_count_blocks;

    ctx->inode_size  = ctx->sb.rev_level ? ctx->sb.dynamic.inode_structure_size : REV0_INODE_SIZE;
    ctx->first_inode = ctx->sb.rev_level ? ctx->sb.dynamic.first_inode : REV0_INODE_FIRST;

    ctx->block_desc_block = !ctx->sb.log_block_size + 1;

    ctx->ifc.mount = ext2_mount;
    ctx->ifc.read  = ext2_read;

    return &ctx->ifc;
}
