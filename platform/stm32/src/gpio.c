#include "platform/gpio.h"
#include "kernel/memory.h"

#define OFFSET_MODER       0
#define OFFSET_OTYPER      1
#define OFFSET_OSPEEDR     2
#define OFFSET_PUPDR       3
#define OFFSET_IDR         4
#define OFFSET_ODR         5
#define OFFSET_BSRR        6
#define OFFSET_LCKR        7
#define OFFSET_AFRL        8
#define OFFSET_AFRH        9
#define OFFSET_BRR        10

#ifdef HAS_GPIO_A
    static gpio_iface *gpio_iface_a = NULL;
#endif
#ifdef HAS_GPIO_B
    static gpio_iface *gpio_iface_b = NULL;
#endif
#ifdef HAS_GPIO_C
    static gpio_iface *gpio_iface_c = NULL;
#endif
#ifdef HAS_GPIO_D
    static gpio_iface *gpio_iface_d = NULL;
#endif
#ifdef HAS_GPIO_E
    static gpio_iface *gpio_iface_e = NULL;
#endif
#ifdef HAS_GPIO_F
    static gpio_iface *gpio_iface_f = NULL;
#endif
#ifdef HAS_GPIO_G
    static gpio_iface *gpio_iface_g = NULL;
#endif
#ifdef HAS_GPIO_H
    static gpio_iface *gpio_iface_h = NULL;
#endif

/**
 * GPIO port context
 */
typedef struct gpio_ctx_t {
    gpio_iface iface;
    uint32_t  *register_base;
} gpio_ctx;

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
static void gpio_init_pin(struct gpio_iface_t *iface, uint32_t pin,
    gpio_otype otype, gpio_mode mode, gpio_speed speed, gpio_pupd pupd,
        gpio_af af) {
    gpio_ctx *ctx = (gpio_ctx*) iface;
    //! set mode register
    *(ctx->register_base + OFFSET_MODER)   &= ~(0x3 << (pin * 2));
    *(ctx->register_base + OFFSET_MODER)   |= (mode << (pin * 2));
    //! set otype register
    *(ctx->register_base + OFFSET_OTYPER)  |= (otype << pin);
    //! set ospeed register
    *(ctx->register_base + OFFSET_OSPEEDR) &= ~(0x3 << (pin * 2));
    *(ctx->register_base + OFFSET_OSPEEDR) |= (speed << (pin * 2));
    //! set pull-up/pull-down register
    *(ctx->register_base + OFFSET_PUPDR)   &= ~(0x3 << (pin * 2));
    *(ctx->register_base + OFFSET_PUPDR)   |= (pupd << (pin * 2));
    //! set alternate function (if any)
    if (mode == GPIO_MODE_AF) {
        if (pin < 8) {
            *(ctx->register_base + OFFSET_AFRL) &= ~(0xf << (pin * 4));
            *(ctx->register_base + OFFSET_AFRL) |= (af << (pin * 4));
        } else {
            *(ctx->register_base + OFFSET_AFRH) &= ~(0xf << (pin * 4));
            *(ctx->register_base + OFFSET_AFRH) |= (af << ((pin - 8) * 4));
        }
    }
}

/**
 * @brief      toggle pin output value
 *
 * @param[in]  iface  GPIO interface
 * @param[in]  pin    GPIO pin
 */
static void gpio_toggle(struct gpio_iface_t *iface, uint32_t pin) {
    gpio_ctx *ctx = (gpio_ctx*) iface;

    *(ctx->register_base + OFFSET_ODR) ^= (0x1 << pin);
}

/**
 * @brief      switch pin mode
 *
 * @param[in]  iface  GPIO interface
 * @param[in]  pin    GPIO pin
 * @param[in]  mode   GPIO mode
 */
static void gpio_switch_mode(struct gpio_iface_t *iface, uint32_t pin, gpio_mode mode) {
    gpio_ctx *ctx = (gpio_ctx*) iface;

    //! clean & set mode register
    *(ctx->register_base + OFFSET_MODER) &= ~(0x3 << (pin * 2));
    *(ctx->register_base + OFFSET_MODER) |= (mode << (pin * 2));
}

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
static int gpio_read(struct gpio_iface_t *iface, uint32_t pin) {
    gpio_ctx *ctx = (gpio_ctx*) iface;

    return (*(ctx->register_base + OFFSET_IDR) & (0x1 << pin));
}

/**
 * @brief      write output value of GPIO pin
 *
 * @param      iface  GPIO interface
 * @param[in]  pin    GPIO pin
 * @param      value  pin output value
 *
 * @return     { description_of_the_return_value }
 */
static void gpio_write(struct gpio_iface_t *iface, uint32_t pin, int value) {
    gpio_ctx *ctx = (gpio_ctx*) iface;

    if (value) {
        *(ctx->register_base + OFFSET_ODR) |=  (0x1 << pin);
    } else {
        *(ctx->register_base + OFFSET_ODR) &= ~(0x1 << pin);
    }
}

/**
 * @brief      deallocate GPIO context
 *
 * @param      iface  GPIO interface
 */
static void gpio_iface_destroy(gpio_iface *iface) {
    cell_free(iface);
}

/**
 * @brief      create GPIO context and initialize it
 *
 * @param      port  GPIO port address
 *
 * @return     allocated interface ptr
 */
static gpio_iface *gpio_iface_create(gpio_port port) {
    gpio_ctx *ctx = cell_alloc(sizeof(gpio_ctx));
    if (!ctx) {
        return NULL;
    }
    gpio_iface *iface = &ctx->iface;

    iface->init = gpio_init_pin;
    iface->read = gpio_read;
    iface->write = gpio_write;
    iface->toggle = gpio_toggle;
    iface->mode = gpio_switch_mode;

    ctx->register_base = (uint32_t*) port;

    //! turn on port clock
    switch (port) {
#ifdef HAS_GPIO_A
        case GPIOA:
            RCC_AHBENR |= BIT17;
            break;
#endif
#ifdef HAS_GPIO_B
        case GPIOB:
            RCC_AHBENR |= BIT18;
            break;
#endif
#ifdef HAS_GPIO_C
        case GPIOC:
            RCC_AHBENR |= BIT19;
            break;
#endif
#ifdef HAS_GPIO_D
        case GPIOD:
            RCC_AHBENR |= BIT20;
            break;
#endif
#ifdef HAS_GPIO_E
        case GPIOE:
            RCC_AHBENR |= BIT21;
            break;
#endif
#ifdef HAS_GPIO_F
        case GPIOF:
            RCC_AHBENR |= BIT22;
            break;
#endif
#ifdef HAS_GPIO_G
        case GPIOG:
            RCC_AHBENR |= BIT23;
            break;
#endif
#ifdef HAS_GPIO_H
        case GPIOH:
            RCC_AHBENR |= BIT16;
            break;
#endif
        default:
            gpio_iface_destroy(iface);
            return NULL;
    }

    return iface;
}

void gpio_init(void) {
#ifdef HAS_GPIO_A
    gpio_iface_a = gpio_iface_create(GPIOA);
#endif
#ifdef HAS_GPIO_B
    gpio_iface_b = gpio_iface_create(GPIOB);
#endif
#ifdef HAS_GPIO_C
    gpio_iface_c = gpio_iface_create(GPIOC);
#endif
#ifdef HAS_GPIO_D
    gpio_iface_d = gpio_iface_create(GPIOD);
#endif
#ifdef HAS_GPIO_E
    gpio_iface_e = gpio_iface_create(GPIOE);
#endif
#ifdef HAS_GPIO_F
    gpio_iface_f = gpio_iface_create(GPIOF);
#endif
#ifdef HAS_GPIO_G
    gpio_iface_g = gpio_iface_create(GPIOG);
#endif
#ifdef HAS_GPIO_H
    gpio_iface_h = gpio_iface_create(GPIOH);
#endif
}

gpio_iface *gpio_iface_get(gpio_port port) {
    switch (port) {
#ifdef HAS_GPIO_A
        case GPIOA:
            return gpio_iface_a;
#endif
#ifdef HAS_GPIO_B
        case GPIOB:
            return gpio_iface_b;
            break;
#endif
#ifdef HAS_GPIO_C
        case GPIOC:
            return gpio_iface_c;
#endif
#ifdef HAS_GPIO_D
        case GPIOD:
            return gpio_iface_d;
#endif
#ifdef HAS_GPIO_E
        case GPIOE:
            return gpio_iface_e;
#endif
#ifdef HAS_GPIO_F
        case GPIOF:
            return gpio_iface_f;
#endif
#ifdef HAS_GPIO_G
        case GPIOG:
            return gpio_iface_g;
#endif
#ifdef HAS_GPIO_H
        case GPIOH:
            return gpio_iface_h;
#endif
        default:
            break;
    }
    return NULL;
}
