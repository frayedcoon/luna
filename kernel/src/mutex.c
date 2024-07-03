#include "kernel/mutex.h"
#include "arch/mutex.h"
#include "lib/syscall.h"

void mutex_lock(struct mutex_t *handle) {
    arch_mutex_lock(&handle->handle, wait, handle);
}

void mutex_unlock(struct mutex_t *handle) {
    arch_mutex_unlock(&handle->handle, signal, handle);
}
