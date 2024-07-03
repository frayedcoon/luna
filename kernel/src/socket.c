#include "kernel/socket.h"
#include "kernel/pipe.h"
#include "kernel/memory.h"
#include "lib/list.h"
#include "lib/string.h"
#include "common/log.h"

#define DEFAULT_BUFFER_SIZE 64

#define CONNECTION_ESTABLISHED(conn_context) ((conn_context)->client_buffer.buffer)

typedef struct socket_io_buffer_t {
    uint8_t              *wr_ptr;
    uint8_t              *rd_ptr;

    uint8_t              *buffer;

    uint32_t              buffer_size;
    uint32_t              busy_count;
} socket_io_buffer;

typedef struct socket_ctx_t {
    uint8_t               port;

    list_ifc             *connections;

    const void           *owner;
} socket_ctx;

typedef struct socket_connection_t {
    socket_io_buffer      owner_buffer;
    socket_io_buffer      client_buffer;

    const void          **reply_ptr;

    socket_ctx           *sock_ref;

    const void           *client;
} socket_connection;

static
list_ifc *socket_list = NULL;

static
int sockets_init(void) {
    if (socket_list) {
        return 0;
    }

    socket_list = list_create(cell_alloc, cell_free);
    if (!socket_list) {
        return -1;
    }

    return 0;
}

static inline
void *sock_find_port(uint8_t port, int find_node) {
    const list_node *node = socket_list->get_front(socket_list);

    socket_ctx *sock_ctx = NULL;

    while (node) {
        sock_ctx = node->ptr;

        if (sock_ctx->port == port) {
            return (find_node ? ((void*)node) : ((void*) sock_ctx));
        }

        node = node->nxt;
    }

    return NULL;
}

static inline
void *sock_find_conn(const void *dest, int find_node) {
    const list_node *sock_node = socket_list->get_front(socket_list);

    socket_ctx *sock_ctx = NULL;

    while (sock_node) {
        sock_ctx = sock_node->ptr;

        const list_node *conn_node =
            sock_ctx->connections->get_front(sock_ctx->connections);

        socket_connection *conn_ctx = NULL;

        while(conn_node) {
            conn_ctx = conn_node->ptr;

            if (dest == conn_ctx) {
                return (find_node ? ((void*)conn_node) : ((void*) conn_ctx));
            }

            conn_node = conn_node->nxt;
        }

        sock_node = sock_node->nxt;
    }

    return NULL;
}


static
socket_connection *socket_init_connection(socket_ctx *ctx, const void *client, const void **reply_ptr,
    uint32_t buffer_size) {
    uint8_t           *buf  = NULL;
    socket_connection *conn = cell_alloc(sizeof(socket_connection));
    if (!conn) {
        goto error;
    }

    memset(conn, 0, sizeof(socket_connection));

    if (!buffer_size) {
        buffer_size = DEFAULT_BUFFER_SIZE;
    }

    buf = cell_alloc(buffer_size);
    if (!buf) {
        goto error;
    }

    conn->owner_buffer.rd_ptr      = buf;
    conn->owner_buffer.wr_ptr      = buf;
    conn->owner_buffer.buffer      = buf;
    conn->owner_buffer.buffer_size = buffer_size;

    if (!ctx->connections->insert_after(ctx->connections, NULL, conn)) {
        goto error;
    }

    conn->client    = client;

    conn->sock_ref  = ctx;
    conn->reply_ptr = reply_ptr;

    return conn;

    error:

    if (buf) {
        cell_free(buf);
    }

    if (conn) {
        cell_free(conn);
    }

    return NULL;
}

void sock_conn_dstor(void *ptr) {
    socket_connection *conn = ptr;
    if (conn) {
        if (conn->owner_buffer.buffer) {
            cell_free(conn->owner_buffer.buffer);
        }
        if (conn->client_buffer.buffer) {
            cell_free(conn->client_buffer.buffer);
        }
        cell_free(conn);
    }
}

int socket_create(const void *owner, uint8_t port) {
    if (sockets_init()) {
        return -1;
    }

    socket_ctx *ctx = cell_alloc(sizeof(socket_ctx));
    if (!ctx) {
        goto error;
    }

    memset(ctx, 0, sizeof(socket_ctx));

    ctx->connections = list_create(cell_alloc, cell_free);
    if (!ctx->connections) {
        goto error;
    }

    ctx->port  = port;

    ctx->owner = owner;

    if (!socket_list->insert_after(socket_list, NULL, ctx)) {
        goto error;
    }

    return 0;

    error:

    if (ctx) {
        if (ctx->connections) {
            ctx->connections->destroy(ctx->connections, sock_conn_dstor);
        }

        cell_free(ctx);
    }

    return -2;
}

int socket_connect(uint8_t port, const void *client, const void **reply_ptr, uint32_t buffer_size) {
    if (sockets_init()) {
        return -1;
    }

    socket_ctx *ctx = sock_find_port(port, 0);
    if (!ctx) {
        return -2;
    }

    socket_connection *conn = socket_init_connection(ctx, client, reply_ptr, buffer_size);
    if (!conn) {
        return -3;
    }

    return 0;
}

const
void *socket_listen(uint8_t port, const void *owner) {
    if (sockets_init()) {
        return NULL;
    }

    socket_ctx *ctx = sock_find_port(port, 0);
    if (!ctx) {
        return NULL;
    }

    if (owner != ctx->owner) {
        return NULL;
    }

    const list_node *node = ctx->connections->get_front(ctx->connections);
    if (node) {
        socket_connection *conn = node->ptr;

        if (!CONNECTION_ESTABLISHED(conn) && *conn->reply_ptr != conn) {
            return conn->client;
        }
    }

    return NULL;
}

const
void *socket_reply(uint8_t port, const void *owner, const void *client, uint32_t buffer_size, int accept) {
    if (sockets_init()) {
        return NULL;
    }

    socket_ctx *ctx = sock_find_port(port, 0);
    if (!ctx) {
        return NULL;
    }

    if (owner != ctx->owner) {
        return NULL;
    }

    const list_node *node =
        ctx->connections->get_front(ctx->connections);

    while (node) {
        socket_connection *conn = node->ptr;

        //! unaccepted connection
        if (!CONNECTION_ESTABLISHED(conn)) {
             if (conn->client == client) {
                if (accept) {
                    if (!buffer_size) {
                        buffer_size = DEFAULT_BUFFER_SIZE;
                    }
                    uint8_t *buf = cell_alloc(buffer_size);
                    if (!buf) {
                        return NULL;
                    }
                    conn->client_buffer.rd_ptr      = buf;
                    conn->client_buffer.wr_ptr      = buf;
                    conn->client_buffer.buffer      = buf;
                    conn->client_buffer.buffer_size = buffer_size;

                    *conn->reply_ptr = conn;

                    return conn;
                } else {
                    //! declined
                    *conn->reply_ptr = NULL;
                    ctx->connections->delete(ctx->connections, node, sock_conn_dstor);
                }

                return NULL;
            }
        }

        node = node->nxt;
    }

    return NULL;
}

const
void *socket_select(uint8_t port, const void *owner) {
    if (sockets_init()) {
        return NULL;
    }

    socket_ctx *ctx = sock_find_port(port, 0);
    if (!ctx) {
        return NULL;
    }

    if (owner != ctx->owner) {
        return NULL;
    }

    const list_node *node =
        ctx->connections->get_front(ctx->connections);

    while (node) {
        socket_connection *conn = node->ptr;

        //! accepted connection
        if (CONNECTION_ESTABLISHED(conn)) {
            if (conn->owner_buffer.busy_count > 0) {
                return conn;
            }
        }

        node = node->nxt;
    }

    return NULL;
}

int socket_write(const void * dest, const void *owner, char *buffer, uint32_t buffer_size) {
    if (sockets_init()) {
        return -1;
    }

    socket_connection *conn = sock_find_conn(dest, 0);
    if (!conn) {
        return -2;
    }

    if (!CONNECTION_ESTABLISHED(conn)) {
        return -3;
    }

    socket_ctx *sock_ctx = conn->sock_ref;

    socket_io_buffer *buf_ctx = NULL;

    uint8_t          *buf     = NULL;

    if (conn->client == owner) {
        buf_ctx = (&conn->owner_buffer);

        buf = buf_ctx->buffer;
    } else if (sock_ctx->owner == owner) {
        buf_ctx = (&conn->client_buffer);

        buf = buf_ctx->buffer;
    } else {
        return -4;
    }

    if (buf_ctx->busy_count + buffer_size > buf_ctx->buffer_size) {
        buffer_size = buf_ctx->buffer_size - buf_ctx->busy_count;
    }

    uint32_t count = 0;

    while (count < buffer_size) {
        *buf_ctx->wr_ptr++ = buffer[count++];
        buf_ctx->busy_count++;

        if (buf_ctx->wr_ptr >= (buf + buf_ctx->buffer_size)) {
            buf_ctx->wr_ptr = buf;
        }
    }

    return count;
}

int socket_read(const void * dest, const void *owner, char *buffer, uint32_t buffer_size) {
    if (sockets_init()) {
        return -1;
    }

    socket_connection *conn = sock_find_conn(dest, 0);
    if (!conn) {
        return -2;
    }

    if (!CONNECTION_ESTABLISHED(conn)) {
        return -3;
    }

    socket_ctx *sock_ctx = conn->sock_ref;

    socket_io_buffer *buf_ctx = NULL;

    uint8_t          *buf     = NULL;

    if (conn->client == owner) {
        buf_ctx = (&conn->client_buffer);

        buf = buf_ctx->buffer;
    } else if (sock_ctx->owner == owner) {
        buf_ctx = (&conn->owner_buffer);

        buf = buf_ctx->buffer;
    } else {
        return -4;
    }

    uint32_t count = 0;

    while (count < buffer_size) {
        if (!buf_ctx->busy_count) {
            break;
        }

        buffer[count++] = *buf_ctx->rd_ptr++;
        buf_ctx->busy_count--;

        if (buf_ctx->rd_ptr >= (buf + buf_ctx->buffer_size)) {
            buf_ctx->rd_ptr = buf;
        }
    }

    return count;
}

const
void *socket_get_peer(const void *dest, const void *source) {
    if (sockets_init()) {
        return NULL;
    }

    socket_connection *conn = sock_find_conn(dest, 0);
    if (conn) {
        if (!CONNECTION_ESTABLISHED(conn)) {
            return NULL;
        }
        socket_ctx *sock = conn->sock_ref;

        if (source == conn->client) {
            return sock->owner;
        }

        if (source == sock->owner) {
            return conn->client;
        }
    }

    return NULL;
}

int socket_close(const void *dest, const void *source) {
    if (sockets_init()) {
        return -1;
    }

    if (is_sock(dest)) {
        socket_ctx *sock = (void *) dest;

        if (sock->owner != source) {
            return -2;
        }

        sock->connections->destroy(sock->connections, sock_conn_dstor);

        cell_free(sock);

        return 0;
    }

    if (is_sock_conn(dest)) {
        const list_node *conn_node = sock_find_conn(dest, 1);
        if (conn_node) {
            socket_connection *conn = conn_node->ptr;
            socket_ctx *sock = conn->sock_ref;

            if (sock->owner != source && conn->client != source) {
                return -3;
            }

            sock->connections->delete(sock->connections, conn_node, sock_conn_dstor);

            return 0;
        }
    }

    return -4;
}

int is_sock(const void *sock) {
    if (sockets_init()) {
        return -1;
    }

    const list_node *node = socket_list->get_front(socket_list);

    socket_ctx *sock_ctx = NULL;

    while (node) {
        sock_ctx = node->ptr;

        if (sock_ctx == sock) {
            return 1;
        }

        node = node->nxt;
    }

    return 0;
}

int is_sock_conn(const void *conn) {
    if (sockets_init()) {
        return -1;
    }

    return ((int) sock_find_conn(conn, 0));
}
