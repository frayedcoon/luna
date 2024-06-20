#ifndef KERNEL_PIPE_H
#define KERNEL_PIPE_H

#include "common/def.h"
#include "common/common.h"

const
void *pipe_create(const void *source, const void *dest, uint8_t *buffer, uint32_t buffer_size);

int pipe_write(const void *pipe, const void *source, char *buffer, uint32_t buffer_size);

int pipe_read(const void *pipe, const void *source, char *buffer, uint32_t buffer_size);

int pipe_close(const void *pipe, const void *source);

const
void *pipe_get_ready(const void *source);

int is_pipe(const void *pipe);

#endif
