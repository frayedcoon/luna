#include "arch/systick.h"

void systick_init(void) {
    STK_CSR |= ( BIT2 | BIT1 | BIT0);
    //! set systick prescaler
    //! 64MHz / 64kHz = 1kHz (every 1 msecond)
    STK_RVR = 0xFA00;
    STK_CVR = 0;

    //! high priority for systick exception
    SHPR3 |= (0x0f << 24);
    //! the lowest possible priority for PendSV exception
    SHPR3 |= (0xff << 16);
}
