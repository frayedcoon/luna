#include "platform/clock.h"

extern volatile int32_t systick_msec;


//! clock = ((HSI / PLLM) * PLLN) / PLLP)
//! clock = ((16 / 16) * 336) / 4) = 84MHz
#define PLLN 336
#define PLLM 16
//! 4
#define PLLP 1
#define PLLQ 4

//! not divided
#define AHB_PRESC  0x00
//! divided by 2
#define APB1_PRESC 0x04
//! not divided
#define APB2_PRESC 0x00

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
    //! Using HSI clock for PLL (16MHz)

    //! HSI on
    RCC_CR |= BIT(0);
    //! wait until HSI ready
    while (!(RCC_CR & BIT(1)));

    //! turn off PLL
    RCC_CR &= ~(BIT24);

    //! PLL prescalers
    uint32_t pll_value = PLLM | (PLLN << 6) | (PLLP << 16) | (PLLQ << 24);
    RCC_PLLCFGR = pll_value;
    //! set prescalers, PLL as sys clock
    RCC_CFGR |= (AHB_PRESC << 4) | (APB1_PRESC << 10) | (APB2_PRESC << 13) | BIT(1);
    //! 5 flash wait states
    FLASH_ACR |= 5;
    //! turn on PLL
    RCC_CR |= BIT(24);
    //! wait until PLL ready
    while (!(RCC_CR & BIT(25)));
    //! wait until PLL ready as sysclk
    while (!(RCC_CFGR & BIT(3)));
}
