#include "arch/svc.h"
#include "arch/core.h"
#include "arch/context.h"
#include "lib/string.h"
#include "kernel/pipe.h"
#include "kernel/memory.h"
#include "kernel/socket.h"
#include "kernel/thread.h"

extern core_context  *core_context_cur;
extern core_context  *core_context_new;

extern scheduler_ifc *scheduler;

extern core_context   idler_core_context;
extern core_context   dummy_core_context;

volatile int context_switch_scheduled = 0;

/**
 * @brief      schedule and switch new context (if needed)
 *
 * @param[in]  arg         not used
 */
void sv_schedule_routine(void *arg) {
    (void) arg;
    core_context *old_ctx = NULL;
    core_context *new_ctx = NULL;

    if (!context_switch_scheduled) {
        context_switch_scheduled = 1;

        scheduler->schedule(&old_ctx, &new_ctx);

        core_context_cur = old_ctx ? old_ctx : &dummy_core_context;
        core_context_new = new_ctx ? new_ctx : &idler_core_context;

        pend_sv_call();
    }
}

/**
 * @brief      switch to user mode
 */
static
void sv_user_mode(void *arg) {
    (void) arg;
    register void *user_stack asm ("r0") = idler_core_context.sp;
    asm volatile (
        //! update PSP from current user thread's SP
        "msr      psp, r0                    \n\t"
        //! can't update CONTROL register directly: using intermedite register
        "mov      r0, $0x03                  \n\t"
        //! CONTROL BITS:
        //! BIT_0 -> on  :  unprivileged mode
        //! BIT_1 -> on  :  pusing PSP
        //! BIT_2 -> off :  not using FPU context
        "msr      control, r0                \n\t"
        //! EXC_RETURN : thread mode with PSP
        "ldr      lr, =0xFFFFFFFD            \n\t"
        //! branch to new mode
        "bx       lr                         \n\t"
    );

    (void) user_stack;
}

static
void sv_memory(void *arg) {
    memory_request *req = (memory_request*) arg;
    if (!req) {
        return;
    }

    if (!req->ptr) {
        req->ptr = cell_alloc(req->size);
    } else if (!req->size) {
        cell_free(req->ptr);
    } else {
        req->ptr = cell_realloc(req->ptr, req->size);
    }
}

static
void sv_sleep(void *arg) {
    thread_sleep_request *req = (thread_sleep_request *) arg;
    if (!req) {
        return;
    }

    thread_delay(req->msec);

    sv_schedule_routine(NULL);
}

static
void sv_thread_create(void *arg) {
    thread_create_request *req = (thread_create_request*) arg;
    if (!req) {
        return;
    }

    req->thread = thread_create(req->task, req->name, req->priority);
}


/**
 * @brief      get thread's pid
 *
 * @param[in]  arg         thread's name or NULL (get own pid)
 */
static
void sv_thread_get(void *arg) {
    thread_get_request *req = (thread_get_request*) arg;
    if (!req) {
        return;
    }

    req->thread = thread_get(arg);
}

static
void sv_thread_destroy(void *arg) {
    thread_destroy_request *req = (thread_destroy_request*) arg;
    if (!req) {
        return;
    }

    if (is_thread(req->thread)) {
        const thread_ctx *thread = req->thread;

        thread_destroy(thread);

        req->result = 0;
    } else {
        req->result = -1;
    }
}

static
void sv_pipe_create(void *arg) {
    pipe_create_request *req = (pipe_create_request*) arg;
    if (!req) {
        return;
    }

    req->pipe = pipe_create(thread_get(NULL), req->dest_thread, req->buffer, req->buffer_size);
}

static
void sv_pipe_recv(void *arg) {
    pipe_recv_request *req = (pipe_recv_request*) arg;
    if (!req) {
        return;
    }

    req->pipe = pipe_get_ready(thread_get(NULL));
}

static
void sv_write(void *arg) {
    rw_request *req = (rw_request*) arg;
    if (!req) {
        return;
    }

    if (is_pipe(req->dest)) {
        req->result = pipe_write(req->dest, thread_get(NULL), req->data, req->data_size);
    } else if (is_sock(req->dest) || is_sock_conn(req->dest)) {
        req->result = socket_write(req->dest, thread_get(NULL), req->data, req->data_size);
    } else {
        req->result = -7;
    }
}

static
void sv_read(void *arg) {
    rw_request *req = (rw_request*) arg;
    if (!req) {
        return;
    }

    if (is_pipe(req->dest)) {
        req->result = pipe_read(req->dest, thread_get(NULL), req->data, req->data_size);
    } else if (is_sock_conn(req->dest)) {
        req->result = socket_read(req->dest, thread_get(NULL), req->data, req->data_size);
    } else {
        req->result = -7;
    }
}

static
void sv_peer(void *arg) {
    socket_peer_request *req = (socket_peer_request*) arg;
    if (!req) {
        return;
    }

    req->peer = NULL;

    if (is_sock_conn(req->sock)) {
        req->peer = socket_get_peer(req->sock, thread_get(NULL));
    }
}

static
void sv_close(void *arg) {
    socket_close_request *req = (socket_close_request*) arg;
    if (!req) {
        return;
    }

    if (is_pipe(req->sock)) {
        req->result = pipe_close(req->sock, thread_get(NULL));
    } else if (is_sock(req->sock) || is_sock_conn(req->sock)) {
        req->result = socket_close(req->sock, thread_get(NULL));
    } else {
        req->result = -7;
    }
}

static
void sv_sock_open(void *arg) {
    socket_open_request *req = (socket_open_request*) arg;
    if (!req) {
        return;
    }

    req->result = socket_create(thread_get(NULL), req->port);
}

static
void sv_sock_conn(void *arg) {
    socket_connect_request *req = (socket_connect_request*) arg;
    if (!req) {
        return;
    }

    req->result = socket_connect(req->port, thread_get(NULL), req->reply_ptr, req->buffer_size);
}

static
void sv_sock_listen(void *arg) {
    socket_port_request *req = (socket_port_request*) arg;
    if (!req) {
        return;
    }

    req->client = socket_listen(req->port, thread_get(NULL));
}

static
void sv_sock_reply(void *arg) {
    socket_reply_request *req = (socket_reply_request*) arg;
    if (!req) {
        return;
    }

    req->conn = socket_reply(req->port, thread_get(NULL), req->client, req->buffer_size, req->accept);
}

static
void sv_sock_select(void *arg) {
    socket_port_request *req = (socket_port_request*) arg;
    if (!req) {
        return;
    }

    req->client = socket_select(req->port, thread_get(NULL));
}

static
void sv_wait(void *arg) {
    thread_wait_request *req = (thread_wait_request*) arg;
    if (req) {
        thread_suspend(thread_get(NULL), req->object);

        sv_schedule_routine(NULL);
    }
}

static
void sv_signal(void *arg) {
    thread_signal_request *req = (thread_signal_request*) arg;
    if (req && req->object) {
        thread_signal(req->object);
        sv_schedule_routine(NULL);
    }
}

void (*svc_handlers[])(void*) = {
    sv_user_mode,
    sv_memory,
    sv_sleep,
    sv_thread_create,
    sv_thread_get,
    sv_thread_destroy,
    sv_pipe_create,
    sv_pipe_recv,
    sv_write,
    sv_read,
    sv_peer,
    sv_close,
    sv_sock_open,
    sv_sock_conn,
    sv_sock_listen,
    sv_sock_reply,
    sv_sock_select,
    sv_wait,
    sv_signal,
};

void sv_call_handler(uint32_t svc_code, void *svc_arg) {
    if (svc_code < countof(svc_handlers)) {
        svc_handlers[svc_code](svc_arg);
    }
}

void pend_sv_call(void) {
    ICSR |= BIT28;
}
