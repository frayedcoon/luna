#ifndef BOARD_CFG_H
#define BOARD_CFG_H

#define HAS_GPIO_A
#define HAS_GPIO_B
#define HAS_GPIO_C
#define HAS_GPIO_D
#define HAS_GPIO_E
#define HAS_GPIO_H

#define HAS_SPI1
#define HAS_SPI2
#define HAS_SPI3
#define HAS_SPI4

#define HAS_TIM1
#define HAS_TIM2
#define HAS_TIM3
#define HAS_TIM4
#define HAS_TIM5
#define HAS_TIM9
#define HAS_TIM10
#define HAS_TIM11

#define HAS_USART1
#define HAS_USART2
#define HAS_USART6

#define SPI2_GPIO_PORT    GPIOB
#define SPI2_CS_PIN       12
#define SPI2_CSK_PIN      13
#define SPI2_MISO_PIN     14
#define SPI2_MOSI_PIN     15

#define USART1_PORT       GPIOA
#define USART1_TX_PIN     9
#define USART1_RX_PIN     10

#define USART2_PORT       GPIOA
#define USART2_TX_PIN     2
#define USART2_RX_PIN     3

#define LED_GPIO_PORT     GPIOA
#define LED_GPIO_PIN      5

#define DHT_GPIO_PORT     GPIOB
#define DHT_GPIO_PIN      4
#define DHT_TIMER         TIM1

#define STDIO USART2

#define SD_SPI SPI2

#define DEFAULT_BAUD_RATE 115200

#define PCD8544_PORT      GPIOA
#define PCD8544_PIN_RST   5
#define PCD8544_PIN_CE    6
#define PCD8544_PIN_DC    7
#define PCD8544_PIN_DIN   8
#define PCD8544_PIN_CLK   9

#define CLOCK_FREQ        84000000

#endif
