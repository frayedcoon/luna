#include "platform/usart.h"
#include "arch/core.h"
#include "arch/nvic.h"
#include "kernel/memory.h"
#include "lib/syscall.h"

typedef struct usart_regs_t {
    uint32_t sr;
    uint32_t dr;
    uint32_t brr;
    uint32_t cr1;
    uint32_t cr2;
    uint32_t cr3;
    uint32_t gtpr;
} usart_regs;

#ifdef HAS_USART1
    usart_iface *usart_iface_1 = NULL;
#endif
#ifdef HAS_USART2
    usart_iface *usart_iface_2 = NULL;
#endif
#ifdef HAS_USART3
    usart_iface *usart_iface_3 = NULL;
#endif
#ifdef HAS_USART4
    usart_iface *usart_iface_4 = NULL;
#endif
#ifdef HAS_USART5
    usart_iface *usart_iface_5 = NULL;
#endif

/**
 * usart interface context
 */
typedef struct usart_ctx_t {
    usart_iface iface;

    gpio_iface *io_iface;
    uint32_t    tx_pin;
    uint32_t    rx_pin;

    char       *buffer;
    uint32_t    buffer_idx;
    uint32_t    buffer_size;
    int         buffer_locked;

    usart_regs   *regs;
} usart_ctx;

/**
 * @brief      get address of usart interface ptr by it's number
 *
 * @param[in]  usart_base  desired usart
 *
 * @return     address of usart interface ptr
 */
static usart_iface **usart_base_to_iface(usart usart_base) {
        switch (usart_base) {
#ifdef HAS_USART1
        case USART1:
            return &usart_iface_1;
#endif
#ifdef HAS_USART2
        case USART2:
            return &usart_iface_2;
#endif
#ifdef HAS_USART3
        case USART3:
            return &usart_iface_3;
#endif
#ifdef HAS_USART4
        case USART4:
            return &usart_iface_4;
#endif
#ifdef HAS_USART5
        case USART5:
            return &usart_iface_5;
#endif
        default:
            break;
    }

    return NULL;
}

/**
 * @brief      free memory allocated for usart interface
 *
 * @param      iface  usart interface
 */
static void usart_destroy(usart_iface *iface) {
    usart_ctx *ctx_ptr = (usart_ctx*) iface;
    usart_iface **iface_ptr =
        usart_base_to_iface((usart) ctx_ptr->regs);
    usart_ctx **ctx = (usart_ctx**) iface_ptr;

    if ((*ctx)->buffer) {
        cell_free((*ctx)->buffer);
    }

    *ctx = NULL;
}

/**
 * @brief      initialize usart by given configuration
 *
 * @param      iface   usart interface
 * @param[in]  config  usart configuration
 *
 * @return     success of initialization
 */
static int usart_iface_init(usart_iface *iface, const usart_config *config) {
    usart_ctx *ctx = (usart_ctx*) iface;
    if (!ctx || !config) {
        return 0;
    }

    const uint32_t sys_freq = 42000000;

    const uint32_t mantissa = sys_freq / (config->baud_rate * 16);
    const uint32_t fraction = ((sys_freq % (config->baud_rate * 16)) * 100) / (config->baud_rate * 16);

    ctx->buffer = cell_alloc(config->buffer_size);
    if (!ctx->buffer) {
        return 0;
    }

    ctx->buffer_size  = config->buffer_size;
    ctx->buffer_idx   = 0;
    ctx->buffer_locked = 0;

    ctx->io_iface = gpio_iface_get(config->port);
    if (!ctx->io_iface) {
        return 0;
    }

    ctx->tx_pin = config->tx_pin;
    ctx->rx_pin = config->rx_pin;

    //! init Tx pin
    ctx->io_iface->init(ctx->io_iface, ctx->tx_pin,
        GPIO_OTYPE_PUSH_PULL, GPIO_MODE_AF, GPIO_SPEED_FAST,
            GPIO_PUPD_NO, GPIO_AF7);
    //! init Rx pin
    ctx->io_iface->init(ctx->io_iface, ctx->rx_pin,
        GPIO_OTYPE_PUSH_PULL, GPIO_MODE_AF, GPIO_SPEED_FAST,
            GPIO_PUPD_NO, GPIO_AF7);

    //! set baudrate divisor
    ctx->regs->brr  = fraction | (mantissa << 4);
    //! enable usart, RXNE interrupt, receiver/transmitter
    ctx->regs->cr1 = BIT(13) | BIT(5) | BIT(2) | BIT(3);

    switch ((usart) ctx->regs) {
#ifdef HAS_USART1
        case USART1:
             nvic_it_enable(IT_USART1);
            break;
#endif
#ifdef HAS_USART2
        case USART2:
             nvic_it_enable(IT_USART2);
            break;
#endif
#ifdef HAS_USART3
        case USART3:
             nvic_it_enable(IT_USART3);
            break;
#endif
#ifdef HAS_USART4
        case USART4:
             nvic_it_enable(IT_USART4);
            break;
#endif
#ifdef HAS_USART5
        case USART5:
             nvic_it_enable(IT_USART5);
            break;
#endif
        default:
            iface->destroy(iface);
            return 0;
    }

    return 1;
}

/**
 * @brief      check RXNE usart flag
 *
 * @param      ctx   usart context
 *
 * @return     RXNE flag
 */
static int usart_receiver_available(usart_ctx *ctx) {
    return (ctx->regs->sr & BIT(5));
}

/**
 * @brief      check TXE usart flag
 *
 * @param      ctx   usart context
 *
 * @return     TXE flag
 */
static int usart_transmitter_available(usart_ctx *ctx) {
    return (ctx->regs->sr & BIT(7));
}

/**
 * @brief      put a single charater to usart transmitter
 *
 * @param      iface  usart interface
 * @param[in]  data   character to send
 */
static void usart_putc(usart_iface *iface, char data) {
    usart_ctx *ctx = (usart_ctx*) iface;

    while (!usart_transmitter_available(ctx));

    ctx->regs->dr = data;

    if (data == '\n') {
        usart_putc(iface, '\r');
    }
}

/**
 * @brief      put a sring to usart transmitter with given length
 *
 * @param      iface  usart interface
 * @param[in]  data   string ptr
 * @param[in]  len    string length
 */
static void usart_nputs(usart_iface *iface, const char *data, uint32_t len) {
    char *data_ptr = (char*) data;

    while (*data_ptr && len--) {
        usart_putc(iface, *data_ptr);
        data_ptr++;
    }
}

/**
 * @brief      put a sring to usart transmitter
 *
 * @param      iface  usart interface
 * @param[in]  data   string ptr
 */
static void usart_puts(usart_iface *iface, const char *data) {
    char *data_ptr = (char*) data;

    while (*data_ptr) {
        usart_putc(iface, *data_ptr);
        data_ptr++;
    }
}

/**
 * @brief      check if transfer to usart buffer locked
 *
 * @param      iface  usart inteface
 *
 * @return     locked/unlocked flag
 */
static int usart_buffer_locked(usart_iface *iface) {
    usart_ctx *ctx = (usart_ctx*) iface;

    return ctx->buffer_locked;
}

/**
 * @brief      lock/unclok usart buffer
 *
 * @param      iface   usart interface
 * @param[in]  locked  lock/unlock flag
 */
static void usart_set_buffer_locked(usart_iface *iface, int locked) {
    usart_ctx *ctx = (usart_ctx*) iface;

    ctx->buffer_locked = locked;
}

/**
 * @brief      get usart receiver buffer
 *
 * @param      iface  usart interface
 *
 * @return     usart receiver buffer
 */
static char *usart_buffer_drain(usart_iface *iface) {
    usart_ctx *ctx = (usart_ctx*) iface;

    while (!ctx->buffer_locked) {
        wait(iface);
    }

    //! set null string terminator
    ctx->buffer[ctx->buffer_idx] = 0;

    ctx->buffer_idx = 0;

    return ctx->buffer;
}

/**
 * @brief      fill usart receiver buffer with incomming data
 *
 * @param      iface  usart interface
 */
static int usart_buffer_consume(usart_iface *iface) {
    usart_ctx *ctx = (usart_ctx*) iface;

    int ready = 0;

    while (usart_receiver_available(ctx)) {
        char input = ctx->regs->dr;

        if (ctx->buffer_locked) {
            continue;
        }

        if (ctx->buffer_idx >= ctx->buffer_size) {
            ctx->buffer_idx = 0;
        }

        switch (input) {
            case '\b':
                if (ctx->buffer_idx > 0) {
                    ctx->buffer_idx--;
                    usart_puts(iface, "\b \b");
                }
                break;
            case '\r':
                usart_puts(iface, "\r\n");
                ctx->buffer_locked = 1;
                ready = 1;
                break;
            default:
                ctx->buffer[ctx->buffer_idx] = input;
                usart_putc(iface, input);
                ctx->buffer_idx++;
                break;
        }
    }

    return ready;
}

/**
 * @brief      allocate usart context and initialize interface pointers
 *
 * @param[in]  usart_base  usart base address
 *
 * @return     usart interface
 */
static usart_iface *usart_iface_create(usart usart_base) {
    usart_ctx *ctx = cell_alloc(sizeof(usart_ctx));
    if (!ctx) {
        return NULL;
    }
    usart_iface *iface = &ctx->iface;

    iface->putc              = usart_putc;
    iface->puts              = usart_puts;
    iface->nputs             = usart_nputs;
    iface->buffer_drain      = usart_buffer_drain;
    iface->buffer_consume    = usart_buffer_consume;
    iface->buffer_locked     = usart_buffer_locked;
    iface->set_buffer_locked = usart_set_buffer_locked;
    iface->destroy           = usart_destroy;


    //! set register base address
    ctx->regs = (usart_regs*) usart_base;

    ctx->buffer = NULL;
    ctx->io_iface = NULL;

    switch (usart_base) {
#ifdef HAS_USART1
        case USART1:
            RCC_APB2ENR |= BIT14;
            break;
#endif
#ifdef HAS_USART2
        case USART2:
            RCC_APB1ENR |= BIT17;
            break;
#endif
#ifdef HAS_USART3
        case USART3:
            RCC_APB1ENR |= BIT18;
            break;
#endif
#ifdef HAS_USART4
        case USART4:
            RCC_APB1ENR |= BIT19;
            break;
#endif
#ifdef HAS_USART5
        case USART5:
            RCC_APB1ENR |= BIT20;
            break;
#endif
        default:
            iface->destroy(iface);
            return NULL;
    }
    return iface;
}

usart_iface *usart_iface_get(usart usart_base) {
    return *(usart_base_to_iface(usart_base));
}

void usart_init(usart usart_base, const usart_config *config) {
    usart_iface **usart_iface = usart_base_to_iface(usart_base);

    *usart_iface = usart_iface_create(usart_base);
    if (!*usart_iface) {
        return;
    }
    if (!usart_iface_init(*usart_iface, config)) {
        (*usart_iface)->destroy(*usart_iface);
    }
}
