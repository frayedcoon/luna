#ifndef PLATFORM_GPIO_H
#define PLATFORM_GPIO_H

#include "platform/peripheral.h"
#include "target/cfg.h"

/**
 * adresses of GPIO register sets
 */
typedef enum gpio_port_t {
#ifdef HAS_GPIO_A
    GPIOA = GPIOA_BASE
#endif
#ifdef HAS_GPIO_B
    ,
    GPIOB = GPIOB_BASE
#endif
#ifdef HAS_GPIO_C
    ,
    GPIOC = GPIOC_BASE
#endif
#ifdef HAS_GPIO_D
    ,
    GPIOD = GPIOD_BASE
#endif
#ifdef HAS_GPIO_E
    ,
    GPIOE = GPIOE_BASE
#endif
#ifdef HAS_GPIO_F
    ,
    GPIOF = GPIOF_BASE
#endif
#ifdef HAS_GPIO_G
    ,
    GPIOG = GPIOG_BASE
#endif
#ifdef HAS_GPIO_H
    ,
    GPIOH = GPIOH_BASE
#endif
} gpio_port;

/**
 * pin mode
 */
typedef enum gpio_mode_t {
    GPIO_MODE_INPUT  = 0x0,
    GPIO_MODE_OUTPUT = 0x1,
    GPIO_MODE_AF     = 0x2,
    GPIO_MODE_ANALOG = 0x3
} gpio_mode;

/**
 * pin output type
 */
typedef enum gpio_otype_t {
    GPIO_OTYPE_PUSH_PULL  = 0x0,
    GPIO_OTYPE_OPEN_DRAIN = 0x1
} gpio_otype;

/**
 * pin transmission speed
 */
typedef enum gpio_speed_t {
    GPIO_SPEED_LOW    = 0x0,
    GPIO_SPEED_MEDIUM = 0x1,
    GPIO_SPEED_HIGH   = 0x2,
    GPIO_SPEED_FAST   = 0x3
} gpio_speed;

/**
 * pin pull-up/pull-down mode
 */
typedef enum gpio_pupd_t {
    GPIO_PUPD_NO   = 0x0,
    GPIO_PUPD_UP   = 0x1,
    GPIO_PUPD_DOWN = 0x2
} gpio_pupd;

/**
 * pin alternate functions
 */
typedef enum gpio_af_t {
    GPIO_AF0  = 0x0,
    GPIO_AF1  = 0x1,
    GPIO_AF2  = 0x2,
    GPIO_AF3  = 0x3,
    GPIO_AF4  = 0x4,
    GPIO_AF5  = 0x5,
    GPIO_AF6  = 0x6,
    GPIO_AF7  = 0x7,
    GPIO_AF8  = 0x8,
    GPIO_AF9  = 0x9,
    GPIO_AF10 = 0xa,
    GPIO_AF11 = 0xb,
    GPIO_AF12 = 0xc,
    GPIO_AF13 = 0xd,
    GPIO_AF14 = 0xe,
    GPIO_AF15 = 0xf
} gpio_af;

/**
 * GPIO port interfaces
 */
typedef struct gpio_iface_t {
    /**
     * @brief      Initializes GPIO pin
     *
     * @param[in]  iface  GPIO interface
     * @param[in]  pin    pin number
     * @param[in]  otype  output type
     * @param[in]  mode   outup mode
     * @param[in]  speed  output speed
     * @param[in]  pupd   pull-up/pull-down
     * @param[in]  af     alternate function(ignored if mode is not AF)
     */
    void (*init)(struct gpio_iface_t *iface, uint32_t pin, gpio_otype otype,
        gpio_mode mode, gpio_speed speed, gpio_pupd pupd, gpio_af af);

    /**
     * @brief      read input value of pin
     *
     * @param[in]  iface  GPIO interface
     * @param[in]  pin    GPIO pin
     *
     * @return     pin input value
     * @retval     false  low voltage
     * @retval     true   high voltage
     */
    int (*read)(struct gpio_iface_t *iface, uint32_t pin);

    /**
     * @brief      write output value of GPIO pin
     *
     * @param      iface  GPIO interface
     * @param[in]  pin    GPIO pin
     * @param      value  pin output value
     *
     * @return     { description_of_the_return_value }
     */
    void (*write)(struct gpio_iface_t *iface, uint32_t pin, int value);

    /**
     * @brief      toggle pin output value
     *
     * @param[in]  iface  GPIO interface
     * @param[in]  pin    GPIO pin
     */
    void (*toggle)(struct gpio_iface_t *iface, uint32_t pin);

    /**
     * @brief      switch pin mode
     *
     * @param[in]  iface  GPIO interface
     * @param[in]  pin    GPIO pin
     * @param[in]  mode   GPIO mode
     */
    void (*mode)(struct gpio_iface_t *iface, uint32_t pin, gpio_mode mode);
} gpio_iface;

/**
 * @brief      initialize all present gpio ports
 */
void gpio_init(void);

/**
 * @brief      get GPIO port interface
 *
 * @param[in]  iface  GPIO interface
 *
 * @return     GPIO interface ptr
 */
gpio_iface *gpio_iface_get(gpio_port port);

#endif
