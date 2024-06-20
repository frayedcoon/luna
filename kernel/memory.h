#ifndef KERNEL_MEMORY_H
#define KERNEL_MEMORY_H

#include "common/common.h"
#include "common/def.h"

#define HEAP_HDR_SIZE sizeof(heap_meta)
#define CELL_HDR_SIZE sizeof(cell)

#define CELL_START ((cell*) (&HEAP_START + HEAP_HDR_SIZE))
#define CELL_REUSE_MAX_OVERHEAD 2
#define HEAP_ALIGNMENT 4

#define ALIGN_MEM(mem) ((mem % HEAP_ALIGNMENT == 0) ? mem :\
    mem + (HEAP_ALIGNMENT - (mem % HEAP_ALIGNMENT)))

#define alloca(x) __builtin_alloca(x)

/**
 * @brief      allocated/freed cell header
 */
typedef struct cell_t {
    /**
     * pointer to next cell
     */
    struct cell_t *ptr;
    /**
     * cell data size in bytes (without header)
     */
    uint16_t       size;
    /**
     * inditates if cell is used or freed
     */
    uint16_t       used;
} __attribute__((packed,aligned(4))) cell;

/**
 * @brief      heap header
 */
typedef struct heap_meta_t {
    /**
     * pointer to free memory at the end of heap
     */
    cell *free_cell;
    /**
     * start of list with used/freed cells
     */
    cell *used_start;
} __attribute__((packed,aligned(4))) heap_meta;

/**
 * @brief      general heap initialization
 * NOTE: MUST be called before any heap allocation
 */
void heap_init(void);

/**
 * @brief      allocated new memory cell
 *
 * @param      size  number of bytes to be allocated
 *
 * @return     pointer to allocated cell data
 */
void *cell_alloc(const uint16_t size);

/**
 * @brief      move allocated memory
 *
 * @param      ptr   allocated cell data
 * @param      size  number of bytes to be allocated at new location
 *
 * @return     pointer to allocated cell data
 */
void *cell_realloc(void* ptr, const uint16_t size);

/**
 * @brief      deallocate cell
 *
 * @param      ptr   allocated cell data
 */
void cell_free(void* ptr);

#endif
