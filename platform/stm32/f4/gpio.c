#include "platform/gpio.h"
#include "kernel/memory.h"

typedef struct gpio_regs_t {
    uint32_t moder;
    uint32_t otyper;
    uint32_t ospeedr;
    uint32_t pupdr;
    uint32_t idr;
    uint32_t odr;
    uint32_t bsrr;
    uint32_t lckr;
    uint32_t afrl;
    uint32_t afrh;
} __attribute__((packed,aligned(4))) gpio_regs;

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
    gpio_iface  iface;
    gpio_regs  *regs;
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
    ctx->regs->moder   &= ~(0x3 << (pin * 2));
    ctx->regs->moder   |= (mode << (pin * 2));
    //! set otype register
    ctx->regs->otyper  |= (otype << pin);
    //! set ospeed register
    ctx->regs->ospeedr &= ~(0x3 << (pin * 2));
    ctx->regs->ospeedr |= (speed << (pin * 2));
    //! set pull-up/pull-down register
    ctx->regs->pupdr   &= ~(0x3 << (pin * 2));
    ctx->regs->pupdr   |= (pupd << (pin * 2));
    //! set alternate function (if any)
    if (mode == GPIO_MODE_AF) {
        if (pin < 8) {
            ctx->regs->afrl &= ~(0xf << (pin * 4));
            ctx->regs->afrl |= (af << (pin * 4));
        } else {
            ctx->regs->afrh &= ~(0xf << (pin * 4));
            ctx->regs->afrh |= (af << ((pin - 8) * 4));
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

    ctx->regs->odr ^= (0x1 << pin);
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
    ctx->regs->moder &= ~(0x3 << (pin * 2));
    ctx->regs->moder |= (mode << (pin * 2));
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

    return (ctx->regs->idr & (0x1 << pin));
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
        ctx->regs->odr |=  (0x1 << pin);
    } else {
        ctx->regs->odr &= ~(0x1 << pin);
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

    ctx->regs = (gpio_regs*) port;

    //! turn on port clock
    switch (port) {
#ifdef HAS_GPIO_A
        case GPIOA:
            RCC_AHB1ENR |= BIT(0);
            break;
#endif
#ifdef HAS_GPIO_B
        case GPIOB:
            RCC_AHB1ENR |= BIT(1);
            break;
#endif
#ifdef HAS_GPIO_C
        case GPIOC:
            RCC_AHB1ENR |= BIT(2);
            break;
#endif
#ifdef HAS_GPIO_D
        case GPIOD:
            RCC_AHB1ENR |= BIT(3);
            break;
#endif
#ifdef HAS_GPIO_E
        case GPIOE:
            RCC_AHB1ENR |= BIT(4);
            break;
#endif
#ifdef HAS_GPIO_F
        case GPIOF:
            RCC_AHB1ENR |= BIT(5);
            break;
#endif
#ifdef HAS_GPIO_G
        case GPIOG:
            RCC_AHB1ENR |= BIT(6);
            break;
#endif
#ifdef HAS_GPIO_H
        case GPIOH:
            RCC_AHB1ENR |= BIT(7);
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
