#include "platform/clock.h"

extern volatile int32_t systick_msec;

/**
 * @brief      delay produced by forcing cpu doing nothing
 *
 * @param[in]  dly   cycles to wait
 */
static void dummy_dly(uint32_t dly) {
    while (dly--);
}

int32_t clock_get(void) {
    return systick_msec;
}

void clock_dly_msecs(uint32_t msecs) {
    int32_t timeout = systick_msec + msecs;
    while (timeout - systick_msec > 0);
}

void clock_dly_secs(uint32_t secs) {
    while (secs--) {
        clock_dly_msecs(1000);
    }
}

void clock_init(void) {
    //! Using HSI clock for PLL (8MHz)

    //! turn off PLL
    RCC_CR &= ~BIT24;
    dummy_dly(10);
    //! PLL multiply = 16 -> HCLK = 64MHz
    RCC_CFGR |= ((0xFF) << 18);
    dummy_dly(10);
    //! APB1/APB2 clock speed = HCLK / 2 = 32MHz
    RCC_CFGR |= BIT10;
    RCC_CFGR |= BIT13;
    dummy_dly(10);
    //! turn on PLL
    RCC_CR |= BIT24;
    dummy_dly(10);
    //! set PLL as clock
    RCC_CFGR |= BIT1;
    //! two wait states
    //! for flash access
    //! 48 < HCLK <= 72 MHz
    FLASH_ACR |= BIT1;
}
