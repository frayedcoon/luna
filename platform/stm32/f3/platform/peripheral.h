#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include "common/common.h"

//! AHB3 Peripherals
#define ADC_3_4_BASE                        0x50000400
#define ADC_1_2_BASE                        0x50000000

//! AHB2 Peripherals
#define GPIOH_BASE                          0x48001c00
#define GPIOG_BASE                          0x48001800
#define GPIOF_BASE                          0x48001400
#define GPIOE_BASE                          0x48001000
#define GPIOD_BASE                          0x48000c00
#define GPIOC_BASE                          0x48000800
#define GPIOB_BASE                          0x48000400
#define GPIOA_BASE                          0x48000000

//! AHB1 Peripherals
#define TSC_BASE                            0x40024000
#define CRC_BASE                            0x40023000
#define FLASH_BASE                          0x40022000
#define RCC_BASE                            0x40021000
#define DMA2_BASE                           0x40020400
#define DMA1_BASE                           0x40020000

//! APB2 Peripherals
#define TIM20_BASE                          0x40015000
#define TIM17_BASE                          0x40014800
#define TIM16_BASE                          0x40014400
#define TIM15_BASE                          0x40014000
#define SPI4_BASE                           0x40013C00
#define USART1_BASE                         0x40013800
#define TIM8_BASE                           0x40013400
#define SPI1_BASE                           0x40013000
#define TIM1_BASE                           0x40012c00
#define EXTI_BASE                           0x40010400
#define SYSCFG_BASE                         0x40010000

//! APB1 Peripherals
#define I2C3                                0x40007800
#define DAC1_BASE                           0x40007400
#define PWR_BASE                            0x40007000
#define BXCAN_BASE                          0x40006400
#define USB_SRAM_BASE                       0x40006000
#define USB_DEVICE_FS_BASE                  0x40005c00
#define I2C2_BASE                           0x40005800
#define I2C1_BASE                           0x40005400
#define USART5_BASE                         0x40005000
#define USART4_BASE                         0x40004c00
#define USART3_BASE                         0x40004800
#define USART2_BASE                         0x40004400
#define I2S3EXT_BASE                        0x40004000
#define SPI3_BASE                           0x40003c00
#define SPI2_BASE                           0x40003800
#define I2S2EXT_BASE                        0x40003400
#define IWDG_BASE                           0x40003000
#define WWDG_BASE                           0x40002c00
#define RTC_BASE                            0x40002800
#define TIM7_BASE                           0x40001400
#define TIM6_BASE                           0x40001000
#define TIM4_BASE                           0x40000800
#define TIM3_BASE                           0x40000400
#define TIM2_BASE                           0x40000000

//! FLASH
#define FLASH_ACR                           REGISTER_32(FLASH_BASE + 0)
#define FLASH_KEYR                          REGISTER_32(FLASH_BASE + 4)
#define FLASH_OPTKEYR                       REGISTER_32(FLASH_BASE + 8)
#define FLASH_SR                            REGISTER_32(FLASH_BASE + 0x0c)
#define FLASH_CR                            REGISTER_32(FLASH_BASE + 0x10)
#define FLASH_AR                            REGISTER_32(FLASH_BASE + 0x14)
#define FLASH_OBR                           REGISTER_32(FLASH_BASE + 0x1c)
#define FLASH_WRPR                          REGISTER_32(FLASH_BASE + 0x20)

//! Option bytes - see reference manual for details.
#define OPTION_WORD0                        REGISTER_32(0x1ffff800)
#define OPTION_WORD1                        REGISTER_32(0x1ffff804)
#define OPTION_WORD2                        REGISTER_32(0x1ffff808)
#define OPTION_WORD3                        REGISTER_32(0x1ffff80c)

//! CRC
#define CRC_DR                              REGISTER_32(CRC_BASE + 0)
#define CRC_IDR                             REGISTER_32(CRC_BASE + 4)
#define CRC_CR                              REGISTER_32(CRC_BASE + 8)
#define CRC_INIT                            REGISTER_32(CRC_BASE + 0x10)
#define CRC_POLY                            REGISTER_32(CRC_BASE + 0x14)

//! PWR
#define PWR_CR                              REGISTER_32(PWR_BASE + 0)
#define PWR_CSR                             REGISTER_32(PWR_BASE + 4)

//! RCC registers
#define RCC_CR                              REGISTER_32(RCC_BASE + 0)
#define RCC_CFGR                            REGISTER_32(RCC_BASE + 4)
#define RCC_CIR                             REGISTER_32(RCC_BASE + 8)
#define RCC_APB2RSTR                        REGISTER_32(RCC_BASE + 0x0c)
#define RCC_APB1RSTR                        REGISTER_32(RCC_BASE + 0x10)
#define RCC_AHBENR                          REGISTER_32(RCC_BASE + 0x14)
#define RCC_APB2ENR                         REGISTER_32(RCC_BASE + 0x18)
#define RCC_APB1ENR                         REGISTER_32(RCC_BASE + 0x1c)
#define RCC_BDCR                            REGISTER_32(RCC_BASE + 0x20)
#define RCC_CSR                             REGISTER_32(RCC_BASE + 0x24)
#define RCC_AHBRSTR                         REGISTER_32(RCC_BASE + 0x28)
#define RCC_CFGR2                           REGISTER_32(RCC_BASE + 0x2c)
#define RCC_CFGR3                           REGISTER_32(RCC_BASE + 0x30)

//! DMA1 (7 channels)
#define DMA1_ISR                            REGISTER_32(DMA1_BASE + 0)
#define DMA1_IFCR                           REGISTER_32(DMA1_BASE + 4)
#define DMA1_CCR1                           REGISTER_32(DMA1_BASE + 8 + 0x0d20*0)
#define DMA1_CCR2                           REGISTER_32(DMA1_BASE + 8 + 0x0d20*1)
#define DMA1_CCR3                           REGISTER_32(DMA1_BASE + 8 + 0x0d20*2)
#define DMA1_CCR4                           REGISTER_32(DMA1_BASE + 8 + 0x0d20*3)
#define DMA1_CCR5                           REGISTER_32(DMA1_BASE + 8 + 0x0d20*4)
#define DMA1_CCR6                           REGISTER_32(DMA1_BASE + 8 + 0x0d20*5)
#define DMA1_CCR7                           REGISTER_32(DMA1_BASE + 8 + 0x0d20*6)
#define DMA1_CNDTR1                         REGISTER_32(DMA1_BASE + 0x0c + 0x0d20*0)
#define DMA1_CNDTR2                         REGISTER_32(DMA1_BASE + 0x0c + 0x0d20*1)
#define DMA1_CNDTR3                         REGISTER_32(DMA1_BASE + 0x0c + 0x0d20*2)
#define DMA1_CNDTR4                         REGISTER_32(DMA1_BASE + 0x0c + 0x0d20*3)
#define DMA1_CNDTR5                         REGISTER_32(DMA1_BASE + 0x0c + 0x0d20*4)
#define DMA1_CNDTR6                         REGISTER_32(DMA1_BASE + 0x0c + 0x0d20*5)
#define DMA1_CNDTR7                         REGISTER_32(DMA1_BASE + 0x0c + 0x0d20*6)
#define DMA1_CPAR1                          REGISTER_32(DMA1_BASE + 0x10 + 0x0d20*0)
#define DMA1_CPAR2                          REGISTER_32(DMA1_BASE + 0x10 + 0x0d20*1)
#define DMA1_CPAR3                          REGISTER_32(DMA1_BASE + 0x10 + 0x0d20*2)
#define DMA1_CPAR4                          REGISTER_32(DMA1_BASE + 0x10 + 0x0d20*3)
#define DMA1_CPAR5                          REGISTER_32(DMA1_BASE + 0x10 + 0x0d20*4)
#define DMA1_CPAR6                          REGISTER_32(DMA1_BASE + 0x10 + 0x0d20*5)
#define DMA1_CPAR7                          REGISTER_32(DMA1_BASE + 0x10 + 0x0d20*6)
#define DMA1_CMAR1                          REGISTER_32(DMA1_BASE + 0x14 + 0x0d20*0)
#define DMA1_CMAR2                          REGISTER_32(DMA1_BASE + 0x14 + 0x0d20*1)
#define DMA1_CMAR3                          REGISTER_32(DMA1_BASE + 0x14 + 0x0d20*2)
#define DMA1_CMAR4                          REGISTER_32(DMA1_BASE + 0x14 + 0x0d20*3)
#define DMA1_CMAR5                          REGISTER_32(DMA1_BASE + 0x14 + 0x0d20*4)
#define DMA1_CMAR6                          REGISTER_32(DMA1_BASE + 0x14 + 0x0d20*5)
#define DMA1_CMAR7                          REGISTER_32(DMA1_BASE + 0x14 + 0x0d20*6)

//! DMA2 (5 channels)
#define DMA2_ISR                            REGISTER_32(DMA2_BASE + 0)
#define DMA2_IFCR                           REGISTER_32(DMA2_BASE + 4)
#define DMA2_CCR1                           REGISTER_32(DMA2_BASE + 8 + 0x0d20*0)
#define DMA2_CCR2                           REGISTER_32(DMA2_BASE + 8 + 0x0d20*1)
#define DMA2_CCR3                           REGISTER_32(DMA2_BASE + 8 + 0x0d20*2)
#define DMA2_CCR4                           REGISTER_32(DMA2_BASE + 8 + 0x0d20*3)
#define DMA2_CCR5                           REGISTER_32(DMA2_BASE + 8 + 0x0d20*4)
#define DMA2_CCR6                           REGISTER_32(DMA2_BASE + 8 + 0x0d20*5)
#define DMA2_CCR7                           REGISTER_32(DMA2_BASE + 8 + 0x0d20*6)
#define DMA2_CNDTR1                         REGISTER_32(DMA2_BASE + 0x0c + 0x0d20*0)
#define DMA2_CNDTR2                         REGISTER_32(DMA2_BASE + 0x0c + 0x0d20*1)
#define DMA2_CNDTR3                         REGISTER_32(DMA2_BASE + 0x0c + 0x0d20*2)
#define DMA2_CNDTR4                         REGISTER_32(DMA2_BASE + 0x0c + 0x0d20*3)
#define DMA2_CNDTR5                         REGISTER_32(DMA2_BASE + 0x0c + 0x0d20*4)
#define DMA2_CNDTR6                         REGISTER_32(DMA2_BASE + 0x0c + 0x0d20*5)
#define DMA2_CNDTR7                         REGISTER_32(DMA2_BASE + 0x0c + 0x0d20*6)
#define DMA2_CPAR1                          REGISTER_32(DMA2_BASE + 0x10 + 0x0d20*0)
#define DMA2_CPAR2                          REGISTER_32(DMA2_BASE + 0x10 + 0x0d20*1)
#define DMA2_CPAR3                          REGISTER_32(DMA2_BASE + 0x10 + 0x0d20*2)
#define DMA2_CPAR4                          REGISTER_32(DMA2_BASE + 0x10 + 0x0d20*3)
#define DMA2_CPAR5                          REGISTER_32(DMA2_BASE + 0x10 + 0x0d20*4)
#define DMA2_CPAR6                          REGISTER_32(DMA2_BASE + 0x10 + 0x0d20*5)
#define DMA2_CPAR7                          REGISTER_32(DMA2_BASE + 0x10 + 0x0d20*6)
#define DMA2_CMAR1                          REGISTER_32(DMA2_BASE + 0x14 + 0x0d20*0)
#define DMA2_CMAR2                          REGISTER_32(DMA2_BASE + 0x14 + 0x0d20*1)
#define DMA2_CMAR3                          REGISTER_32(DMA2_BASE + 0x14 + 0x0d20*2)
#define DMA2_CMAR4                          REGISTER_32(DMA2_BASE + 0x14 + 0x0d20*3)
#define DMA2_CMAR5                          REGISTER_32(DMA2_BASE + 0x14 + 0x0d20*4)
#define DMA2_CMAR6                          REGISTER_32(DMA2_BASE + 0x14 + 0x0d20*5)
#define DMA2_CMAR7                          REGISTER_32(DMA2_BASE + 0x14 + 0x0d20*6)

//! SYS Config
#define SYSCFG_CFGR1                        REGISTER_32(SYSCFG_BASE + 0)
#define SYSCFG_RCR                          REGISTER_32(SYSCFG_BASE + 4)
#define SYSCFG_EXTICR1                      REGISTER_32(SYSCFG_BASE + 8)
#define SYSCFG_EXTICR2                      REGISTER_32(SYSCFG_BASE + 0x0c)
#define SYSCFG_EXTICR3                      REGISTER_32(SYSCFG_BASE + 0x10)
#define SYSCFG_EXTICR4                      REGISTER_32(SYSCFG_BASE + 0x14)
#define SYSCFG_CFGR2                        REGISTER_32(SYSCFG_BASE + 0x18)

//! EXTI
#define EXTI_IMR1                           REGISTER_32(EXTI_BASE + 0)
#define EXTI_EMR1                           REGISTER_32(EXTI_BASE + 4)
#define EXTI_RTSR1                          REGISTER_32(EXTI_BASE + 8)
#define EXTI_FTSR1                          REGISTER_32(EXTI_BASE + 0x0c)
#define EXTI_SWIER1                         REGISTER_32(EXTI_BASE + 0x10)
#define EXTI_PR1                            REGISTER_32(EXTI_BASE + 0x14)
#define EXTI_IMR2                           REGISTER_32(EXTI_BASE + 0x20)
#define EXTI_EMR2                           REGISTER_32(EXTI_BASE + 0x24)
#define EXTI_RTSR2                          REGISTER_32(EXTI_BASE + 0x28)
#define EXTI_FTSR2                          REGISTER_32(EXTI_BASE + 0x2c)
#define EXTI_SWIER2                         REGISTER_32(EXTI_BASE + 0x30)
#define EXTI_PR2                            REGISTER_32(EXTI_BASE + 0x34)

//! ADC1
#define ADC1_ISR                            REGISTER_32(ADC_1_2_BASE + 0)
#define ADC1_IER                            REGISTER_32(ADC_1_2_BASE + 4)
#define ADC1_CR                             REGISTER_32(ADC_1_2_BASE + 8)
#define ADC1_CFGR1                          REGISTER_32(ADC_1_2_BASE + 0x0c)
#define ADC1_SMPR1                          REGISTER_32(ADC_1_2_BASE + 0x14)
#define ADC1_SMPR2                          REGISTER_32(ADC_1_2_BASE + 0x18)
#define ADC1_TR1                            REGISTER_32(ADC_1_2_BASE + 0x20)
#define ADC1_TR2                            REGISTER_32(ADC_1_2_BASE + 0x24)
#define ADC1_TR3                            REGISTER_32(ADC_1_2_BASE + 0x28)
#define ADC1_SQR1                           REGISTER_32(ADC_1_2_BASE + 0x30)
#define ADC1_SQR2                           REGISTER_32(ADC_1_2_BASE + 0x34)
#define ADC1_SQR3                           REGISTER_32(ADC_1_2_BASE + 0x38)
#define ADC1_SQR4                           REGISTER_32(ADC_1_2_BASE + 0x3c)
#define ADC1_DR                             REGISTER_32(ADC_1_2_BASE + 0x40)
#define ADC1_JSQR                           REGISTER_32(ADC_1_2_BASE + 0x4c)
#define ADC1_OFR1                           REGISTER_32(ADC_1_2_BASE + 0x60)
#define ADC1_OFR2                           REGISTER_32(ADC_1_2_BASE + 0x64)
#define ADC1_OFR3                           REGISTER_32(ADC_1_2_BASE + 0x68)
#define ADC1_OFR4                           REGISTER_32(ADC_1_2_BASE + 0x6c)
#define ADC1_JDR1                           REGISTER_32(ADC_1_2_BASE + 0x80)
#define ADC1_JDR2                           REGISTER_32(ADC_1_2_BASE + 0x84)
#define ADC1_JDR3                           REGISTER_32(ADC_1_2_BASE + 0x88)
#define ADC1_JDR4                           REGISTER_32(ADC_1_2_BASE + 0x8c)
#define ADC1_AWD2CR                         REGISTER_32(ADC_1_2_BASE + 0xa0)
#define ADC1_AWD3CR                         REGISTER_32(ADC_1_2_BASE + 0xa4)
#define ADC1_DIFSEL                         REGISTER_32(ADC_1_2_BASE + 0xb0)
#define ADC1_CALFACT                        REGISTER_32(ADC_1_2_BASE + 0xa4)

//! ADC2
#define ADC2_ISR                            REGISTER_32(ADC_1_2_BASE +0x100 + 0)
#define ADC2_IER                            REGISTER_32(ADC_1_2_BASE +0x100 + 4)
#define ADC2_CR                             REGISTER_32(ADC_1_2_BASE +0x100 + 8)
#define ADC2_CFGR1                          REGISTER_32(ADC_1_2_BASE +0x100 + 0x0c)
#define ADC2_SMPR1                          REGISTER_32(ADC_1_2_BASE +0x100 + 0x14)
#define ADC2_SMPR2                          REGISTER_32(ADC_1_2_BASE +0x100 + 0x18)
#define ADC2_TR1                            REGISTER_32(ADC_1_2_BASE +0x100 + 0x20)
#define ADC2_TR2                            REGISTER_32(ADC_1_2_BASE +0x100 + 0x24)
#define ADC2_TR3                            REGISTER_32(ADC_1_2_BASE +0x100 + 0x28)
#define ADC2_SQR1                           REGISTER_32(ADC_1_2_BASE +0x100 + 0x30)
#define ADC2_SQR2                           REGISTER_32(ADC_1_2_BASE +0x100 + 0x34)
#define ADC2_SQR3                           REGISTER_32(ADC_1_2_BASE +0x100 + 0x38)
#define ADC2_SQR4                           REGISTER_32(ADC_1_2_BASE +0x100 + 0x3c)
#define ADC2_DR                             REGISTER_32(ADC_1_2_BASE +0x100 + 0x40)
#define ADC2_JSQR                           REGISTER_32(ADC_1_2_BASE +0x100 + 0x4c)
#define ADC2_OFR1                           REGISTER_32(ADC_1_2_BASE +0x100 + 0x60)
#define ADC2_OFR2                           REGISTER_32(ADC_1_2_BASE +0x100 + 0x64)
#define ADC2_OFR3                           REGISTER_32(ADC_1_2_BASE +0x100 + 0x68)
#define ADC2_OFR4                           REGISTER_32(ADC_1_2_BASE +0x100 + 0x6c)
#define ADC2_JDR1                           REGISTER_32(ADC_1_2_BASE +0x100 + 0x80)
#define ADC2_JDR2                           REGISTER_32(ADC_1_2_BASE +0x100 + 0x84)
#define ADC2_JDR3                           REGISTER_32(ADC_1_2_BASE +0x100 + 0x88)
#define ADC2_JDR4                           REGISTER_32(ADC_1_2_BASE +0x100 + 0x8c)
#define ADC2_AWD2CR                         REGISTER_32(ADC_1_2_BASE +0x100 + 0xa0)
#define ADC2_AWD3CR                         REGISTER_32(ADC_1_2_BASE +0x100 + 0xa4)
#define ADC2_DIFSEL                         REGISTER_32(ADC_1_2_BASE +0x100 + 0xb0)
#define ADC2_CALFACT                        REGISTER_32(ADC_1_2_BASE +0x100 + 0xa4)
#define ADC12_CSR                           REGISTER_32(ADC_1_2_BASE +0x300 + 0x00)
#define ADC12_CCR                           REGISTER_32(ADC_1_2_BASE +0x300 + 0x08)
#define ADC12_CDR                           REGISTER_32(ADC_1_2_BASE +0x300 + 0x0c)

//! ADC3
#define ADC3_ISR                            REGISTER_32(ADC_3_4_BASE + 0)
#define ADC3_IER                            REGISTER_32(ADC_3_4_BASE + 4)
#define ADC3_CR                             REGISTER_32(ADC_3_4_BASE + 8)
#define ADC3_CFGR1                          REGISTER_32(ADC_3_4_BASE + 0x0c)
#define ADC3_SMPR1                          REGISTER_32(ADC_3_4_BASE + 0x14)
#define ADC3_SMPR2                          REGISTER_32(ADC_3_4_BASE + 0x18)
#define ADC3_TR1                            REGISTER_32(ADC_3_4_BASE + 0x20)
#define ADC3_TR2                            REGISTER_32(ADC_3_4_BASE + 0x24)
#define ADC3_TR3                            REGISTER_32(ADC_3_4_BASE + 0x28)
#define ADC3_SQR1                           REGISTER_32(ADC_3_4_BASE + 0x30)
#define ADC3_SQR2                           REGISTER_32(ADC_3_4_BASE + 0x34)
#define ADC3_SQR3                           REGISTER_32(ADC_3_4_BASE + 0x38)
#define ADC3_SQR4                           REGISTER_32(ADC_3_4_BASE + 0x3c)
#define ADC3_DR                             REGISTER_32(ADC_3_4_BASE + 0x40)
#define ADC3_JSQR                           REGISTER_32(ADC_3_4_BASE + 0x4c)
#define ADC3_OFR1                           REGISTER_32(ADC_3_4_BASE + 0x60)
#define ADC3_OFR2                           REGISTER_32(ADC_3_4_BASE + 0x64)
#define ADC3_OFR3                           REGISTER_32(ADC_3_4_BASE + 0x68)
#define ADC3_OFR4                           REGISTER_32(ADC_3_4_BASE + 0x6c)
#define ADC3_JDR1                           REGISTER_32(ADC_3_4_BASE + 0x80)
#define ADC3_JDR2                           REGISTER_32(ADC_3_4_BASE + 0x84)
#define ADC3_JDR3                           REGISTER_32(ADC_3_4_BASE + 0x88)
#define ADC3_JDR4                           REGISTER_32(ADC_3_4_BASE + 0x8c)
#define ADC3_AWD2CR                         REGISTER_32(ADC_3_4_BASE + 0xa0)
#define ADC3_AWD3CR                         REGISTER_32(ADC_3_4_BASE + 0xa4)
#define ADC3_DIFSEL                         REGISTER_32(ADC_3_4_BASE + 0xb0)
#define ADC3_CALFACT                        REGISTER_32(ADC_3_4_BASE + 0xa4)

//! ADC2
#define ADC4_ISR                            REGISTER_32(ADC_3_4_BASE +0x100 + 0)
#define ADC4_IER                            REGISTER_32(ADC_3_4_BASE +0x100 + 4)
#define ADC4_CR                             REGISTER_32(ADC_3_4_BASE +0x100 + 8)
#define ADC4_CFGR1                          REGISTER_32(ADC_3_4_BASE +0x100 + 0x0c)
#define ADC4_SMPR1                          REGISTER_32(ADC_3_4_BASE +0x100 + 0x14)
#define ADC4_SMPR2                          REGISTER_32(ADC_3_4_BASE +0x100 + 0x18)
#define ADC4_TR1                            REGISTER_32(ADC_3_4_BASE +0x100 + 0x20)
#define ADC4_TR2                            REGISTER_32(ADC_3_4_BASE +0x100 + 0x24)
#define ADC4_TR3                            REGISTER_32(ADC_3_4_BASE +0x100 + 0x28)
#define ADC4_SQR1                           REGISTER_32(ADC_3_4_BASE +0x100 + 0x30)
#define ADC4_SQR2                           REGISTER_32(ADC_3_4_BASE +0x100 + 0x34)
#define ADC4_SQR3                           REGISTER_32(ADC_3_4_BASE +0x100 + 0x38)
#define ADC4_SQR4                           REGISTER_32(ADC_3_4_BASE +0x100 + 0x3c)
#define ADC4_DR                             REGISTER_32(ADC_3_4_BASE +0x100 + 0x40)
#define ADC4_JSQR                           REGISTER_32(ADC_3_4_BASE +0x100 + 0x4c)
#define ADC4_OFR1                           REGISTER_32(ADC_3_4_BASE +0x100 + 0x60)
#define ADC4_OFR2                           REGISTER_32(ADC_3_4_BASE +0x100 + 0x64)
#define ADC4_OFR3                           REGISTER_32(ADC_3_4_BASE +0x100 + 0x68)
#define ADC4_OFR4                           REGISTER_32(ADC_3_4_BASE +0x100 + 0x6c)
#define ADC4_JDR1                           REGISTER_32(ADC_3_4_BASE +0x100 + 0x80)
#define ADC4_JDR2                           REGISTER_32(ADC_3_4_BASE +0x100 + 0x84)
#define ADC4_JDR3                           REGISTER_32(ADC_3_4_BASE +0x100 + 0x88)
#define ADC4_JDR4                           REGISTER_32(ADC_3_4_BASE +0x100 + 0x8c)
#define ADC4_AWD2CR                         REGISTER_32(ADC_3_4_BASE +0x100 + 0xa0)
#define ADC4_AWD3CR                         REGISTER_32(ADC_3_4_BASE +0x100 + 0xa4)
#define ADC4_DIFSEL                         REGISTER_32(ADC_3_4_BASE +0x100 + 0xb0)
#define ADC4_CALFACT                        REGISTER_32(ADC_3_4_BASE +0x100 + 0xa4)
#define ADC34_CSR                           REGISTER_32(ADC_3_4_BASE +0x300 + 0x00)
#define ADC34_CCR                           REGISTER_32(ADC_3_4_BASE +0x300 + 0x08)
#define ADC34_CDR                           REGISTER_32(ADC_3_4_BASE +0x300 + 0x0c)

//! DAC
#define DAC_CR                              REGISTER_32(DAC_BASE + 0)
#define DAC_SWTRIGR                         REGISTER_32(DAC_BASE + 4)
#define DAC_DHR12R1                         REGISTER_32(DAC_BASE + 8)
#define DAC_DHR12L1                         REGISTER_32(DAC_BASE + 0x0c)
#define DAC_DHR8R1                          REGISTER_32(DAC_BASE + 0x10)
#define DAC_DHR12R2                         REGISTER_32(DAC_BASE + 0x14)
#define DAC_DHR12L2                         REGISTER_32(DAC_BASE + 0x18)
#define DAC_DHR8R2                          REGISTER_32(DAC_BASE + 0x1c)
#define DAC_DHR12RD                         REGISTER_32(DAC_BASE + 0x20)
#define DAC_DHR12LD                         REGISTER_32(DAC_BASE + 0x24)
#define DAC_DHR8RD                          REGISTER_32(DAC_BASE + 0x28)
#define DAC_DOR1                            REGISTER_32(DAC_BASE + 0x2c)
#define DAC_DOR2                            REGISTER_32(DAC_BASE + 0x30)
#define DAC_SR                              REGISTER_32(DAC_BASE + 0x34)

//! COMP (in same address area as syscfg)
#define COMP1_CSR                           REGISTER_32(SYSCFG_BASE + 0x1c)
#define COMP2_CSR                           REGISTER_32(SYSCFG_BASE + 0x20)
#define COMP3_CSR                           REGISTER_32(SYSCFG_BASE + 0x24)
#define COMP4_CSR                           REGISTER_32(SYSCFG_BASE + 0x28)
#define COMP5_CSR                           REGISTER_32(SYSCFG_BASE + 0x2c)
#define COMP6_CSR                           REGISTER_32(SYSCFG_BASE + 0x30)
#define COMP7_CSR                           REGISTER_32(SYSCFG_BASE + 0x34)
#define OPAMP1_CSR                          REGISTER_32(SYSCFG_BASE + 0x38)
#define OPAMP2_CSR                          REGISTER_32(SYSCFG_BASE + 0x3c)
#define OPAMP3_CSR                          REGISTER_32(SYSCFG_BASE + 0x40)
#define OPAMP4_CSR                          REGISTER_32(SYSCFG_BASE + 0x44)

//! TSC
#define TS_CR                               REGISTER_32(TSC_BASE + 0)
#define TSC_IER                             REGISTER_32(TSC_BASE + 4)
#define TSC_ICR                             REGISTER_32(TSC_BASE + 8)
#define TSC_ISR                             REGISTER_32(TSC_BASE + 0x0c)
#define TSC_IOHCR                           REGISTER_32(TSC_BASE + 0x10)
#define TSC_IOASCR                          REGISTER_32(TSC_BASE + 0x18)
#define TSC_IOSCR                           REGISTER_32(TSC_BASE + 0x20)
#define TSC_IOCCR                           REGISTER_32(TSC_BASE + 0x28)
#define TSC_IOGCSR                          REGISTER_32(TSC_BASE + 0x30)
#define TSC_IOG1CR                          REGISTER_32(TSC_BASE + 0x34)
#define TSC_IOG2CR                          REGISTER_32(TSC_BASE + 0x38)
#define TSC_IOG3CR                          REGISTER_32(TSC_BASE + 0x3c)
#define TSC_IOG4CR                          REGISTER_32(TSC_BASE + 0x40)
#define TSC_IOG5CR                          REGISTER_32(TSC_BASE + 0x44)
#define TSC_IOG6CR                          REGISTER_32(TSC_BASE + 0x48)
#define TSC_IOG7CR                          REGISTER_32(TSC_BASE + 0x4c)
#define TSC_IOG8CR                          REGISTER_32(TSC_BASE + 0x50)

//! FLASH
#define FLASH_ACR                           REGISTER_32(FLASH_BASE + 0)
#define FLASH_KEYR                          REGISTER_32(FLASH_BASE + 4)
#define FLASH_OPTKEYR                       REGISTER_32(FLASH_BASE + 8)
#define FLASH_SR                            REGISTER_32(FLASH_BASE + 0x0c)
#define FLASH_CR                            REGISTER_32(FLASH_BASE + 0x10)
#define FLASH_AR                            REGISTER_32(FLASH_BASE + 0x14)
#define FLASH_OBR                           REGISTER_32(FLASH_BASE + 0x1c)
#define FLASH_WRPR                          REGISTER_32(FLASH_BASE + 0x20)

//! RCC registers
#define RCC_CR                              REGISTER_32(RCC_BASE + 0)
#define RCC_CFGR                            REGISTER_32(RCC_BASE + 4)
#define RCC_CIR                             REGISTER_32(RCC_BASE + 8)
#define RCC_APB2RSTR                        REGISTER_32(RCC_BASE + 0x0c)
#define RCC_APB1RSTR                        REGISTER_32(RCC_BASE + 0x10)
#define RCC_AHBENR                          REGISTER_32(RCC_BASE + 0x14)
#define RCC_APB2ENR                         REGISTER_32(RCC_BASE + 0x18)
#define RCC_APB1ENR                         REGISTER_32(RCC_BASE + 0x1c)
#define RCC_BDCR                            REGISTER_32(RCC_BASE + 0x20)
#define RCC_CSR                             REGISTER_32(RCC_BASE + 0x24)

//! BXCAN
#define CAN_MCR                             REGISTER_32(BXCAN_BASE + 0x000)
#define CAN_MSR                             REGISTER_32(BXCAN_BASE + 0x004)
#define CAN_TSR                             REGISTER_32(BXCAN_BASE + 0x008)
#define CAN_RF0R                            REGISTER_32(BXCAN_BASE + 0x00c)
#define CAN_RF1R                            REGISTER_32(BXCAN_BASE + 0x010)
#define CAN_IER                             REGISTER_32(BXCAN_BASE + 0x014)
#define CAN_ESR                             REGISTER_32(BXCAN_BASE + 0x018)
#define CAN_BTR                             REGISTER_32(BXCAN_BASE + 0x01c)
#define CAN_TI0R                            REGISTER_32(BXCAN_BASE + 0x180)
#define CAN_TDT0R                           REGISTER_32(BXCAN_BASE + 0x184)
#define CAN_TDL0R                           REGISTER_32(BXCAN_BASE + 0x188)
#define CAN_TDH0R                           REGISTER_32(BXCAN_BASE + 0x18c)
#define CAN_TI1R                            REGISTER_32(BXCAN_BASE + 0x190)
#define CAN_TDT1R                           REGISTER_32(BXCAN_BASE + 0x194)
#define CAN_TDL1R                           REGISTER_32(BXCAN_BASE + 0x198)
#define CAN_TDH1R                           REGISTER_32(BXCAN_BASE + 0x19c)
#define CAN_TI2R                            REGISTER_32(BXCAN_BASE + 0x1a0)
#define CAN_TDT2R                           REGISTER_32(BXCAN_BASE + 0x1a4)
#define CAN_TDL2R                           REGISTER_32(BXCAN_BASE + 0x1a8)
#define CAN_TDH2R                           REGISTER_32(BXCAN_BASE + 0x1ac)
#define CAN_RI0R                            REGISTER_32(BXCAN_BASE + 0x1b0)
#define CAN_RDT0R                           REGISTER_32(BXCAN_BASE + 0x1b4)
#define CAN_RDL0R                           REGISTER_32(BXCAN_BASE + 0x1b8)
#define CAN_RDH0R                           REGISTER_32(BXCAN_BASE + 0x1bc)
#define CAN_RI1R                            REGISTER_32(BXCAN_BASE + 0x1c0)
#define CAN_RDT1R                           REGISTER_32(BXCAN_BASE + 0x1c4)
#define CAN_RDL1R                           REGISTER_32(BXCAN_BASE + 0x1c8)
#define CAN_RDH1R                           REGISTER_32(BXCAN_BASE + 0x1cc)
#define CAN_FMR                             REGISTER_32(BXCAN_BASE + 0x200)
#define CAN_FM1R                            REGISTER_32(BXCAN_BASE + 0x204)
#define CAN_FS1R                            REGISTER_32(BXCAN_BASE + 0x20c)
#define CAN_FFA1R                           REGISTER_32(BXCAN_BASE + 0x214)
#define CAN_FA1R                            REGISTER_32(BXCAN_BASE + 0x21c)

#define CAN_F0R1                            REGISTER_32(BXCAN_BASE + 0x240)
#define CAN_F0R2                            REGISTER_32(BXCAN_BASE + 0x244)
#define CAN_F1R1                            REGISTER_32(BXCAN_BASE + 0x248)
#define CAN_F1R2                            REGISTER_32(BXCAN_BASE + 0x24c)
#define CAN_F2R1                            REGISTER_32(BXCAN_BASE + 0x250)
#define CAN_F2R2                            REGISTER_32(BXCAN_BASE + 0x254)
#define CAN_F3R1                            REGISTER_32(BXCAN_BASE + 0x258)
#define CAN_F3R2                            REGISTER_32(BXCAN_BASE + 0x25c)
#define CAN_F4R1                            REGISTER_32(BXCAN_BASE + 0x260)
#define CAN_F4R2                            REGISTER_32(BXCAN_BASE + 0x264)
#define CAN_F5R1                            REGISTER_32(BXCAN_BASE + 0x268)
#define CAN_F5R2                            REGISTER_32(BXCAN_BASE + 0x26c)
#define CAN_F6R1                            REGISTER_32(BXCAN_BASE + 0x270)
#define CAN_F6R2                            REGISTER_32(BXCAN_BASE + 0x274)
#define CAN_F7R1                            REGISTER_32(BXCAN_BASE + 0x278)
#define CAN_F7R2                            REGISTER_32(BXCAN_BASE + 0x27c)
#define CAN_F8R1                            REGISTER_32(BXCAN_BASE + 0x280)
#define CAN_F8R2                            REGISTER_32(BXCAN_BASE + 0x284)
#define CAN_F9R1                            REGISTER_32(BXCAN_BASE + 0x288)
#define CAN_F9R2                            REGISTER_32(BXCAN_BASE + 0x28c)
#define CAN_F10R1                           REGISTER_32(BXCAN_BASE + 0x290)
#define CAN_F10R2                           REGISTER_32(BXCAN_BASE + 0x294)
#define CAN_F11R1                           REGISTER_32(BXCAN_BASE + 0x298)
#define CAN_F11R2                           REGISTER_32(BXCAN_BASE + 0x29c)
#define CAN_F12R1                           REGISTER_32(BXCAN_BASE + 0x2a0)
#define CAN_F12R2                           REGISTER_32(BXCAN_BASE + 0x2a4)
#define CAN_F13R1                           REGISTER_32(BXCAN_BASE + 0x2a8)
#define CAN_F13R2                           REGISTER_32(BXCAN_BASE + 0x2ac)
#define CAN_F14R1                           REGISTER_32(BXCAN_BASE + 0x2b0)
#define CAN_F14R2                           REGISTER_32(BXCAN_BASE + 0x2b4)
#define CAN_F15R1                           REGISTER_32(BXCAN_BASE + 0x2b8)
#define CAN_F15R2                           REGISTER_32(BXCAN_BASE + 0x2bc)
#define CAN_F16R1                           REGISTER_32(BXCAN_BASE + 0x2c0)
#define CAN_F16R2                           REGISTER_32(BXCAN_BASE + 0x2c4)
#define CAN_F17R1                           REGISTER_32(BXCAN_BASE + 0x2c8)
#define CAN_F17R2                           REGISTER_32(BXCAN_BASE + 0x2cc)
#define CAN_F18R1                           REGISTER_32(BXCAN_BASE + 0x2d0)
#define CAN_F18R2                           REGISTER_32(BXCAN_BASE + 0x2d4)
#define CAN_F19R1                           REGISTER_32(BXCAN_BASE + 0x2d8)
#define CAN_F19R2                           REGISTER_32(BXCAN_BASE + 0x2dc)
#define CAN_F20R1                           REGISTER_32(BXCAN_BASE + 0x2e0)
#define CAN_F20R2                           REGISTER_32(BXCAN_BASE + 0x2e4)
#define CAN_F21R1                           REGISTER_32(BXCAN_BASE + 0x2e8)
#define CAN_F21R2                           REGISTER_32(BXCAN_BASE + 0x2ec)
#define CAN_F22R1                           REGISTER_32(BXCAN_BASE + 0x2f0)
#define CAN_F22R2                           REGISTER_32(BXCAN_BASE + 0x2f4)
#define CAN_F23R1                           REGISTER_32(BXCAN_BASE + 0x2f8)
#define CAN_F23R2                           REGISTER_32(BXCAN_BASE + 0x2fc)
#define CAN_F24R1                           REGISTER_32(BXCAN_BASE + 0x300)
#define CAN_F24R2                           REGISTER_32(BXCAN_BASE + 0x304)
#define CAN_F25R1                           REGISTER_32(BXCAN_BASE + 0x308)
#define CAN_F25R2                           REGISTER_32(BXCAN_BASE + 0x30c)
#define CAN_F26R1                           REGISTER_32(BXCAN_BASE + 0x310)
#define CAN_F26R2                           REGISTER_32(BXCAN_BASE + 0x314)
#define CAN_F27R1                           REGISTER_32(BXCAN_BASE + 0x318)
#define CAN_F27R2                           REGISTER_32(BXCAN_BASE + 0x31c)

//! Option bytes - see reference manual for details.
#define OPTION_WORD0                        REGISTER_32(0x1ffff800)
#define OPTION_WORD1                        REGISTER_32(0x1ffff804)
#define OPTION_WORD2                        REGISTER_32(0x1ffff808)

//! IWDG
#define IWDG_KR                             REGISTER_32(IWDG_BASE + 0)
#define IWDG_PR                             REGISTER_32(IWDG_BASE + 4)
#define IWDG_RLR                            REGISTER_32(IWDG_BASE + 8)
#define IWDG_SR                             REGISTER_32(IWDG_BASE + 0x0c)
#define IWDG_WINR                           REGISTER_32(IWDG_BASE + 0x10)

//! TSC
#define TS_CR                               REGISTER_32(TSC_BASE + 0)
#define TSC_IER                             REGISTER_32(TSC_BASE + 4)
#define TSC_ICR                             REGISTER_32(TSC_BASE + 8)
#define TSC_ISR                             REGISTER_32(TSC_BASE + 0x0c)
#define TSC_IOHCR                           REGISTER_32(TSC_BASE + 0x10)
#define TSC_IOASCR                          REGISTER_32(TSC_BASE + 0x18)
#define TSC_IOSCR                           REGISTER_32(TSC_BASE + 0x20)
#define TSC_IOCCR                           REGISTER_32(TSC_BASE + 0x28)
#define TSC_IOGCSR                          REGISTER_32(TSC_BASE + 0x30)
#define TSC_IOG1CR                          REGISTER_32(TSC_BASE + 0x34)
#define TSC_IOG2CR                          REGISTER_32(TSC_BASE + 0x38)
#define TSC_IOG3CR                          REGISTER_32(TSC_BASE + 0x3c)
#define TSC_IOG4CR                          REGISTER_32(TSC_BASE + 0x40)
#define TSC_IOG5CR                          REGISTER_32(TSC_BASE + 0x44)
#define TSC_IOG6CR                          REGISTER_32(TSC_BASE + 0x48)

//! WWDG
#define WWDG_CR                             REGISTER_32(WWDG_BASE + 0)
#define WWDG_CFR                            REGISTER_32(WWDG_BASE + 4)
#define WWDG_SR                             REGISTER_32(WWDG_BASE + 8)

//! HDMI
#define CEC_CR                              REGISTER_32(HDMI_BASE + 0)
#define CEC_CFGR                            REGISTER_32(HDMI_BASE + 4)
#define CEC_TXDR                            REGISTER_32(HDMI_BASE + 8)
#define CEC_RXDR                            REGISTER_32(HDMI_BASE + 0x0c)
#define CEC_ISR                             REGISTER_32(HDMI_BASE + 0x10)
#define CEC_IER                             REGISTER_32(HDMI_BASE + 0x14)

//! DBG
#define DBGMCU_IDCODE                       REGISTER_32(DBG_BASE + 0)
#define DBGMCU_CR                           REGISTER_32(DBG_BASE + 4)
#define DBGMCU_APB1_FZ                      REGISTER_32(DBG_BASE + 8)
#define DBGMCU_APB2_FZ                      REGISTER_32(DBG_BASE + 0x0c)

//! USB
#define USB_EP0R                            REGISTER_32(USB_DEVICE_FS_BASE + 0)
#define USB_EP1R                            REGISTER_32(USB_DEVICE_FS_BASE + 4)
#define USB_EP2R                            REGISTER_32(USB_DEVICE_FS_BASE + 8)
#define USB_EP3R                            REGISTER_32(USB_DEVICE_FS_BASE + 0x0c)
#define USB_EP4R                            REGISTER_32(USB_DEVICE_FS_BASE + 0x10)
#define USB_EP5R                            REGISTER_32(USB_DEVICE_FS_BASE + 0x14)
#define USB_EP6R                            REGISTER_32(USB_DEVICE_FS_BASE + 0x18)
#define USB_EP7R                            REGISTER_32(USB_DEVICE_FS_BASE + 0x1c)
#define USB_CNTR                            REGISTER_32(USB_DEVICE_FS_BASE + 0x40)
#define USB_ISTR                            REGISTER_32(USB_DEVICE_FS_BASE + 0x44)
#define USB_FNR                             REGISTER_32(USB_DEVICE_FS_BASE + 0x48)
#define USB_DADDR                           REGISTER_32(USB_DEVICE_FS_BASE + 0x4c)
#define USB_BTABLE                          REGISTER_32(USB_DEVICE_FS_BASE + 0x50)

//! SPI1
#define SPI1_CR1                            REGISTER_32(SPI1_BASE + 0)
#define SPI1_CR2                            REGISTER_32(SPI1_BASE + 4)
#define SPI1_SR                             REGISTER_32(SPI1_BASE + 8)
#define SPI1_DR                             REGISTER_16(SPI1_BASE + 0x0c)
#define SPI1_CRCPR                          REGISTER_32(SPI1_BASE + 0x10)
#define SPI1_RXCRCR                         REGISTER_32(SPI1_BASE + 0x14)
#define SPI1_TXCRCR                         REGISTER_32(SPI1_BASE + 0x18)
#define SPI1_I2SCFGR                        REGISTER_32(SPI1_BASE + 0x1c)
#define SPI1_I2SPR                          REGISTER_32(SPI1_BASE + 0x20)

//! SPI2
#define SPI2_CR1                            REGISTER_32(SPI2_BASE + 0)
#define SPI2_CR2                            REGISTER_32(SPI2_BASE + 4)
#define SPI2_SR                             REGISTER_32(SPI2_BASE + 8)
#define SPI2_DR                             REGISTER_32(SPI2_BASE + 0x0c)
#define SPI2_CRCPR                          REGISTER_32(SPI2_BASE + 0x10)
#define SPI2_RXCRCR                         REGISTER_32(SPI2_BASE + 0x14)
#define SPI2_TXCRCR                         REGISTER_32(SPI2_BASE + 0x18)
#define SPI2_I2SCFGR                        REGISTER_32(SPI2_BASE + 0x1c)
#define SPI2_I2SPR                          REGISTER_32(SPI2_BASE + 0x20)

//! SPI3
#define SPI3_CR1                            REGISTER_32(SPI3_BASE + 0)
#define SPI3_CR2                            REGISTER_32(SPI3_BASE + 4)
#define SPI3_SR                             REGISTER_32(SPI3_BASE + 8)
#define SPI3_DR                             REGISTER_32(SPI3_BASE + 0x0c)
#define SPI3_CRCPR                          REGISTER_32(SPI3_BASE + 0x10)
#define SPI3_RXCRCR                         REGISTER_32(SPI3_BASE + 0x14)
#define SPI3_TXCRCR                         REGISTER_32(SPI3_BASE + 0x18)
#define SPI3_I2SCFGR                        REGISTER_32(SPI3_BASE + 0x1c)
#define SPI3_I2SPR                          REGISTER_32(SPI3_BASE + 0x20)

//! I2C1
#define I2C1_CR1                            REGISTER_32(I2C1_BASE + 0)
#define I2C1_CR2                            REGISTER_32(I2C1_BASE + 4)
#define I2C1_OAR1                           REGISTER_32(I2C1_BASE + 8)
#define I2C1_OAR2                           REGISTER_32(I2C1_BASE + 0x0c)
#define I2C1_TIMINGR                        REGISTER_32(I2C1_BASE + 0x10)
#define I2C1_TIMEOUTR                       REGISTER_32(I2C1_BASE + 0x14)
#define I2C1_ISR                            REGISTER_32(I2C1_BASE + 0x18)
#define I2C1_ICR                            REGISTER_32(I2C1_BASE + 0x1c)
#define I2C1_PECR                           REGISTER_32(I2C1_BASE + 0x20)
#define I2C1_RXDR                           REGISTER_8(I2C1_BASE + 0x24)
#define I2C1_TXDR                           REGISTER_8(I2C1_BASE + 0x28)

//! I2C2
#define I2C2_CR1                            REGISTER_32(I2C2_BASE + 0)
#define I2C2_CR2                            REGISTER_32(I2C2_BASE + 4)
#define I2C2_OAR1                           REGISTER_32(I2C2_BASE + 8)
#define I2C2_OAR2                           REGISTER_32(I2C2_BASE + 0x0c)
#define I2C2_TIMINGR                        REGISTER_32(I2C2_BASE + 0x10)
#define I2C2_TIMEOUTR                       REGISTER_32(I2C2_BASE + 0x14)
#define I2C2_ISR                            REGISTER_32(I2C2_BASE + 0x18)
#define I2C2_ICR                            REGISTER_32(I2C2_BASE + 0x1c)
#define I2C2_PECR                           REGISTER_32(I2C2_BASE + 0x20)
#define I2C2_RXDR                           REGISTER_8(I2C2_BASE + 0x24)
#define I2C2_TXDR                           REGISTER_8(I2C2_BASE + 0x28)

//! RTC
#define RTC_TR                              REGISTER_32(RTC_BASE + 0)
#define RTC_DR                              REGISTER_32(RTC_BASE + 4)
#define RTC_CR                              REGISTER_32(RTC_BASE + 8)
#define RTC_ISR                             REGISTER_32(RTC_BASE + 0x0c)
#define RTC_PRER                            REGISTER_32(RTC_BASE + 0x10)
#define RTC_ALRMAR                          REGISTER_32(RTC_BASE + 0x1c)
#define RTC_WPR                             REGISTER_32(RTC_BASE + 0x24)
#define RTC_SSR                             REGISTER_32(RTC_BASE + 0x28)
#define RTC_SHIFTR                          REGISTER_32(RTC_BASE + 0x2c)
#define RTC_TSTR                            REGISTER_32(RTC_BASE + 0x30)
#define RTC_TSDR                            REGISTER_32(RTC_BASE + 0x34)
#define RTC_TSSSR                           REGISTER_32(RTC_BASE + 0x38)
#define RTC_CALR                            REGISTER_32(RTC_BASE + 0x3c)
#define RTC_TAFCR                           REGISTER_32(RTC_BASE + 0x40)
#define RTC_ALRMASSR                        REGISTER_32(RTC_BASE + 0x44)
#define RTC_BKP0R                           REGISTER_32(RTC_BASE + 0x50)
#define RTC_BKP1R                           REGISTER_32(RTC_BASE + 0x54)
#define RTC_BKP2R                           REGISTER_32(RTC_BASE + 0x58)
#define RTC_BKP3R                           REGISTER_32(RTC_BASE + 0x5c)
#define RTC_BKP4R                           REGISTER_32(RTC_BASE + 0x60)

//! IWDG
#define IWDG_KR                             REGISTER_32(IWDG_BASE + 0)
#define IWDG_PR                             REGISTER_32(IWDG_BASE + 4)
#define IWDG_RLR                            REGISTER_32(IWDG_BASE + 8)
#define IWDG_SR                             REGISTER_32(IWDG_BASE + 0x0c)
#define IWDG_WINR                           REGISTER_32(IWDG_BASE + 0x10)

//! WWDG
#define WWDG_CR                             REGISTER_32(WWDG_BASE + 0)
#define WWDG_CFR                            REGISTER_32(WWDG_BASE + 4)
#define WWDG_SR                             REGISTER_32(WWDG_BASE + 8)

#endif
