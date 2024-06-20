#ifndef ARCH_MUTEX_H
#define ARCH_MUTEX_H

typedef void (*mutex_hook)(const void *handle);

void arch_mutex_lock(const void *mutex_handle, mutex_hook wait, const void *mutex_var);

void arch_mutex_unlock(const void *mutex_handle, mutex_hook signal, const void *mutex_var);

#endif
