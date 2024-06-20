#ifndef KERNEL_MUTEX_H
#define KERNEL_MUTEX_H

#include "common/common.h"

typedef struct mutex_t {
    uint32_t handle;
} mutex;

void mutex_lock(mutex *handle);

void mutex_unlock(mutex *handle);

#endif
