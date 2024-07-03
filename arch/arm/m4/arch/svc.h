#ifndef ARCH_SVC_H
#define ARCH_SVC_H

#include "common/common.h"
#include "common/def.h"

/**
 * supervisor call codes enumeration
 */
typedef enum sv_code_t {
    /**
     * switch to user mode
     */
    SVC_USER_MODE               = 0x00,
    /**
     * allocate memory
     */
    SVC_MEMORY                  = 0x01,
    /**
     * call thread scheduler
     */
    SVC_RESCHEDULE              = 0x02,
    /**
     * create thread
     */
    SVC_THREAD_CREATE           = 0x03,
    /**
     * get thread pid
     */
    SVC_THREAD_PID              = 0x04,
    /**
     * get thread pid
     */
    SVC_THREAD_DESTROY          = 0x05,
    /**
     * create pipe
     */
    SVC_PIPE_OPEN               = 0x06,
    /**
     * get ready pipe
     */
    SVC_PIPE_RECV               = 0x07,
    /**
     * write to descriptor
     */
    SVC_WRITE                   = 0x08,
    /**
     * read from descriptor
     */
    SVC_READ                    = 0x09,
     /**
     * get socket peer
     */
    SVC_PEER                    = 0x0a,
    /**
     * close descriptor
     */
    SVC_CLOSE                   = 0x0b,
    /**
     * create socket
     */
    SVC_SOCK_OPEN               = 0x0c,
    /**
     * send connect to socket
     */
    SVC_SOCK_CONN               = 0x0d,
    /**
     * listen connect requests on socket
     */
    SVC_SOCK_LISTEN             = 0x0e,
    /**
     * reply connect requset on socket
     */
    SVC_SOCK_REPLY              = 0x0f,
    /**
     * get pending client descriptor on socket
     */
    SVC_SOCK_SELECT             = 0x10,
    /**
     * lock thread until signal
     */
    SVC_WAIT                    = 0x11,
    /**
     * signal thread to unlock
     */
    SVC_WAKEUP                  = 0x12,
} sv_code;

typedef struct memory_request_t {
    /**
     * desired memory size
     */
    uint32_t               size;
    /**
     * memory ptr (output)
     */
    void                  *ptr;
} memory_request;

typedef struct thread_sleep_request_t {
    uint32_t               msec;
} thread_sleep_request;

typedef struct thread_wait_request_t {
    const void            *object;
} thread_wait_request;

typedef struct thread_signal_request_t {
    const void            *object;
} thread_signal_request;

typedef struct thread_create_request_t {
    const void            *thread;
    /**
     * thread's name
     */
    const char            *name;
    /**
     * thread routine handler
     */
    void                  (*task)();
    /**
     * thread priority
     */
    uint8_t                priority;
} thread_create_request;

typedef struct thread_get_request_t {
    const void            *thread;
    /**
     * thread's name
     */
    const char            *name;
} thread_get_request;

typedef struct thread_destroy_request_t {
    int                    result;
    const void            *thread;
} thread_destroy_request;

typedef struct pipe_create_request_t {
    const void            *pipe;
    /**
     * destination thread
     */
    const void            *dest_thread;
     /**
     * pipe buffer ptr
     */
    uint8_t               *buffer;
    /**
     * pipe buffer size
     */
    uint32_t               buffer_size;
} pipe_create_request;

typedef struct pipe_recv_request_t {
    const void            *pipe;
} pipe_recv_request;

typedef struct rw_request_t {
    int                    result;
    /**
     * descriptor
     */
    const void            *dest;
    /**
     * buffer to write/read to
     */
    char*                  data;
    /**
     * buffer size
     */
    uint32_t               data_size;
} rw_request;

typedef struct socket_peer_request_t {
    const void            *peer;
    const void            *sock;
} socket_peer_request;

typedef struct socket_close_request_t {
    int                    result;
    const void            *sock;
} socket_close_request;

typedef struct socket_open_request_t {
    int                    result;
    /**
     * socket port
     */
    uint8_t                port;
} socket_open_request;

typedef struct socket_connect_request_t {
    int                    result;
    /**
     * target port
     */
    uint8_t                port;
    uint32_t               buffer_size;
    /**
     * connection descriptor/error (output)
     */
    const void           **reply_ptr;
} socket_connect_request;

typedef struct socket_port_request_t {
    const void            *client;
    /**
     * socket port
     */
    uint8_t                port;
} socket_port_request;

typedef struct socket_reply_request_t {
    const void            *conn;
    /**
     * socket descriptor
     */
    uint8_t                port;
    /**
     * requester
     */
    const void            *client;
    uint32_t               buffer_size;
    uint8_t                accept;
} socket_reply_request;

/**
 * @brief      performs SV Call with given parameter
 *
 * @param[in]  svc_number  number of supervisor function to call
 *
 * @param[in]  arg         sv call argument
 *
 * @return     sv call error code
 */
inline __attribute__((always_inline)) void sv_call(sv_code svc_number, void *arg) {
    register uint32_t svc_ret  asm ("r0") = (uint32_t) svc_number;
    register void *svc_arg  asm ("r1") = arg;
    asm volatile (
        "svc 0                               \n\t"
        : [svc] "=r" (svc_ret), [sva] "=r" (svc_arg)
    );
}

/**
 * @brief      SV Call exception handler. Performs swith to user mode
 *
 * UIBC
 *
 */
void sv_call_handler(uint32_t  svc_code, void *svc_arg);

/**
 * @brief      pends execution of pend_sv_handler
 */
void pend_sv_call(void);

#endif
