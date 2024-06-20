#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include "common/common.h"

//! AHB2 Peripherals
#define USB_OTG_FS_BASE                     0x50000000

//! AHB1 Peripherals
#define CRC_BASE                            0x40023000
#define FLASH_BASE                          0x40023C00
#define RCC_BASE                            0x40023800
#define DMA2_BASE                           0x40026400
#define DMA1_BASE                           0x40026000
#define GPIOH_BASE                          0x40021C00
#define GPIOE_BASE                          0x40021000
#define GPIOD_BASE                          0x40020C00
#define GPIOC_BASE                          0x40020800
#define GPIOB_BASE                          0x40020400
#define GPIOA_BASE                          0x40020000

//! APB2 Peripherals
#define TIM11_BASE                          0x40014800
#define TIM10_BASE                          0x40014400
#define TIM9_BASE                           0x40014000
#define EXTI_BASE                           0x40013C00
#define SYSCFG_BASE                         0x40013800
#define SPI4_BASE                           0x40013400
#define SPI1_BASE                           0x40014000
#define SDIO_BASE                           0x40012C00
#define ADC1_BASE                           0x40012000
#define USART6_BASE                         0x40011400
#define USART1_BASE                         0x40011000
#define TIM1_BASE                           0x40010000

//! APB1 Peripherals
#define PWR_BASE                            0x40007000
#define I2C3_BASE                           0x40005C00
#define I2C2_BASE                           0x40005800
#define I2C1_BASE                           0x40005400
#define USART2_BASE                         0x40004400
#define I2S3EXT_BASE                        0x40004000
#define SPI3_BASE                           0x40003c00
#define SPI2_BASE                           0x40003800
#define I2S2EXT_BASE                        0x40003400
#define IWDG_BASE                           0x40003000
#define WWDG_BASE                           0x40002c00
#define RTC_BASE                            0x40002800
#define TIM5_BASE                           0x40000C00
#define TIM4_BASE                           0x40000800
#define TIM3_BASE                           0x40000400
#define TIM2_BASE                           0x40000000

//! FLASH
#define FLASH_ACR                           REGISTER_32(FLASH_BASE + 0)
#define FLASH_KEYR                          REGISTER_32(FLASH_BASE + 4)
#define FLASH_OPTKEYR                       REGISTER_32(FLASH_BASE + 8)
#define FLASH_SR                            REGISTER_32(FLASH_BASE + 0x0c)
#define FLASH_CR                            REGISTER_32(FLASH_BASE + 0x10)
#define FLASH_OPTCR                         REGISTER_32(FLASH_BASE + 0x14)

//! CRC
#define CRC_DR                              REGISTER_32(CRC_BASE + 0)
#define CRC_IDR                             REGISTER_32(CRC_BASE + 4)
#define CRC_CR                              REGISTER_32(CRC_BASE + 8)

//! PWR
#define PWR_CR                              REGISTER_32(PWR_BASE + 0)
#define PWR_CSR                             REGISTER_32(PWR_BASE + 4)

//! RCC registers
#define RCC_CR                              REGISTER_32(RCC_BASE + 0)
#define RCC_PLLCFGR                         REGISTER_32(RCC_BASE + 4)
#define RCC_CFGR                            REGISTER_32(RCC_BASE + 8)
#define RCC_CIR                             REGISTER_32(RCC_BASE + 0x0c)
#define RCC_AHB1RSTR                        REGISTER_32(RCC_BASE + 0x10)
#define RCC_AHB2RSTR                        REGISTER_32(RCC_BASE + 0x14)
#define RCC_APB1RSTR                        REGISTER_32(RCC_BASE + 0x20)
#define RCC_APB2RSTR                        REGISTER_32(RCC_BASE + 0x24)
#define RCC_AHB1ENR                         REGISTER_32(RCC_BASE + 0x30)
#define RCC_AHB2ENR                         REGISTER_32(RCC_BASE + 0x34)
#define RCC_APB1ENR                         REGISTER_32(RCC_BASE + 0x40)
#define RCC_APB2ENR                         REGISTER_32(RCC_BASE + 0x44)
#define RCC_AHB1LPENR                       REGISTER_32(RCC_BASE + 0x50)
#define RCC_AHB2LPENR                       REGISTER_32(RCC_BASE + 0x54)
#define RCC_APB1LPENR                       REGISTER_32(RCC_BASE + 0x60)
#define RCC_APB2LPENR                       REGISTER_32(RCC_BASE + 0x64)
#define RCC_BDCR                            REGISTER_32(RCC_BASE + 0x70)
#define RCC_CSR                             REGISTER_32(RCC_BASE + 0x74)
#define RCC_SSCGR                           REGISTER_32(RCC_BASE + 0x80)
#define RCC_PLLI2SCFGR                      REGISTER_32(RCC_BASE + 0x84)
#define RCC_DCKCFGR                         REGISTER_32(RCC_BASE + 0x8C)

//! DMA1
#define DMA1_LISR                           REGISTER_32(DMA1_BASE + 0)
#define DMA1_HISR                           REGISTER_32(DMA1_BASE + 4)
#define DMA1_LIFCR                          REGISTER_32(DMA1_BASE + 8)
#define DMA1_HIFCR                          REGISTER_32(DMA1_BASE + 0x0C)

#define DMA1_S0CR                           REGISTER_32(DMA1_BASE + 0x10)
#define DMA1_S0NDTR                         REGISTER_32(DMA1_BASE + 0x14)
#define DMA1_S0PAR                          REGISTER_32(DMA1_BASE + 0x18)
#define DMA1_S0M0AR                         REGISTER_32(DMA1_BASE + 0x1C)
#define DMA1_S0M1AR                         REGISTER_32(DMA1_BASE + 0x20)
#define DMA1_S0FCR                          REGISTER_32(DMA1_BASE + 0x24)

#define DMA1_S1CR                           REGISTER_32(DMA1_BASE + 0x28)
#define DMA1_S1NDTR                         REGISTER_32(DMA1_BASE + 0x2C)
#define DMA1_S1PAR                          REGISTER_32(DMA1_BASE + 0x30)
#define DMA1_S1M0AR                         REGISTER_32(DMA1_BASE + 0x34)
#define DMA1_S1M1AR                         REGISTER_32(DMA1_BASE + 0x38)
#define DMA1_S1FCR                          REGISTER_32(DMA1_BASE + 0x3C)

#define DMA1_S2CR                           REGISTER_32(DMA1_BASE + 0x40)
#define DMA1_S2NDTR                         REGISTER_32(DMA1_BASE + 0x44)
#define DMA1_S2PAR                          REGISTER_32(DMA1_BASE + 0x48)
#define DMA1_S2M0AR                         REGISTER_32(DMA1_BASE + 0x4C)
#define DMA1_S2M1AR                         REGISTER_32(DMA1_BASE + 0x50)
#define DMA1_S2FCR                          REGISTER_32(DMA1_BASE + 0x54)

#define DMA1_S3CR                           REGISTER_32(DMA1_BASE + 0x58)
#define DMA1_S3NDTR                         REGISTER_32(DMA1_BASE + 0x5C)
#define DMA1_S3PAR                          REGISTER_32(DMA1_BASE + 0x60)
#define DMA1_S3M0AR                         REGISTER_32(DMA1_BASE + 0x64)
#define DMA1_S3M1AR                         REGISTER_32(DMA1_BASE + 0x68)
#define DMA1_S3FCR                          REGISTER_32(DMA1_BASE + 0x6C)

#define DMA1_S4CR                           REGISTER_32(DMA1_BASE + 0x70)
#define DMA1_S4NDTR                         REGISTER_32(DMA1_BASE + 0x74)
#define DMA1_S4PAR                          REGISTER_32(DMA1_BASE + 0x78)
#define DMA1_S4M0AR                         REGISTER_32(DMA1_BASE + 0x7C)
#define DMA1_S4M1AR                         REGISTER_32(DMA1_BASE + 0x80)
#define DMA1_S4FCR                          REGISTER_32(DMA1_BASE + 0x84)

#define DMA1_S5CR                           REGISTER_32(DMA1_BASE + 0x88)
#define DMA1_S5NDTR                         REGISTER_32(DMA1_BASE + 0x8C)
#define DMA1_S5PAR                          REGISTER_32(DMA1_BASE + 0x90)
#define DMA1_S5M0AR                         REGISTER_32(DMA1_BASE + 0x94)
#define DMA1_S5M1AR                         REGISTER_32(DMA1_BASE + 0x98)
#define DMA1_S5FCR                          REGISTER_32(DMA1_BASE + 0x9C)

#define DMA1_S6CR                           REGISTER_32(DMA1_BASE + 0xA0)
#define DMA1_S6NDTR                         REGISTER_32(DMA1_BASE + 0xA4)
#define DMA1_S6PAR                          REGISTER_32(DMA1_BASE + 0xA8)
#define DMA1_S6M0AR                         REGISTER_32(DMA1_BASE + 0xAC)
#define DMA1_S6M1AR                         REGISTER_32(DMA1_BASE + 0xB0)
#define DMA1_S6FCR                          REGISTER_32(DMA1_BASE + 0xB4)

#define DMA1_S7CR                           REGISTER_32(DMA1_BASE + 0xB8)
#define DMA1_S7NDTR                         REGISTER_32(DMA1_BASE + 0xBC)
#define DMA1_S7PAR                          REGISTER_32(DMA1_BASE + 0xC0)
#define DMA1_S7M0AR                         REGISTER_32(DMA1_BASE + 0xC4)
#define DMA1_S7M1AR                         REGISTER_32(DMA1_BASE + 0xC8)
#define DMA1_S7FCR                          REGISTER_32(DMA1_BASE + 0xCC)

//! DMA2
#define DMA2_LISR                           REGISTER_32(DMA2_BASE + 0)
#define DMA2_HISR                           REGISTER_32(DMA2_BASE + 4)
#define DMA2_LIFCR                          REGISTER_32(DMA2_BASE + 8)
#define DMA2_HIFCR                          REGISTER_32(DMA2_BASE + 0x0C)

#define DMA2_S0CR                           REGISTER_32(DMA2_BASE + 0x10)
#define DMA2_S0NDTR                         REGISTER_32(DMA2_BASE + 0x14)
#define DMA2_S0PAR                          REGISTER_32(DMA2_BASE + 0x18)
#define DMA2_S0M0AR                         REGISTER_32(DMA2_BASE + 0x1C)
#define DMA2_S0M1AR                         REGISTER_32(DMA2_BASE + 0x20)
#define DMA2_S0FCR                          REGISTER_32(DMA2_BASE + 0x24)

#define DMA2_S1CR                           REGISTER_32(DMA2_BASE + 0x28)
#define DMA2_S1NDTR                         REGISTER_32(DMA2_BASE + 0x2C)
#define DMA2_S1PAR                          REGISTER_32(DMA2_BASE + 0x30)
#define DMA2_S1M0AR                         REGISTER_32(DMA2_BASE + 0x34)
#define DMA2_S1M1AR                         REGISTER_32(DMA2_BASE + 0x38)
#define DMA2_S1FCR                          REGISTER_32(DMA2_BASE + 0x3C)

#define DMA2_S2CR                           REGISTER_32(DMA2_BASE + 0x40)
#define DMA2_S2NDTR                         REGISTER_32(DMA2_BASE + 0x44)
#define DMA2_S2PAR                          REGISTER_32(DMA2_BASE + 0x48)
#define DMA2_S2M0AR                         REGISTER_32(DMA2_BASE + 0x4C)
#define DMA2_S2M1AR                         REGISTER_32(DMA2_BASE + 0x50)
#define DMA2_S2FCR                          REGISTER_32(DMA2_BASE + 0x54)

#define DMA2_S3CR                           REGISTER_32(DMA2_BASE + 0x58)
#define DMA2_S3NDTR                         REGISTER_32(DMA2_BASE + 0x5C)
#define DMA2_S3PAR                          REGISTER_32(DMA2_BASE + 0x60)
#define DMA2_S3M0AR                         REGISTER_32(DMA2_BASE + 0x64)
#define DMA2_S3M1AR                         REGISTER_32(DMA2_BASE + 0x68)
#define DMA2_S3FCR                          REGISTER_32(DMA2_BASE + 0x6C)

#define DMA2_S4CR                           REGISTER_32(DMA2_BASE + 0x70)
#define DMA2_S4NDTR                         REGISTER_32(DMA2_BASE + 0x74)
#define DMA2_S4PAR                          REGISTER_32(DMA2_BASE + 0x78)
#define DMA2_S4M0AR                         REGISTER_32(DMA2_BASE + 0x7C)
#define DMA2_S4M1AR                         REGISTER_32(DMA2_BASE + 0x80)
#define DMA2_S4FCR                          REGISTER_32(DMA2_BASE + 0x84)

#define DMA2_S5CR                           REGISTER_32(DMA2_BASE + 0x88)
#define DMA2_S5NDTR                         REGISTER_32(DMA2_BASE + 0x8C)
#define DMA2_S5PAR                          REGISTER_32(DMA2_BASE + 0x90)
#define DMA2_S5M0AR                         REGISTER_32(DMA2_BASE + 0x94)
#define DMA2_S5M1AR                         REGISTER_32(DMA2_BASE + 0x98)
#define DMA2_S5FCR                          REGISTER_32(DMA2_BASE + 0x9C)

#define DMA2_S6CR                           REGISTER_32(DMA2_BASE + 0xA0)
#define DMA2_S6NDTR                         REGISTER_32(DMA2_BASE + 0xA4)
#define DMA2_S6PAR                          REGISTER_32(DMA2_BASE + 0xA8)
#define DMA2_S6M0AR                         REGISTER_32(DMA2_BASE + 0xAC)
#define DMA2_S6M1AR                         REGISTER_32(DMA2_BASE + 0xB0)
#define DMA2_S6FCR                          REGISTER_32(DMA2_BASE + 0xB4)

#define DMA2_S7CR                           REGISTER_32(DMA2_BASE + 0xB8)
#define DMA2_S7NDTR                         REGISTER_32(DMA2_BASE + 0xBC)
#define DMA2_S7PAR                          REGISTER_32(DMA2_BASE + 0xC0)
#define DMA2_S7M0AR                         REGISTER_32(DMA2_BASE + 0xC4)
#define DMA2_S7M1AR                         REGISTER_32(DMA2_BASE + 0xC8)
#define DMA2_S7FCR                          REGISTER_32(DMA2_BASE + 0xCC)

//! SYS Config
#define SYSCFG_MEMRMP                       REGISTER_32(SYSCFG_BASE + 0)
#define SYSCFG_PMC                          REGISTER_32(SYSCFG_BASE + 4)
#define SYSCFG_EXTICR1                      REGISTER_32(SYSCFG_BASE + 8)
#define SYSCFG_EXTICR2                      REGISTER_32(SYSCFG_BASE + 0x0c)
#define SYSCFG_EXTICR3                      REGISTER_32(SYSCFG_BASE + 0x10)
#define SYSCFG_EXTICR4                      REGISTER_32(SYSCFG_BASE + 0x14)
#define SYSCFG_CMPCR                        REGISTER_32(SYSCFG_BASE + 0x18)

//! EXTI
#define EXTI_IMR                            REGISTER_32(EXTI_BASE + 0)
#define EXTI_EMR                            REGISTER_32(EXTI_BASE + 4)
#define EXTI_RTSR                           REGISTER_32(EXTI_BASE + 8)
#define EXTI_FTSR                           REGISTER_32(EXTI_BASE + 0x0c)
#define EXTI_SWIER                          REGISTER_32(EXTI_BASE + 0x10)
#define EXTI_PR                             REGISTER_32(EXTI_BASE + 0x14)

//! IWDG
#define IWDG_KR                             REGISTER_32(IWDG_BASE + 0)
#define IWDG_PR                             REGISTER_32(IWDG_BASE + 4)
#define IWDG_RLR                            REGISTER_32(IWDG_BASE + 8)
#define IWDG_SR                             REGISTER_32(IWDG_BASE + 0x0c)

//! WWDG
#define WWDG_CR                             REGISTER_32(WWDG_BASE + 0)
#define WWDG_CFR                            REGISTER_32(WWDG_BASE + 4)
#define WWDG_SR                             REGISTER_32(WWDG_BASE + 8)

//! DBG
#define DBG_BASE                            0xE0042000
#define DBGMCU_IDCODE                       REGISTER_32(DBG_BASE + 0)
#define DBGMCU_CR                           REGISTER_32(DBG_BASE + 4)
#define DBGMCU_APB1_FZ                      REGISTER_32(DBG_BASE + 8)
#define DBGMCU_APB2_FZ                      REGISTER_32(DBG_BASE + 0x0c)

//! SPI1
#define SPI1_CR1                            REGISTER_32(SPI1_BASE + 0)
#define SPI1_SR                             REGISTER_32(SPI1_BASE + 8)
#define SPI1_DR                             REGISTER_32(SPI1_BASE + 0x0c)
#define SPI1_CRCPR                          REGISTER_32(SPI1_BASE + 0x10)
#define SPI1_RXCRCR                         REGISTER_32(SPI1_BASE + 0x14)
#define SPI1_TXCRCR                         REGISTER_32(SPI1_BASE + 0x18)
#define SPI1_I2SCFGR                        REGISTER_32(SPI1_BASE + 0x1c)
#define SPI1_I2SPR                          REGISTER_32(SPI1_BASE + 0x20)

//! SPI2
#define SPI2_CR1                            REGISTER_32(SPI2_BASE + 0)
#define SPI2_SR                             REGISTER_32(SPI2_BASE + 8)
#define SPI2_DR                             REGISTER_32(SPI2_BASE + 0x0c)
#define SPI2_CRCPR                          REGISTER_32(SPI2_BASE + 0x10)
#define SPI2_RXCRCR                         REGISTER_32(SPI2_BASE + 0x14)
#define SPI2_TXCRCR                         REGISTER_32(SPI2_BASE + 0x18)
#define SPI2_I2SCFGR                        REGISTER_32(SPI2_BASE + 0x1c)
#define SPI2_I2SPR                          REGISTER_32(SPI2_BASE + 0x20)

//! SPI3
#define SPI3_CR1                            REGISTER_32(SPI3_BASE + 0)
#define SPI3_SR                             REGISTER_32(SPI3_BASE + 8)
#define SPI3_DR                             REGISTER_32(SPI3_BASE + 0x0c)
#define SPI3_CRCPR                          REGISTER_32(SPI3_BASE + 0x10)
#define SPI3_RXCRCR                         REGISTER_32(SPI3_BASE + 0x14)
#define SPI3_TXCRCR                         REGISTER_32(SPI3_BASE + 0x18)
#define SPI3_I2SCFGR                        REGISTER_32(SPI3_BASE + 0x1c)
#define SPI3_I2SPR                          REGISTER_32(SPI3_BASE + 0x20)

//! SPI4
#define SPI4_CR1                            REGISTER_32(SPI4_BASE + 0)
#define SPI4_SR                             REGISTER_32(SPI4_BASE + 8)
#define SPI4_DR                             REGISTER_32(SPI4_BASE + 0x0c)
#define SPI4_CRCPR                          REGISTER_32(SPI4_BASE + 0x10)
#define SPI4_RXCRCR                         REGISTER_32(SPI4_BASE + 0x14)
#define SPI4_TXCRCR                         REGISTER_32(SPI4_BASE + 0x18)
#define SPI4_I2SCFGR                        REGISTER_32(SPI4_BASE + 0x1c)
#define SPI4_I2SPR                          REGISTER_32(SPI4_BASE + 0x20)

//! I2C1
#define I2C1_CR1                            REGISTER_32(I2C1_BASE + 0)
#define I2C1_CR2                            REGISTER_32(I2C1_BASE + 4)
#define I2C1_OAR1                           REGISTER_32(I2C1_BASE + 8)
#define I2C1_OAR2                           REGISTER_32(I2C1_BASE + 0x0c)
#define I2C1_DR                             REGISTER_32(I2C1_BASE + 0x10)
#define I2C1_SR1                            REGISTER_32(I2C1_BASE + 0x14)
#define I2C1_SR2                            REGISTER_32(I2C1_BASE + 0x18)
#define I2C1_CCR                            REGISTER_32(I2C1_BASE + 0x1c)
#define I2C1_TRISE                          REGISTER_32(I2C1_BASE + 0x20)
#define I2C1_FLTR                           REGISTER_32(I2C1_BASE + 0x24)

//! I2C2
#define I2C2_CR1                            REGISTER_32(I2C2_BASE + 0)
#define I2C2_CR2                            REGISTER_32(I2C2_BASE + 4)
#define I2C2_OAR1                           REGISTER_32(I2C2_BASE + 8)
#define I2C2_OAR2                           REGISTER_32(I2C2_BASE + 0x0c)
#define I2C2_DR                             REGISTER_32(I2C2_BASE + 0x10)
#define I2C2_SR1                            REGISTER_32(I2C2_BASE + 0x14)
#define I2C2_SR2                            REGISTER_32(I2C2_BASE + 0x18)
#define I2C2_CCR                            REGISTER_32(I2C2_BASE + 0x1c)
#define I2C2_TRISE                          REGISTER_32(I2C2_BASE + 0x20)
#define I2C2_FLTR                           REGISTER_32(I2C2_BASE + 0x24)

//! I2C3
#define I2C3_CR1                            REGISTER_32(I2C3_BASE + 0)
#define I2C3_CR2                            REGISTER_32(I2C3_BASE + 4)
#define I2C3_OAR1                           REGISTER_32(I2C3_BASE + 8)
#define I2C3_OAR2                           REGISTER_32(I2C3_BASE + 0x0c)
#define I2C3_DR                             REGISTER_32(I2C3_BASE + 0x10)
#define I2C3_SR1                            REGISTER_32(I2C3_BASE + 0x14)
#define I2C3_SR2                            REGISTER_32(I2C3_BASE + 0x18)
#define I2C3_CCR                            REGISTER_32(I2C3_BASE + 0x1c)
#define I2C3_TRISE                          REGISTER_32(I2C3_BASE + 0x20)
#define I2C3_FLTR                           REGISTER_32(I2C3_BASE + 0x24)

//! RTC
#define RTC_TR                              REGISTER_32(RTC_BASE + 0)
#define RTC_DR                              REGISTER_32(RTC_BASE + 4)
#define RTC_CR                              REGISTER_32(RTC_BASE + 8)
#define RTC_ISR                             REGISTER_32(RTC_BASE + 0x0c)
#define RTC_PRER                            REGISTER_32(RTC_BASE + 0x10)
#define RTC_WUTR                            REGISTER_32(RTC_BASE + 0x14)
#define RTC_CALIBR                          REGISTER_32(RTC_BASE + 0x18)
#define RTC_ALRMAR                          REGISTER_32(RTC_BASE + 0x1c)
#define RTC_ALRMBR                          REGISTER_32(RTC_BASE + 0x20)
#define RTC_WPR                             REGISTER_32(RTC_BASE + 0x24)
#define RTC_SSR                             REGISTER_32(RTC_BASE + 0x28)
#define RTC_TSTR                            REGISTER_32(RTC_BASE + 0x30)
#define RTC_TSSSR                           REGISTER_32(RTC_BASE + 0x38)
#define RTC_CALR                            REGISTER_32(RTC_BASE + 0x3c)
#define RTC_TAFCR                           REGISTER_32(RTC_BASE + 0x40)
#define RTC_ALRMASSR                        REGISTER_32(RTC_BASE + 0x44)
#define RTC_ALRMBSSR                        REGISTER_32(RTC_BASE + 0x48)
#define RTC_BKP0R                           REGISTER_32(RTC_BASE + 0x50)
#define RTC_BKP1R                           REGISTER_32(RTC_BASE + 0x54)
#define RTC_BKP2R                           REGISTER_32(RTC_BASE + 0x58)
#define RTC_BKP3R                           REGISTER_32(RTC_BASE + 0x5c)
#define RTC_BKP4R                           REGISTER_32(RTC_BASE + 0x60)
#define RTC_BKP5R                           REGISTER_32(RTC_BASE + 0x64)
#define RTC_BKP6R                           REGISTER_32(RTC_BASE + 0x68)
#define RTC_BKP7R                           REGISTER_32(RTC_BASE + 0x6c)
#define RTC_BKP8R                           REGISTER_32(RTC_BASE + 0x70)
#define RTC_BKP9R                           REGISTER_32(RTC_BASE + 0x74)
#define RTC_BKP10R                          REGISTER_32(RTC_BASE + 0x78)
#define RTC_BKP11R                          REGISTER_32(RTC_BASE + 0x7c)
#define RTC_BKP12R                          REGISTER_32(RTC_BASE + 0x80)
#define RTC_BKP13R                          REGISTER_32(RTC_BASE + 0x84)
#define RTC_BKP14R                          REGISTER_32(RTC_BASE + 0x88)
#define RTC_BKP15R                          REGISTER_32(RTC_BASE + 0x8c)
#define RTC_BKP16R                          REGISTER_32(RTC_BASE + 0x90)
#define RTC_BKP17R                          REGISTER_32(RTC_BASE + 0x94)
#define RTC_BKP18R                          REGISTER_32(RTC_BASE + 0x98)
#define RTC_BKP19R                          REGISTER_32(RTC_BASE + 0x9c)

#endif
