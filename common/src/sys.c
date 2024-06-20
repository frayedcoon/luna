#include "common/sys.h"
#include "kernel/syscall.h"
#include "common/log.h"

int timed_read(const void *dest, char *data, uint32_t data_size, uint32_t ms) {
    int ret = -1;

    while (ms--) {
        ret = read(dest, data, data_size);
        if (ret > 0) {
            break;
        }

        sleep(1);
    }

    return ret;
}

void *expect_answer(void * volatile * ptr, void *init_value, uint32_t timeout) {
    while (timeout--) {
        if (*ptr != init_value) {
            return (void *) *ptr;
        }

        sleep(1);
    }

    return NULL;
}
