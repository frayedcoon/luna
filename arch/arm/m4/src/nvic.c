#include "arch/nvic.h"
#include "arch/core.h"
#include "arch/context.h"
#include "kernel/thread.h"
#include "lib/string.h"
#include "common/def.h"
#include "common/log.h"

void nvic_it_enable(it_channel channel) {
    *((uint32_t*) ISER_BASE + ((uint8_t) channel >> 5)) = (0x1 << ((uint8_t) channel & 0x1f));
}

void nvic_it_disable(it_channel channel) {
    *((uint32_t*) ICER_BASE + ((uint8_t) channel >> 5)) = (0x1 << ((uint8_t) channel & 0x1f));
}
