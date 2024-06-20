#ifndef KERNEL_SOCKET_H
#define KERNEL_SOCKET_H

#include "common/def.h"
#include "common/common.h"

#define SOCKET_MAX_NAME_LENGTH 32

int socket_create(const void *owner, uint8_t port);

int socket_connect(uint8_t port, const void *owner, const void **reply_ptr, uint32_t buffer_size);

const
void *socket_listen(uint8_t port, const void *owner);

const
void *socket_reply(uint8_t port, const void * owner, const void *client, uint32_t buffer_size, int accept);

const
void *socket_select(uint8_t port, const void *owner);

int socket_write(const void *dest, const void *owner, char * buffer, uint32_t buf_size);

int socket_read(const void *dest, const void *owner, char * buffer, uint32_t buf_size);

const
void *socket_get_peer(const void *dest, const void *source);

int socket_close(const void *dest, const void *source);

int is_sock(const void * sock);

int is_sock_conn(const void * conn);

#endif
