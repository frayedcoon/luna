#include "kernel/memory.h"

/**
 * heap header
 */
heap_meta *heap_hdr;

/**
 * @brief      find best-fit freed cell
 *
 * @param      size  desired size in bytes
 *
 * @return     suitable freed cell
 */
static cell *cell_find_in_used(const uint16_t size) {
    cell *cur_cell = heap_hdr->used_start;
    cell *best_cell = NULL;
    //! going throught list of cells
    while (cur_cell) {
        //! suitable not busy cell found
        if (!cur_cell->used && cur_cell->size >= size &&
            size * CELL_REUSE_MAX_OVERHEAD >= cur_cell->size) {
            //! first suitable cell found
            if (!best_cell) {
                best_cell = cur_cell;
            //! better cell found
            } else if (cur_cell->size < best_cell->size) {
                best_cell = cur_cell;
            }
        }
        cur_cell = cur_cell->ptr;
    }
    //! found suitable freed cell or NULL
    return best_cell ? best_cell : NULL;
}

/**
 * @brief      get last allocated cell
 *
 * @return     pointer to last cell
 */
static cell *cell_last(void) {
    cell *cur_cell = heap_hdr->used_start;
    //! returning if nothing has been allocated yet
    if (!cur_cell) {
        return NULL;
    }
    //! going throught list of cells
    while (cur_cell->ptr) {
        cur_cell = cur_cell->ptr;
    }
    //! last used cell header
    return cur_cell;
}

/**
 * @brief      delete last freed cell
 */
static void cell_delete_last_free(void) {
    cell *curr_cell = heap_hdr->used_start;
    cell *prev_cell;
    //!reinitialize heap if the first allocated cell marked as free
    if (!curr_cell->ptr && !curr_cell->used) {
        heap_init();
        return;
    } else {
        while (curr_cell->ptr) {
            prev_cell = curr_cell;
            curr_cell = curr_cell->ptr;
        }
        if (!curr_cell->used) {
            prev_cell->ptr = NULL;
            heap_hdr->free_cell = curr_cell;
        }
    }
}

/**
 * @brief      delete freed cells at the end of used list
 */
static void cell_drop_free_trailing(void) {
    while (!(cell_last()->used) && heap_hdr->used_start) {
        cell_delete_last_free();
    }
}

void heap_init(void) {
    //! write heap header
    heap_hdr = (heap_meta*) &HEAP_START;
    //! set the first cell to be allocated
    heap_hdr->free_cell = CELL_START;
    //! clear list of allocated cells
    heap_hdr->used_start = NULL;
}

void *cell_alloc(const uint16_t size) {
    //! look for suitable freed cell
    cell *cur_cell = cell_find_in_used(size);
    //! total cell size to be allocated
    const uint16_t req_space = ALIGN_MEM(size) + CELL_HDR_SIZE;
    //! found suitable freed cell
    if (cur_cell) {
        cur_cell->used = 1;
    //! need to allocate at the end
    } else {
        cell *prv_cell = cell_last();
        //! fisrt allocation
        if (!prv_cell) {
            cur_cell = heap_hdr->free_cell;
            heap_hdr->used_start = cur_cell;
        //! common allocation at the end of heap
        } else {
            //! no memory
            if ((cell*) ((uint8_t*) heap_hdr->free_cell + req_space) >
                (cell*) &STACK_END) {
                return NULL;
            }
            prv_cell->ptr = heap_hdr->free_cell;
            cur_cell = prv_cell->ptr;
        }
        heap_hdr->free_cell =
            (cell*) ((uint8_t*) heap_hdr->free_cell + req_space);
        //! setup new cell header
        cur_cell->ptr = NULL;
        cur_cell->used = 1;
        cur_cell->size = req_space - CELL_HDR_SIZE;
    }
    //! returning pointer to the end of cell header
    return cur_cell + 1;
}

void *cell_realloc(void* ptr, const uint16_t size) {
    //! get cell header
    cell *cur_cell = (cell*) ptr;
    cur_cell--;
    //! returning if reallocating fewer memory
    if (cur_cell->size > size) {
        return ptr;
    }
    //! allocating new cell
    uint8_t *new_data = cell_alloc(size);

    if (new_data) {
        //! copying data to new location
        for (uint8_t i = 0; i < size; ++i) {
            *(new_data + i) = *((uint8_t*) ptr + i);
        }
        //! free old cell
        cell_free(ptr);
    }
    return new_data;
}

void cell_free(void* ptr) {
    if (!ptr) {
        return;
    }
    //! get cell header
    cell *cur_cell = (cell*) ptr - 1;
    //! mark cell as not used
    cur_cell->used = 0;
    //! delete freed cell at the end of list
    cell_drop_free_trailing();
}
