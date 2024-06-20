#include "platform/kernel_entry.h"
#include "platform/handler.h"
#include "arch/nvic.h"
#include "arch/handler.h"
#include "arch/svc.h"

const void * Vectors[] __attribute__((section(".vectors"))) = {
    (void *) &STACK_END, /* Initial MSP value */
    kernel_entry,        /* Reset Handler */
    default_handler,     /* NMI */
    h_fault_handler,     /* Hard Fault */
    default_handler,     /* MemManage */
    default_handler,     /* BusFault  */
    default_handler,     /* UsageFault */
    default_handler,     /* RESERVED */
    default_handler,     /* RESERVED */
    default_handler,     /* RESERVED */
    default_handler,     /* RESERVED */
    sv_call_handler,     /* SVCall */
    default_handler,     /* RESERVED */
    default_handler,     /* RESERVED */
    pend_sv_handler,     /* PendSV */
    systick_handler,     /* SysTick */
    default_handler,     /* WWDG */
    default_handler,     /* PVD */
    default_handler,     /* TAMP_STAMP */
    default_handler,     /* RTC_WKUP */
    default_handler,     /* FLASH */
    default_handler,     /* RCC */
    default_handler,     /* EXTI0 */
    default_handler,     /* EXTI1 */
    default_handler,     /* EXTI2 and TSC */
    default_handler,     /* EXTI3 */
    default_handler,     /* EXTI4 */
    default_handler,     /* DMA_CH1 */
    default_handler,     /* DMA_CH2 */
    default_handler,     /* DMA_CH3 */
    default_handler,     /* DMA_CH4 */
    default_handler,     /* DMA_CH5 */
    default_handler,     /* DMA_CH6 */
    default_handler,     /* DMA_CH7 */
    default_handler,     /* ADC1_2 */
    default_handler,     /* USB_HP/CAN_TX */
    default_handler,     /* USB_LP/CAN_RX0 */
    default_handler,     /* CAN_RX1 */
    default_handler,     /* CAN_SCE */
    default_handler,     /* EXTI9_5 */
    default_handler,     /* TIM1_BRK/TIM15 */
    default_handler,     /* TIM1_UP/TIM16 */
    default_handler,     /* TIM1_TRG/TIM17 */
    default_handler,     /* TIM1_CC */
    default_handler,     /* TIM2 */
    default_handler,     /* TIM3 */
    default_handler,     /* TIM4 */
    default_handler,     /* I2C1_EV_EXTI23 */
    default_handler,     /* I2C1_ER */
    default_handler,     /* I2C2_EV_EXTI24 */
    default_handler,     /* I2C2_ER */
    default_handler,     /* SPI1 */
    default_handler,     /* SPI2 */
    default_handler,     /* USART1_EXTI25 */
    usart2_handler,      /* USART2_EXTI26 */
    default_handler,     /* USART3_EXTI28 */
    default_handler,     /* EXTI15_10 */
    default_handler,     /* RTCAlarm */
    default_handler,     /* USB_WKUP */
    default_handler,     /* TIM8_BRK */
    default_handler,     /* TIM8_UP */
    default_handler,     /* TIM8_TRG_COM */
    default_handler,     /* TIM8_CC */
    default_handler,     /* ADC3 */
    default_handler,     /* RESERVED */
    default_handler,     /* RESERVED */
    default_handler,     /* RESERVED */
    default_handler,     /* SPI3 */
    default_handler,     /* UART4_EXTI34 */
    default_handler,     /* UART5_EXTI35 */
    default_handler,     /* TIM6_DACUNDER */
    default_handler,     /* TIM7 */
    default_handler,     /* DMA2_CH1 */
    default_handler,     /* DMA2_CH2 */
    default_handler,     /* DMA2_CH3 */
    default_handler,     /* DMA2_CH4 */
    default_handler,     /* DMA2_CH5 */
    default_handler,     /* ADC4 */
    default_handler,     /* RESERVED */
    default_handler,     /* RESERVED */
    default_handler,     /* COMP123 */
    default_handler,     /* COMP456 */
    default_handler,     /* COMP7 */
    default_handler,     /* RESERVED */
    default_handler,     /* RESERVED */
    default_handler,     /* RESERVED */
    default_handler,     /* RESERVED */
    default_handler,     /* RESERVED */
    default_handler,     /* RESERVED */
    default_handler,     /* RESERVED */
    default_handler,     /* USB_HP */
    default_handler,     /* USB_LP */
    default_handler,     /* USB_WKUP */
    default_handler,     /* RESERVED */
    default_handler,     /* RESERVED */
    default_handler,     /* RESERVED */
    default_handler,     /* RESERVED */
    default_handler,     /* FPU */
};

void kernel_entry(void) {
    unsigned char *src;
    unsigned char *dst;
    unsigned len;

    src= &INIT_DATA_VALUES;
    dst= &INIT_DATA_START;
    len= &INIT_DATA_END - &INIT_DATA_START;

    while (len--) {
        *dst++ = *src++;
    }
    dst = &BSS_START;
    len = &BSS_END - &BSS_START;
    while (len--) {
        *dst++=0;
    }

    kernel();
}
