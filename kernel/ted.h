#ifndef KERNEL_TED_H
#define KERNEL_TED_H

#include "common/def.h"
#include "common/common.h"

typedef void (*ted_event)(void *ctx);

int ted_schedule(ted_event event, void *event_ctx, uint32_t msecs);

void ted_dispatch(void);

#endif
