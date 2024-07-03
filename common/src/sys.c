#include "common/sys.h"
#include "lib/syscall.h"
#include "common/log.h"

int timed_read(const void *dest, void *data, uint32_t data_size, uint32_t ms) {
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
