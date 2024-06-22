#ifndef KERNEL_SYSCALL_H
#define KERNEL_SYSCALL_H

#include "arch/svc.h"

typedef enum service_ports_t {
    VFS_PORT = 0x00,
    LED_PORT = 0x01,
} service_ports;

void yield(void);

void wait(const void *object);

void signal(const void *object);

void sleep(int msec);

void *malloc(uint32_t size);

void *zmalloc(uint32_t size);

void *realloc(void *ptr, uint32_t size);

void free(void *ptr);

const
void *create_thread(void (*task)(), const char *name, uint8_t priority);

const
void *get_thread(const char *name);

int destroy_thread(const void * thread);

const
void *popen(const void *dest_thread, uint8_t *buffer, uint32_t buffer_size);

const
void *precv(void);

int write(const void * dest, const void *data, uint32_t data_size);

int read(const void * dest, void *data, uint32_t data_size);

const
void *peer(const void *dest);

int close(const void * dest);

int socket(uint8_t port);

const
void *connect(uint8_t port, uint32_t buffer_size);

const
void *listen(uint8_t port);

const
void *accept(uint8_t port, const void *client, uint32_t buffer_size);

const
void *decline(uint8_t port, const void *client);

const
void *select(uint8_t port);

#endif
