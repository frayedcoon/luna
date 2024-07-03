#ifndef LIB_LIST_H
#define LIB_LIST_H

#include "common/common.h"
#include "common/def.h"

typedef struct list_node_t {
    void                           *ptr;
    const struct list_node_t       *prv;
    const struct list_node_t       *nxt;
} list_node;

typedef void (*list_ctx_dstor)   (void *data_ptr);

typedef void* (*list_allocator)  (uint16_t size);
typedef void (*list_deallocator) (void *data_ptr);

typedef struct list_ifc_t {
    const
    list_node* (*get_front)     (struct list_ifc_t *ifc);
    const
    list_node* (*get_back)      (struct list_ifc_t *ifc);
    const
    list_node* (*insert_before) (struct list_ifc_t *ifc,
                                 const list_node *node, void *ptr);
    const
    list_node* (*insert_after)  (struct list_ifc_t *ifc,
                                 const list_node *node, void *ptr);

    void       (*delete)        (struct list_ifc_t *ifc, const list_node *node,
                                 list_ctx_dstor dstor);

    int        (*size)          (struct list_ifc_t *ifc);

    void       (*destroy)       (struct list_ifc_t *ifc, list_ctx_dstor dstor);
} list_ifc;

list_ifc *list_create(list_allocator allocator, list_deallocator deallocator);

#endif
