#include "srv/vfs.h"
#include "common/log.h"
#include "common/sys.h"
#include "lib/parser.h"
#include "kernel/socket.h"
#include "platform/clock.h"

#define VFS_SOCK_BUFFER_SIZE       32

#define VFS_CONNECTION_BUFFER_SIZE 32

#define VFS_CONNECTIONS_MAX        (sizeof(uint32_t) * 8)

typedef struct vfs_node_t {
    const struct vfs_node_t      *parent;

    char                         *name;
    vfs_node_type                 type;
    void                         *ctx;
} vfs_node;

typedef struct vfs_ctx_t {
    vfs_node          *root;

    list_ifc          *users;
} vfs_ctx;

typedef struct vfs_user_t {
    const void       *user;

    const vfs_node   *current_dir;
} vfs_user;

typedef struct dir_ctx_t {
    list_ifc          *dirents;
} dir_ctx;

typedef struct link_ctx_t {
    vfs_node          *dest;
} link_ctx;

typedef struct port_ctx_t {
    int                fd;
} port_ctx;

static vfs_ctx context = {0};

static
vfs_ctx *ctx = NULL;

static
const
char *vfs_link_parent = "..";

static
const
char *vfs_link_self   = ".";

static char io_buffer[VFS_SOCK_BUFFER_SIZE] = {0};

typedef struct vfs_client_ctx_t {
    const void * conn;
    uint8_t buffer[VFS_CONNECTION_BUFFER_SIZE];
} vfs_client_ctx;

static vfs_client_ctx client_ctx[VFS_CONNECTIONS_MAX] = {0};

static
vfs_node *vfs_add_node(vfs_node_type type, void *cfg);

static
int dir_ctx_create_dir_links(const vfs_node *dest, const vfs_node *self) {
    vfs_link_cfg parent_link_cfg = {
        .common             = {
            .name               = (char*) vfs_link_parent,
            .parent             = self,
        },
        .link_dest          = dest,
    };

    vfs_node *parent_link = vfs_add_node(VFS_NODE_LINK, &parent_link_cfg);
    if (!parent_link) {
        return -1;
    }

    vfs_link_cfg self_link_cfg = {
        .common             = {
            .name               = (char*) vfs_link_self,
            .parent             = self,
        },
        .link_dest          = self,
    };

    vfs_node *self_link = vfs_add_node(VFS_NODE_LINK, &self_link_cfg);
    if (!self_link) {
        return -2;
    }

    return 0;
}

static
void *dir_ctx_cstor(vfs_dir_cfg *cfg) {
    vfs_node *node = malloc(sizeof(vfs_node));
    if (!node) {
        goto failed_node;
    }

    node->parent = cfg->common.parent;
    if (node->parent) {
        const vfs_node *parent_node  = cfg->common.parent;
        list_ifc *dirents            = ((dir_ctx*) parent_node->ctx)->dirents;

        dirents->insert_after(dirents, NULL, node);
    }

    node->name = strdup(cfg->common.name);
    if (!node->name) {
        goto failed_node_name;
    }

    node->type = VFS_NODE_DIR;

    node->ctx  = malloc(sizeof(dir_ctx));
    if (!node) {
        goto failed_node_ctx;
    }

    dir_ctx *node_ctx = (dir_ctx*) node->ctx;

    node_ctx->dirents = list_create();
    if (!node_ctx->dirents) {
        goto failed_node_ctx_dirents;
    }

    int ret = dir_ctx_create_dir_links((node->parent) ? node->parent :
        node, node);
    if (ret) {
        goto failed_create_link;
    }

    return node;

    failed_create_link:

    node_ctx->dirents->destroy(node_ctx->dirents, free);

    failed_node_ctx_dirents:

    free(node->ctx);

    failed_node_ctx:

    free(node->name);

    failed_node_name:

    free(node);

    failed_node:

    return NULL;
}

static
void *link_ctx_cstor(vfs_link_cfg *cfg) {
    vfs_node *node = zmalloc(sizeof(vfs_node));
    if (!node) {
        goto fail;
    }

    node->parent = cfg->common.parent;
    if (cfg->common.parent) {
        const vfs_node *parent_node  = cfg->common.parent;
        list_ifc *dirents            = ((dir_ctx*) parent_node->ctx)->dirents;

        dirents->insert_after(dirents, NULL, node);
    }

    node->name = strdup(cfg->common.name);
    if (!node->name) {
        goto fail;
    }

    node->type = VFS_NODE_LINK;

    node->ctx  = malloc(sizeof(link_ctx));
    if (!node) {
        goto fail;
    }

    link_ctx *node_ctx = (link_ctx*) node->ctx;

    node_ctx->dest = (void*) cfg->link_dest;

    return node;

fail:
    if (node) {
        if (node->name) {
            free(node->name);
        }
        free(node);
    }

    return NULL;
}

static
void *port_ctx_cstor(vfs_port_cfg *cfg) {
    vfs_node *node = malloc(sizeof(vfs_node));
    if (!node) {
        return NULL;
    }

    node->parent = cfg->common.parent;
    if (cfg->common.parent) {
        const vfs_node *parent_node  = cfg->common.parent;
        list_ifc *dirents            = ((dir_ctx*) parent_node->ctx)->dirents;

        dirents->insert_after(dirents, NULL, node);
    }

    node->name = strdup(cfg->common.name);
    if (!node->name) {
        free(node);
        return NULL;
    }

    node->type = VFS_NODE_PORT;

    node->ctx  = malloc(sizeof(port_ctx));
    if (!node->ctx) {
        free(node->name);
        free(node);
        return NULL;
    }

    port_ctx *prt_ctx = (port_ctx*) node->ctx;

    prt_ctx->fd = cfg->fd;

    return node;
}

void* (*node_ctx_cstor[VFS_NODE_TYPE_SIZE])(void*) = {
    (void*) dir_ctx_cstor,
    (void*) link_ctx_cstor,
    (void*) port_ctx_cstor,
};

static
vfs_node *vfs_add_node(vfs_node_type type, void *cfg) {
    if (node_ctx_cstor[type]) {
        return node_ctx_cstor[type](cfg);
    }

    return NULL;
}

static
void purge_dir_heldper(dir_ctx *ctx_dir, list_ctx_dstor dstor) {
    ctx_dir->dirents->destroy(ctx_dir->dirents, dstor);
}

static
void vfs_purge_node(vfs_node *node) {
    ASSERT(node && node->type >= VFS_NODE_TYPE_FRST &&
        node->type <= VFS_NODE_TYPE_SIZE);

    switch (node->type) {
        case VFS_NODE_DIR:
            purge_dir_heldper(node->ctx, (list_ctx_dstor) vfs_purge_node);
            break;
        case VFS_NODE_LINK:
        case VFS_NODE_PORT:
            break;
    }

    free(node->ctx);

    free(node->name);
}

static
void vfs_clean_ctx(void) {
    if (!ctx) {
        return;
    }

    vfs_purge_node(ctx->root);

    ctx->users->destroy(ctx->users, free);

    ctx = NULL;
}

static
const
vfs_node *vfs_resolve_path(const char *path, const vfs_node *work_dir) {
    if (!path) {
        return NULL;
    }

    list_ifc *entries = parse_buffer(path, '/', 0);
    if (!entries) {
        return NULL;
    }

    const vfs_node *node = (work_dir) ? work_dir : ctx->root;

    const list_node *path_entry = entries->get_front(entries);
    while (path_entry) {
        const dir_ctx *dirent_ctx = (dir_ctx*) node->ctx;
        const list_node *dirent   =
            dirent_ctx->dirents->get_front(dirent_ctx->dirents);

        while (dirent) {
            vfs_node *tmp = dirent->ptr;

            if (strcmp(tmp->name, path_entry->ptr) == 0) {
                while (tmp->type == VFS_NODE_LINK) {
                    link_ctx *ctx_link = (link_ctx*) tmp->ctx;
                    if (ctx_link->dest->type == VFS_NODE_DIR) {
                        node = ctx_link->dest;
                        goto found;
                    } else if (ctx_link->dest->type == VFS_NODE_LINK) {
                        tmp = ctx_link->dest;
                    }
                }
                //! found path entry in vfs
                if (tmp->type == VFS_NODE_DIR) {
                    node = tmp;

                    goto found;
                }
            }

            dirent = dirent->nxt;
        }

        node = NULL;

        break;

        found:

        path_entry = path_entry->nxt;
    }

    entries->destroy(entries, free);

    return node;
}

static
const
vfs_node *vfs_get_parent(const char *path, const vfs_node *current_dir) {
    if (!path) {
        return NULL;
    }

    char *last_slash = (path) ? strrchr (path, '/') : NULL;

    if (last_slash == path) {
        ++last_slash;
    } else if (last_slash != NULL && last_slash[1] == '\0') {
        last_slash = memchr(path, last_slash - path, '/');
    }

    if (last_slash != NULL) {
        last_slash[0] = '\0';
    } else {
        path = (char *) vfs_link_self;
    }

    return vfs_resolve_path(path, current_dir);
}

static
const
vfs_user *find_vfs_user(const void * user_ctx) {
    ASSERT(ctx);

    const list_node *user_entry = ctx->users->get_front(ctx->users);
    while (user_entry) {
        const vfs_user *user = user_entry->ptr;

        if (user->user == user_ctx) {
            return user;
        }

        user_entry = user_entry->nxt;
    }

    return NULL;
}

static
int vfs_mkdir(const char *path, const void * user) {
    if (!ctx || !path || !path[0]) {
        return -1;
    }

    int absolute_path = (path[0] == VFS_SEP_NAME) ? 1 : 0;

    list_ifc *path_entries = parse_buffer(path, '/', 0);
    if (!path_entries) {
        return -2;
    }

    const vfs_node *parent = vfs_get_parent(path, (absolute_path) ? NULL : find_vfs_user(user)->current_dir);
    if (!parent) {
        path_entries->destroy(path_entries, free);
        return -4;
    }

    const dir_ctx *ctx_dir = (dir_ctx*) parent->ctx;
    const list_node *entry = ctx_dir->dirents->get_front(ctx_dir->dirents);
    while (entry) {
        vfs_node *entry_ctx = (vfs_node*) entry->ptr;
        if (strncmp(entry_ctx->name, path_entries->get_back(path_entries)->ptr,
            strsize(entry_ctx->name)) == 0) {
            path_entries->destroy(path_entries, free);
            return -5;
        }

        entry = entry->nxt;
    }

    vfs_dir_cfg dir_cfg = {
        .common             = {
            .name               = path_entries->get_back(path_entries)->ptr,
            .parent             = parent,
        },
    };

    const vfs_node *dir_node = vfs_add_node(VFS_NODE_DIR, &dir_cfg);
    if (dir_node) {
        return 0;
    }

    return -6;
}

static
int vfs_list_dir(const vfs_node *node, char *buf, int buf_size) {
    if (!ctx || !node) {
        return -1;
    }

    strcpy(buf, "");

    const dir_ctx *ctx_dir = (dir_ctx*) node->ctx;
    int ret_len = ctx_dir->dirents->size(ctx_dir->dirents);
    if (!ret_len) {
        return 0;
    }

    const list_node *entry = ctx_dir->dirents->get_front(ctx_dir->dirents);
    while (entry) {
        char *entry_name =((vfs_node*)entry->ptr)->name;

        ret_len += strlen(entry_name);

        entry = entry->nxt;
    }

    if (ret_len > buf_size) {
        return -2;
    }

    strcat(buf, "0, ");

    entry = ctx_dir->dirents->get_front(ctx_dir->dirents);
    while (entry) {
        char *entry_name =((vfs_node*)entry->ptr)->name;

        strcat(buf, entry_name);
        strcat(buf, " ");

        entry = entry->nxt;
    }

    return 0;
}

static
int vfs_change_dir(const char *path, const void *user) {
    if (!ctx || !path || !path[0]) {
        return -1;
    }

    int absolute_path = (path[0] == VFS_SEP_NAME) ? 1 : 0;

    const vfs_user *user_ctx = find_vfs_user(user);

    const vfs_node *dirent = vfs_resolve_path(path,
        (absolute_path) ? NULL : user_ctx->current_dir);
    if (!dirent) {
        return -2;
    }

    vfs_user *mutable_user = (vfs_user*) user_ctx;

    mutable_user->current_dir = dirent;

    return 0;

}

static
vfs_client_ctx *find_free_client_slot(void) {
    for (vfs_client_ctx * ctx = client_ctx; ctx < client_ctx + countof(client_ctx); ctx++) {
        if (!ctx->conn) {
            return ctx;
        }
    }

    return NULL;
}

static
void client_slot_free(vfs_client_ctx * ctx) {
    if (!ctx) {
        return;
    }

    close(ctx->conn);

    ctx->conn = NULL;
}

static
void connections_cleanup(void) {
    for (vfs_client_ctx * ctx = client_ctx; ctx < client_ctx + countof(client_ctx); ctx++) {
        if (!ctx->conn) {
            continue;
        }
        if (!peer(ctx->conn)) {
            client_slot_free(ctx);
        }
    }
}

void vfs_loop(void) {
    vfs_clean_ctx();

    vfs_node * node = NULL;

    ctx = &context;

    ctx->users = list_create();

    vfs_dir_cfg node_cfg = {
        .common             = {
            .name               = "/",
            .parent             = NULL,
        },
    };

    ctx->root = vfs_add_node(VFS_NODE_DIR, &node_cfg);
    ASSERT(ctx->root);

    int ret = socket(VFS_PORT);
    ASSERT(!ret);

    node_cfg.common.name = "srv";
    node_cfg.common.parent = ctx->root;
    node = vfs_add_node(VFS_NODE_DIR, &node_cfg);
    ASSERT(node);

    node_cfg.common.name = "vfs";
    node_cfg.common.parent = node;
    node = vfs_add_node(VFS_NODE_PORT, &node_cfg);
    ASSERT(node);

    const void * client = NULL;
    const void * connection = NULL;

    while(1) {
        sleep(100);
        connections_cleanup();

        client = listen(VFS_PORT);
        if (client) {
            vfs_client_ctx *client_ctx = find_free_client_slot();
            if (!client_ctx) {
                decline(VFS_PORT, client);
                continue;
            }

            client_ctx->conn = accept(VFS_PORT, client, client_ctx->buffer, VFS_CONNECTION_BUFFER_SIZE);
            if (!client_ctx->conn) {
                LOG_DBG("Failed to accept sock connection");
            } else {
                vfs_user *user_entry = (vfs_user*) find_vfs_user(client);
                if (!user_entry) {
                    user_entry = malloc(sizeof(vfs_user));
                    ASSERT(user_entry);

                    user_entry->user        = client;
                    user_entry->current_dir = ctx->root;

                    ctx->users->insert_after(ctx->users, NULL, user_entry);
                }
            }
        }

        connection = select(VFS_PORT);
        if (connection) {
            char reply[VFS_SOCK_BUFFER_SIZE];

            const void *connection_peer = peer(connection);
            if (!connection_peer) {
                LOG_ERR("sock peer not found");
                continue;
            }

            ret = timed_read(connection, io_buffer, VFS_SOCK_BUFFER_SIZE, 100);
            if (ret > 0) {
                list_ifc *params = parse_buffer(io_buffer, ' ', 0);
                if (!params) {
                    LOG_ERR("failed to parse request: %s", io_buffer);
                    continue;
                }

                const list_node *param = params->get_front(params);

                int param_size = params->size(params);
                if (!param_size) {
                    goto free_params_list;
                }

                LOG_DBG("\'%s\' control received", param->ptr);

                if (param_size == 2 && !strncmp(param->ptr, VFS_MKDIR_REQ, strlen(param->ptr))) {

                    param = param->nxt;

                    ret = vfs_mkdir(param->ptr, connection_peer);

                    ret = snprintf(reply, VFS_SOCK_BUFFER_SIZE,
                        "%d, %s", ret, &io_buffer[6]);
                    if (ret > 0 && ret < VFS_SOCK_BUFFER_SIZE) {
                        ret = write(connection, reply, strsize(reply));
                        if (ret > 0) {
                            goto free_params_list;
                        }
                        LOG_ERR("MKDIR: failed to reply");
                    }

                } else if (param_size == 1 && strncmp(param->ptr, VFS_LS_REQ, strlen(param->ptr)) == 0) {

                    const vfs_user *user = find_vfs_user(connection_peer);
                    if (!user) {
                        LOG_ERR("requester not found");
                        goto free_params_list;
                    }

                    ret = vfs_list_dir(user->current_dir, reply,
                        VFS_SOCK_BUFFER_SIZE);
                    if (ret) {
                        LOG_ERR("failed to get dir list: %d", ret);
                        ret = snprintf(reply, VFS_SOCK_BUFFER_SIZE, "%d", ret);
                        if (ret < 0 || ret >= VFS_SOCK_BUFFER_SIZE) {
                            LOG_ERR("failed to compose reply");
                            goto free_params_list;
                        }
                    }

                    ret = write(connection, reply, strsize(reply));
                    if (ret > 0) {
                        goto free_params_list;
                    }

                    LOG_ERR("LS: failed to reply");
                } else if (param_size == 2 && strncmp(param->ptr, VFS_CD_REQ, strlen(param->ptr)) == 0) {

                    param = param->nxt;

                    ret = vfs_change_dir(param->ptr, connection_peer);
                    ret = snprintf(reply, VFS_SOCK_BUFFER_SIZE, "%d", ret);
                    if (ret < 0 || ret >= VFS_SOCK_BUFFER_SIZE) {
                        LOG_ERR("failed to compose reply");
                        goto free_params_list;
                    }

                    ret = write(connection, reply, strsize(reply));
                    if (ret > 0) {
                        goto free_params_list;
                    }

                    LOG_ERR("CD: failed to reply");
                }

                free_params_list:

                params->destroy(params, free);
            }
        }
    }
}

KERNEL_SRV(vfs, vfs_loop, 128, 15);
