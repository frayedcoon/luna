#include "kernel/pipe.h"
#include "kernel/memory.h"
#include "lib/list.h"
#include "common/log.h"

typedef struct pipe_buffer_t {
    uint32_t              buffer_size;

    uint8_t              *wr_ptr;
    uint8_t              *rd_ptr;
    uint32_t              busy_count;

    uint8_t              *buffer;
} pipe_buffer;

typedef struct pipe_ctx_t {
    const void           *source;
    const void           *dest;

    pipe_buffer           buf_ctx;
} pipe_ctx;

static
list_ifc *pipe_list = NULL;

static
int pipes_init(void) {
    if (pipe_list) {
        return -1;
    }

    pipe_list = list_create(cell_alloc, cell_free);
    if (!pipe_list) {
        return -2;
    }

    return 0;
}

static inline
void *pipe_find(const void *pipe, int find_node) {
    if (!pipe_list && pipes_init()) {
        return NULL;
    }

    const list_node *node = pipe_list->get_front(pipe_list);

    pipe_ctx *pipe_ctx = NULL;

    while (node) {
        pipe_ctx = node->ptr;

        if (pipe_ctx == pipe) {
            return (find_node ? ((void*)node) : ((void*) pipe_ctx));
        }

        node = node->nxt;
    }

    return NULL;
}

const
void *pipe_create(const void *source, const void *dest, uint8_t *buffer, uint32_t buffer_size) {
    if (!pipe_list && pipes_init()) {
        return NULL;
    }

    if (!buffer || !buffer_size) {
        return NULL;
    }

    pipe_ctx *ctx = cell_alloc(sizeof(pipe_ctx));
    if (!ctx) {
        return NULL;
    }

    memset(ctx, 0, sizeof(pipe_ctx));

    ctx->buf_ctx.buffer_size  = buffer_size;

    ctx->buf_ctx.buffer       = buffer;
    ctx->buf_ctx.wr_ptr       = buffer;
    ctx->buf_ctx.rd_ptr       = buffer;

    ctx->source               = source;
    ctx->dest                 = dest;

    if (!pipe_list->insert_after(pipe_list, NULL, ctx)) {
        cell_free(ctx);
        return NULL;
    }

    return ctx;
}

int pipe_write(const void *pipe, const void *source, char *buffer, uint32_t buffer_size) {
    if (!pipe_list && pipes_init()) {
        return -1;
    }

    if (!buffer_size) {
        return -1;
    }

    if (!is_pipe(pipe)) {
        return -1;
    }

    pipe_ctx *sock_ctx = (void *) pipe;
    if (!sock_ctx) {
        return -2;
    }

    if (sock_ctx->source != source) {
        return -3;
    }

    pipe_buffer *buf = &sock_ctx->buf_ctx;

    if (buf->busy_count + buffer_size > buf->buffer_size) {
        return -4;
    }

    uint32_t count = 0;

    while (count < buffer_size) {
        *buf->wr_ptr++ = buffer[count++];
        buf->busy_count++;

        if (buf->wr_ptr >= (buf->buffer + buf->buffer_size)) {
            buf->wr_ptr = buf->buffer;
        }
    }

    return count;
}

int pipe_read(const void *pipe, const void *source, char *buffer, uint32_t buffer_size) {
    if (!pipe_list && pipes_init()) {
        return -1;
    }

    if (!is_pipe(pipe)) {
        return -1;
    }

    pipe_ctx *sock_ctx = (void *) pipe;
    if (!sock_ctx) {
        return -2;
    }

    if (sock_ctx->dest != source) {
        return -3;
    }

    pipe_buffer *buf = &sock_ctx->buf_ctx;

    uint32_t count = 0;

    while (count < buffer_size) {
        if (!buf->busy_count) {
            break;
        }

        buffer[count++] = *buf->rd_ptr++;
        buf->busy_count--;

        if (buf->rd_ptr >= (buf->buffer + buf->buffer_size)) {
            buf->rd_ptr = buf->buffer;
        }
    }

    return count;
}

int pipe_close(const void *pipe, const void *source) {
    if (!pipe_list && pipes_init()) {
        return -1;
    }

    if (!is_pipe(pipe)) {
        return -1;
    }

    pipe_ctx *sock_ctx = (void *) pipe;
    if (!sock_ctx) {
        return -2;
    }

    if (sock_ctx->source != source && sock_ctx->dest != source) {
        return -3;
    }

    pipe_list->delete(pipe_list, pipe_find(pipe, 1), cell_free);

    return 0;
}

const
void *pipe_get_ready(const void *source) {
    if (!pipe_list) {
        return NULL;
    }

    const list_node *node = pipe_list->get_front(pipe_list);

    pipe_ctx *sock_ctx = NULL;

    while (node) {
        sock_ctx = node->ptr;

        if (sock_ctx->dest == source && sock_ctx->buf_ctx.busy_count > 0) {
            return sock_ctx;
        }

        node = node->nxt;
    }

    return NULL;
}

int is_pipe(const void *pipe) {
    if (pipe_find(pipe, 0)) {
        return 1;
    }

    return 0;
}
