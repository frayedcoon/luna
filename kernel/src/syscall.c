#include "kernel/syscall.h"
#include "kernel/thread.h"
#include "common/sys.h"
#include "lib/string.h"

#define CONNECT_TIMEOUT_MS 500

void sleep(int msec) {
    thread_delay(msec);
    sv_call(SVC_RESCHEDULE, NULL);
}

void yield(void) {
    thread_yield();
    sv_call(SVC_RESCHEDULE, NULL);
}

void wait(const void *object) {
    thread_wait_request req = {
        .object        = object,
    };

    sv_call(SVC_WAIT, &req);
}

void signal(const void * object) {
    thread_signal_request req = {
        .object        = object,
    };

    sv_call(SVC_WAKEUP, &req);
}

void *malloc(uint32_t size) {
    memory_request req = {
        .size       = size,
        .ptr        = NULL,
    };

    sv_call(SVC_MEMORY, &req);

    return req.ptr;
}

void *zmalloc(uint32_t size) {
    memory_request req = {
        .size       = size,
        .ptr        = NULL,
    };

    sv_call(SVC_MEMORY, &req);

    if (req.ptr && req.size) {
        __builtin_bzero(req.ptr, req.size);
    }

    return req.ptr;
}

void *realloc(void *ptr, uint32_t size) {
    memory_request req = {
        .size       = size,
        .ptr        = ptr,
    };

    sv_call(SVC_MEMORY, &req);

    return req.ptr;
}

void free(void *ptr) {
    memory_request req = {
        .size       = 0,
        .ptr        = ptr,
    };

    sv_call(SVC_MEMORY, &req);
}

const
void *create_thread(void (*task)(), const char *name, uint8_t priority) {
    thread_create_request req = {
        .name          = name,
        .task          = task,
        .priority      = priority,
        .thread        = NULL,
    };

    sv_call(SVC_THREAD_CREATE, &req);

    return req.thread;
}

const
void *get_thread(const char *name) {
    thread_get_request req = {
        .name          = name,
        .thread        = NULL,
    };

    sv_call(SVC_THREAD_PID, &req);

    return req.thread;
}

int destroy_thread(const void * thread) {
    thread_destroy_request req = {
        .thread        = thread,
        .result        = -21,
    };

    sv_call(SVC_THREAD_DESTROY, &req);

    return req.result;
}

const
void *popen(const void *dest_thread, uint8_t *buffer, uint32_t buffer_size) {
    pipe_create_request req = {
        .pipe           = NULL,
        .dest_thread    = dest_thread,
        .buffer         = buffer,
        .buffer_size    = buffer_size,
    };

    sv_call(SVC_PIPE_OPEN, &req);

    return req.pipe;
}

const
void *precv(void) {
    pipe_recv_request req = {
        .pipe          = NULL,
    };

    sv_call(SVC_PIPE_RECV, &req);

    return req.pipe;
}

int write(const void * dest, char *data, uint32_t data_size) {
    rw_request req = {
        .result    = 0,
        .dest      = dest,
        .data      = data,
        .data_size = data_size,
    };

    sv_call(SVC_WRITE, &req);

    return req.result;
}

int read(const void * dest, char *data, uint32_t data_size) {
    rw_request req = {
        .result    = 0,
        .dest      = dest,
        .data      = data,
        .data_size = data_size,
    };

    sv_call(SVC_READ, &req);

    return req.result;
}

const
void *peer(const void *dest) {
    socket_peer_request req = {
        .peer      = NULL,
        .sock      = dest,
    };

    sv_call(SVC_PEER, &req);

    return req.peer;
}

int close(const void * dest) {
    socket_close_request req = {
        .result    = -21,
        .sock      = dest,
    };

    sv_call(SVC_CLOSE, &req);

    return req.result;
}

int socket(uint8_t port) {
    socket_open_request req = {
        .result             = -21,
        .port               = port,
    };

    sv_call(SVC_SOCK_OPEN, &req);

    return req.result;
}

const
void *connect(uint8_t port, uint32_t buffer_size) {
    void * volatile reply = connect;

    socket_connect_request req = {
        .result                = -21,
        .port                  = port,
        .buffer_size           = buffer_size,
        .reply_ptr             = (const void **) &reply,
    };

    sv_call(SVC_SOCK_CONN, &req);
    if (req.result) {
        return NULL;
    }

    void *ret = expect_answer(&reply, connect, CONNECT_TIMEOUT_MS);

    return ret;
}

const
void *listen(uint8_t port) {
    socket_port_request req = {
        .client    = NULL,
        .port      = port,
    };

    sv_call(SVC_SOCK_LISTEN, &req);

    return req.client;
}

const
void *accept(uint8_t port, const void *client, uint32_t buffer_size) {
    socket_reply_request req = {
        .conn                = NULL,
        .port                = port,
        .client              = client,
        .buffer_size         = buffer_size,
        .accept              = 1,
    };

    sv_call(SVC_SOCK_REPLY, &req);

    return req.conn;
}

const
void *decline(uint8_t port, const void *client) {
    socket_reply_request req = {
        .conn                = NULL,
        .port                = port,
        .client              = client,
        .buffer_size         = 0,
        .accept              = 0,
    };

    sv_call(SVC_SOCK_REPLY, &req);

    return req.conn;
}

const
void *select(uint8_t port) {
    socket_port_request req = {
        .client    = NULL,
        .port      = port,
    };

    sv_call(SVC_SOCK_SELECT, &req);

    return req.client;
}
