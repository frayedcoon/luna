#ifndef COMMON_SYS
#define COMMON_SYS

#include "common/common.h"

int timed_read(const void *dest, void *data, uint32_t data_size, uint32_t ms);

void *expect_answer(void * volatile * ptr, void *init_value, uint32_t timeout);

#endif
