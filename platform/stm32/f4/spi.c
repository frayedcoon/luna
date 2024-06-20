#include "platform/spi.h"
#include "kernel/syscall.h"

typedef struct spi_regs_t {
    uint32_t cr1;
    uint32_t cr2;
    uint32_t sr;
    uint32_t dr;
    uint32_t crcpr;
    uint32_t rxcrcr;
    uint32_t txcrcr;
    uint32_t i2scfgr;
    uint32_t i2spr;
} spi_regs;

#ifdef HAS_SPI1
    spi_iface *spi_iface_1 = NULL;
#endif
#ifdef HAS_SPI2
    spi_iface *spi_iface_2 = NULL;
#endif
#ifdef HAS_SPI3
    spi_iface *spi_iface_3 = NULL;
#endif
#ifdef HAS_SPI4
    spi_iface *spi_iface_4 = NULL;
#endif

/**
 * spi interface context
 */
typedef struct spi_ctx_t {
    spi_iface iface;

    gpio_iface *io_iface;

    gpio_port port;
    uint32_t cs_pin;
    uint32_t sck_pin;
    uint32_t miso_pin;
    uint32_t mosi_pin;

    spi_regs   *regs;
} spi_ctx;

/**
 * @brief      get address of SPI interface ptr by it's number
 *
 * @param[in]  spi_base  desired SPI
 *
 * @return     address of SPI interface ptr
 */
static spi_iface **spi_base_to_iface(spi spi_base) {
        switch (spi_base) {
#ifdef HAS_SPI1
        case SPI1:
            return &spi_iface_1;
#endif
#ifdef HAS_SPI2
        case SPI2:
            return &spi_iface_2;
#endif
#ifdef HAS_SPI3
        case SPI3:
            return &spi_iface_3;
#endif
#ifdef HAS_SPI4
        case SPI4:
            return &spi_iface_4;
#endif
        default:
            break;
    }

    return NULL;
}

static int transmitter_available(spi_ctx *ctx) {
    return (ctx->regs->sr & BIT(1));
}

static int receiver_available(spi_ctx *ctx) {
    return (ctx->regs->sr & BIT(0));
}

static void spi_set_prescaler(spi_iface *iface, spi_prescaler prescaler) {
    spi_ctx *ctx = (spi_ctx*) iface;
    if (!ctx) {
        return;
    }
    ctx->regs->cr1 &= ~(BIT(5) | BIT(4) | BIT(3));
    ctx->regs->cr1 |= ((uint8_t)prescaler << 3);
}

static int spi_transmit_receive(spi_iface *iface, const uint8_t *tx_buf, uint8_t *rx_buf, uint32_t len) {
    spi_ctx *ctx = (spi_ctx*) iface;
    if (!ctx) {
        return -1;
    }

    while (len) {
        while (!transmitter_available(ctx));
        ctx->regs->dr = tx_buf ? *tx_buf++ : 0xff;
        while (!receiver_available(ctx));
        uint8_t rx = ctx->regs->dr;
        if (rx_buf) {
            *rx_buf++ = rx;
        }
        len--;
    }

    return 0;
}

static int spi_transmit(spi_iface *iface, const uint8_t *data, uint32_t len) {
    spi_ctx *ctx = (spi_ctx*) iface;
    if (!ctx) {
        return -1;
    }

    return spi_transmit_receive(iface, data, NULL, len);
}

static int spi_receive(spi_iface *iface, uint8_t *data, uint32_t len) {
    spi_ctx *ctx = (spi_ctx*) iface;
    if (!ctx) {
        return -1;
    }

    return spi_transmit_receive(iface, NULL, data, len);
}

static void spi_enable(struct spi_iface_t *iface, int enable) {
    spi_ctx *ctx = (spi_ctx*) iface;
    if (!ctx) {
        return;
    }

    if (enable) {
        ctx->regs->cr1 |=  BIT(6);
    } else {
        ctx->regs->cr1 &= ~BIT(6);
    }
}

static void spi_select(struct spi_iface_t *iface, int cs_value) {
    spi_ctx *ctx = (spi_ctx*) iface;
    if (!ctx) {
        return;
    }

    ctx->io_iface->write(ctx->io_iface, ctx->cs_pin, cs_value);
}

static uint8_t spi_txrx_byte(struct spi_iface_t *iface, uint8_t byte) {
    spi_ctx *ctx = (spi_ctx*) iface;
    if (!ctx) {
        return 0;
    }

    uint8_t res = 0;

    spi_transmit_receive(iface, &byte, &res, 1);

    return res;
}

static void spi_tx_byte(struct spi_iface_t *iface, uint8_t byte) {
    spi_ctx *ctx = (spi_ctx*) iface;
    if (!ctx) {
        return;
    }

    spi_txrx_byte(iface, byte);
}

static uint8_t spi_rx_byte(struct spi_iface_t *iface) {
    spi_ctx *ctx = (spi_ctx*) iface;
    if (!ctx) {
        return 0;
    }

    return spi_txrx_byte(iface, 0xff);
}

static void spi_destroy(spi_iface *iface) {
    spi_ctx *ctx_ptr = (spi_ctx*) iface;
    if (ctx_ptr) {
        spi_iface **iface_ptr = spi_base_to_iface((spi) ctx_ptr->regs);

        if (*iface_ptr == iface) {
            free(iface);
            *iface_ptr = NULL;
        }
    }
}

/**
 * @brief      allocate SPI context and initialize interface pointers
 *
 * @param[in]  spi_base  SPI base address
 *
 * @return     SPI interface
 */
static spi_iface *spi_iface_create(spi spi_base) {
    spi_ctx *ctx = malloc(sizeof(spi_ctx));
    if (!ctx) {
        return NULL;
    }
    spi_iface *iface = &ctx->iface;

    iface->enable            = spi_enable;
    iface->tx_byte           = spi_tx_byte;
    iface->rx_byte           = spi_rx_byte;
    iface->set_prescaler     = spi_set_prescaler;
    iface->txrx_byte         = spi_txrx_byte;
    iface->transmit          = spi_transmit;
    iface->receive           = spi_receive;
    iface->transmit_receive  = spi_transmit_receive;
    iface->select            = spi_select;
    iface->destroy           = spi_destroy;

    //! set register base address
    ctx->regs = (spi_regs*) spi_base;

    switch (spi_base) {
#ifdef HAS_SPI1
        case SPI1:
            RCC_APB2ENR |= BIT(12);
            break;
#endif
#ifdef HAS_SPI2
        case SPI2:
            RCC_APB1ENR |= BIT(14);
            break;
#endif
#ifdef HAS_SPI3
        case SPI3:
            RCC_APB1ENR |= BIT(15);
            break;
#endif
#ifdef HAS_SPI4
        case SPI4:
            RCC_APB2ENR |= BIT(13);
            break;
#endif
        default:
            iface->destroy(iface);
            return NULL;
    }
    return iface;
}

/**
 * @brief      initialize SPI by given configuration
 *
 * @param      iface   SPI interface
 * @param[in]  config  SPI configuration
 *
 * @return     success of initialization
 */
static int spi_iface_init(spi_iface *iface, const spi_config *config) {
    spi_ctx *ctx = (spi_ctx*) iface;
    if (!ctx || !config) {
        return 0;
    }

    ctx->io_iface = gpio_iface_get(config->port);
    if (!ctx->io_iface) {
        return 0;
    }

    ctx->cs_pin   = config->cs_pin;
    ctx->sck_pin  = config->sck_pin;
    ctx->miso_pin = config->miso_pin;
    ctx->mosi_pin = config->mosi_pin;

    //! init CS pin
    ctx->io_iface->init(ctx->io_iface, ctx->cs_pin,
        GPIO_OTYPE_PUSH_PULL, GPIO_MODE_OUTPUT, GPIO_SPEED_FAST, GPIO_PUPD_UP, 0);
    //! init SCK pin
    ctx->io_iface->init(ctx->io_iface, ctx->sck_pin,
        GPIO_OTYPE_PUSH_PULL, GPIO_MODE_AF, GPIO_SPEED_HIGH,
            GPIO_PUPD_NO, GPIO_AF5);
    //! init MISO pin
    ctx->io_iface->init(ctx->io_iface, ctx->miso_pin,
        GPIO_OTYPE_PUSH_PULL, GPIO_MODE_AF, GPIO_SPEED_HIGH,
            GPIO_PUPD_NO, GPIO_AF5);
    //! init MOSI pin
    ctx->io_iface->init(ctx->io_iface, ctx->mosi_pin,
        GPIO_OTYPE_PUSH_PULL, GPIO_MODE_AF, GPIO_SPEED_HIGH,
            GPIO_PUPD_NO, GPIO_AF5);

    //! CPOL/CPHA
    ctx->regs->cr1 &= ~(BIT(1) | BIT(0));
    ctx->regs->cr1 |= config->cpol | config->cpha;

    //! mode
    ctx->regs->cr1 |= (config->mode << 2);

    //! baud rate
    ctx->regs->cr1 &= ~(BIT(5) | BIT(4) | BIT(3));
    ctx->regs->cr1 |= (0x07 << 3);

    //! MSB first
    ctx->regs->cr1 &= ~BIT(7);

    //! software slave management
    ctx->regs->cr1 |= (BIT(9) | BIT(8));

    //! full-duplex
    ctx->regs->cr1 &= ~BIT(10);

    //! 8-bit data
    ctx->regs->cr1 &= ~BIT(11);

    return 1;
}

spi_iface *spi_iface_get(spi spi_base) {
    return *(spi_base_to_iface(spi_base));
}

void spi_init(spi spi_base, const spi_config *config) {
    spi_iface **spi_iface = spi_base_to_iface(spi_base);

    *spi_iface = spi_iface_create(spi_base);
    if (!*spi_iface) {
        return;
    }
    if (!spi_iface_init(*spi_iface, config)) {
        (*spi_iface)->destroy(*spi_iface);
    }
}
