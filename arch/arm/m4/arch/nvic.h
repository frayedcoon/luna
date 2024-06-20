#ifndef ARCH_NVIC_H
#define ARCH_NVIC_H

#include "common/common.h"

/**
 * interrupt channels enumeration
 */
typedef enum it_channel_t {
    IT_WWDG         = 0,
    IT_PVD          = 1,
    IT_TAMPER_STAMP = 2,
    IT_RTC_WKUP     = 3,
    IT_FLASH        = 4,
    IT_RCC          = 5,
    IT_EXTI0        = 6,
    IT_EXTI1        = 7,
    IT_EXTI2_TS     = 8,
    IT_EXTI3        = 9,
    IT_EXTI4        = 10,
    IT_DMA1_CH1     = 11,
    IT_DMA1_CH2     = 12,
    IT_DMA1_CH3     = 13,
    IT_DMA1_CH4     = 14,
    IT_DMA1_CH5     = 15,
    IT_DMA1_CH6     = 16,
    IT_DMA1_CH7     = 17,
    IT_ADC1_2       = 18,
    IT_CAN_TX       = 19,
    IT_CAN_RX0      = 20,
    IT_CAN_RX1      = 21,
    IT_CAN_SCE      = 22,
    IT_EXTI9_5      = 23,
    IT_TIM1_BRK     = 24,
    IT_TIM1_UP      = 25,
    IT_TIM1_TRG_COM = 26,
    IT_TIM1_CC      = 27,
    IT_TIM2         = 28,
    IT_TIM3         = 29,
    IT_TIM4         = 30,
    IT_I2C1_EV      = 31,
    IT_I2C1_ER      = 32,
    IT_I2C2_EV      = 33,
    IT_I2C2_ER      = 34,
    IT_SPI1         = 35,
    IT_SPI2         = 36,
    IT_USART1       = 37,
    IT_USART2       = 38,
    IT_USART3       = 39,
    IT_EXTI15_10    = 40,
    IT_RTC_ALARM    = 41,
    IT_EXTI19       = 42,
    IT_TIM8_BRK     = 43,
    IT_TIM8_UP      = 44,
    IT_TIM8_TRG_COM = 45,
    IT_TIM8_CC      = 46,
    IT_ADC3         = 47,
    /**
     * RESERVED [48-50]
     */
    IT_SPI3         = 51,
    IT_USART4       = 52,
    IT_USART5       = 53,
    IT_TIM6_DAC     = 54,
    IT_TIM7         = 55,
    IT_DMA2_CH1     = 56,
    IT_DMA2_CH2     = 57,
    IT_DMA2_CH3     = 58,
    IT_DMA2_CH4     = 59,
    IT_DMA2_CH5     = 60,
    IT_ADC4         = 61,
    /**
     * RESERVED [62-63]
     */
    IT_COMP1_2_3    = 64,
    IT_COMP4_5_6    = 65,
    IT_COMP7        = 66,
    /**
     * RESERVED [67-73]
     */
    IT_USB_HP       = 74,
    IT_USB_LP       = 75,
    IT_USB_WKUP     = 76,
    /**
     * RESERVED [77-80]
     */
    IT_FPU          = 81
} it_channel;

/**
 * @brief      enable interrupt by given channel
 *
 * @param[in]  channel  interrupt channel
 */
void nvic_it_enable(it_channel channel);

/**
 * @brief      disable interrupt by given channel
 *
 * @param[in]  channel  interrupt channel
 */
void nvic_it_disable(it_channel channel);

#endif
