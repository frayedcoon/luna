#include "lib/list.h"
#include "kernel/memory.h"
#include "common/utils.h"

typedef struct mutable_list_node_t {
    void                           *ptr;
    struct mutable_list_node_t     *prv;
    struct mutable_list_node_t     *nxt;
} mutable_list_node;

typedef struct list_ctx_t {
    list_ifc                 ifc;

    mutable_list_node       *front;
    mutable_list_node       *back;
} list_ctx;

static const
list_node *list_get_front(list_ifc *ifc) {
    if (!ifc) {
        return NULL;
    }

    list_ctx *ctx = containerof(ifc, list_ctx, ifc);

    return ((list_node*) ctx->front);
}

static const
list_node *list_get_back(list_ifc *ifc) {
    if (!ifc) {
        return NULL;
    }

    list_ctx *ctx = containerof(ifc, list_ctx, ifc);

    return ((list_node*) ctx->back);
}

static const
list_node *list_insert_before(list_ifc *ifc, const list_node *node, void *ptr) {
    if (!ifc) {
        return NULL;
    }

    list_ctx *ctx = containerof(ifc, list_ctx, ifc);

    if (node && !ctx->front) {
        return NULL;
    }

    mutable_list_node *tmp = NULL;
    mutable_list_node *bfr = (node) ?
                             ((mutable_list_node*) node->prv) : NULL;
    mutable_list_node *aft = (node) ?
                             ((mutable_list_node*) node)      : ctx->front;

    tmp = cell_alloc(sizeof(mutable_list_node));
    if (!tmp) {
        return NULL;
    }

    memset(tmp, 0, sizeof(mutable_list_node));

    if (!ctx->front && !ctx->back) {
        ctx->front = tmp;
        ctx->back  = tmp;
    }

    tmp->prv = bfr;
    tmp->nxt = aft;

    tmp->ptr = ptr;

    if (!tmp->prv) {
        ctx->front = tmp;
    }

    if (bfr) {
        bfr->nxt = tmp;
    }

    if (aft) {
        aft->prv = tmp;
    }

    return ((list_node*) tmp);
}

static const
list_node *list_insert_after(list_ifc *ifc, const list_node *node, void *ptr) {
    if (!ifc) {
        return NULL;
    }

    list_ctx *ctx = containerof(ifc, list_ctx, ifc);

    if (node && !ctx->back) {
        return NULL;
    }

    mutable_list_node *tmp = NULL;
    mutable_list_node *bfr = (node) ?
                             ((mutable_list_node*) node     ) : ctx->back;
    mutable_list_node *aft = (node) ?
                             ((mutable_list_node*) node->nxt) : NULL;

    tmp = cell_alloc(sizeof(mutable_list_node));
    if (!tmp) {
        return NULL;
    }

    memset(tmp, 0, sizeof(mutable_list_node));

    if (!ctx->front && !ctx->back) {
        ctx->front = tmp;
        ctx->back  = tmp;
    }

    tmp->prv = bfr;
    tmp->nxt = aft;

    tmp->ptr = ptr;

    if (!tmp->nxt) {
        ctx->back = tmp;
    }

    if (bfr) {
        bfr->nxt = tmp;
    }

    if (aft) {
        aft->prv = tmp;
    }

    return ((list_node*) tmp);
}

static
void list_delete(list_ifc *ifc, const list_node *node, list_ctx_dstor dstor) {
    if (!ifc || !node) {
        return;
    }

    list_ctx *ctx = containerof(ifc, list_ctx, ifc);

    mutable_list_node *bfr = (mutable_list_node*) node->prv;
    mutable_list_node *aft = (mutable_list_node*) node->nxt;

    if (bfr) {
        bfr->nxt   = aft;
    } else {
        ctx->front = aft;
        if (aft) {
            aft->prv   = NULL;
        }
    }

    if (aft) {
        aft->prv   = bfr;
    } else {
        ctx->back  = bfr;
        if (bfr){
            bfr->nxt   = NULL;
        }
    }

    if (dstor) {
        dstor(node->ptr);
    }

    cell_free((void*) node);
}

static
int list_size(list_ifc *ifc) {
    if (!ifc) {
        return -1;
    }

    list_ctx *ctx = containerof(ifc, list_ctx, ifc);

    mutable_list_node *tmp = ctx->front;

    int i = 0;

    while (tmp) {
        i++;
        tmp = tmp->nxt;
    }

    return i;
}

static
void list_destroy(list_ifc *ifc, list_ctx_dstor dstor) {
    if (!ifc) {
        return;
    }

    list_ctx *ctx = containerof(ifc, list_ctx, ifc);

    list_node *tmp = (list_node*) ctx->front;

    while (tmp) {
        ifc->delete(ifc, tmp, dstor);

        tmp = (list_node*) ctx->front;
    }

    cell_free(ctx);
}

static const
list_ifc list_ops = {
    .get_front     = list_get_front,
    .get_back      = list_get_back,
    .insert_before = list_insert_before,
    .insert_after  = list_insert_after,
    .delete        = list_delete,
    .size          = list_size,
    .destroy       = list_destroy,
};

list_ifc *list_create(void) {
    list_ctx *ctx = cell_alloc(sizeof(list_ctx));
    if (!ctx) {
        return NULL;
    }

    memset(ctx, 0, sizeof(list_ctx));
    memcpy(&ctx->ifc, &list_ops, sizeof(list_ifc));

    return &ctx->ifc;
}
