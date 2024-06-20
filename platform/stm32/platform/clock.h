#ifndef PLATFORM_CLOCK_H
#define PLATFORM_CLOCK_H

#include "common/common.h"
#include "platform/peripheral.h"

/**
 * @brief      gets the value of systick in mseconds.
 *
 * @return     the systick mseconds.
 */
int32_t clock_get(void);

/**
 * @brief      delay in mseconds
 *
 * @param[in]  msecs  msecconds to wait
 */
void clock_dly_msecs(uint32_t msecs);

/**
 * @brief      delay in seconds
 *
 * @param[in]  secs  secconds to wait
 */
void clock_dly_secs(uint32_t secs);

/**
 * @brief      initialize hardware clock/peripheral clock
 */
void clock_init(void);

#endif
