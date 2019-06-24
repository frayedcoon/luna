#include "driver/dht.h"
#include "kernel/thread.h"
#include "kernel/memory.h"
#include "lib/string.h"

#define DHT_CYCLE_STEP    5
#define DHT_CYCLE_TIMEOUT 100

/**
 * dht driver context
 */
typedef struct dht_ctx_t {
    dht_iface     iface;

    gpio_iface   *io_iface;
    uint32_t      pin;

    timer_iface  *tim_iface;

    uint8_t       humidity;
    uint8_t       temperature;

    uint8_t       cycle_iterator;
} __attribute__((packed,aligned(4))) dht_ctx;

/**
 * dht context instance ptr
 */
static dht_ctx *context = NULL;

/**
 * @brief      read data cyles from dht sensor
 *
 * @param      iface  dht driver interface
 *
 * @return     success of read
 */
static int dht_read(dht_iface *iface) {
    dht_ctx *ctx = (dht_ctx*) iface;
    if (!ctx) {
        return 0;
    }

    uint8_t data[5] = {0, 0, 0, 0, 0};

    uint32_t timeout = 0;
    uint8_t cnt = 7;
    uint8_t idx = 0;

    ctx->cycle_iterator = 0;

    ctx->io_iface->mode(ctx->io_iface, ctx->pin, GPIO_MODE_OUTPUT);

    //! put pin to low for 18 msecs
    ctx->io_iface->write(ctx->io_iface, ctx->pin, 0);
    ctx->tim_iface->delay(ctx->tim_iface, TIMER_MILI_SECOND, 18);

    //! put pin to high for 40 usecs
    ctx->io_iface->write(ctx->io_iface, ctx->pin, 1);
    ctx->tim_iface->delay(ctx->tim_iface, TIMER_MICRO_SECOND, 40);

    ctx->io_iface->mode(ctx->io_iface, ctx->pin, GPIO_MODE_INPUT);

    timeout = 0;
    while (!ctx->io_iface->read(ctx->io_iface, ctx->pin)) {
        ctx->tim_iface->delay(ctx->tim_iface, TIMER_MICRO_SECOND,
            DHT_CYCLE_STEP);
        timeout += DHT_CYCLE_STEP;
        if (timeout > DHT_CYCLE_TIMEOUT) {
            return 0;
        }
    }

    timeout = 0;
    while (ctx->io_iface->read(ctx->io_iface, ctx->pin)) {
        ctx->tim_iface->delay(ctx->tim_iface, TIMER_MICRO_SECOND,
            DHT_CYCLE_STEP);
        timeout += DHT_CYCLE_STEP;
        if (timeout > DHT_CYCLE_TIMEOUT) {
            return 0;
        }
    }

    for (ctx->cycle_iterator = 0; ctx->cycle_iterator < 40;
        ctx->cycle_iterator++) {
        timeout = 0;
        while (!ctx->io_iface->read(ctx->io_iface, ctx->pin)) {
            ctx->tim_iface->delay(ctx->tim_iface, TIMER_MICRO_SECOND,
                DHT_CYCLE_STEP);
            timeout += DHT_CYCLE_STEP;
            if (timeout > DHT_CYCLE_TIMEOUT) {
                return 0;
            }
        }

        timeout = 0;
        while (ctx->io_iface->read(ctx->io_iface, ctx->pin)) {
            ctx->tim_iface->delay(ctx->tim_iface, TIMER_MICRO_SECOND,
                DHT_CYCLE_STEP);
            timeout += DHT_CYCLE_STEP;
            if (timeout > DHT_CYCLE_TIMEOUT) {
                return 0;
            }
        }

        if (timeout > 30) {
            data[idx] |= (1 << cnt);
        }
        if (cnt == 0) {
            cnt = 7;
            idx++;
        } else {
            cnt--;
        }
    }

    uint8_t sum = data[0] + data[1] + data[2] + data[3];

    if (sum == data[4] && sum != 0) {
        ctx->humidity    = data[0];
        ctx->temperature = data[2];
    } else {
        return 0;
    }

    return 1;
}

/**
 * @brief      get last valid temperature value
 *
 * @param      iface  dht driver interface
 *
 * @return     temperature value
 */
static uint8_t dht_get_temperature(dht_iface *iface) {
    dht_ctx *ctx = (dht_ctx*) iface;
    if (!ctx) {
        return 0;
    }

    return ctx->temperature;
}

/**
 * @brief      get last valid humidity value
 *
 * @param      iface  dht driver interface
 *
 * @return     humidity value
 */
static uint8_t dht_get_humidity(dht_iface *iface) {
    dht_ctx *ctx = (dht_ctx*) iface;
    if (!ctx) {
        return 0;
    }

    return ctx->humidity;
}

/**
 * @brief      deallocate dht driver context
 *
 * @param      iface  dht driver interface
 */
static void dht_destroy(dht_iface *iface) {
    dht_ctx *ctx = (dht_ctx*) iface;
    if (!ctx) {
        return;
    }

    cell_free(ctx);
}

/**
 * @brief      initialize dht driver
 *
 * @param[in]  config  dht driver configuration
 */
void dht_init(const dht_config *config) {
    if (context) {
        cell_free(context);
    }

    context = cell_alloc(sizeof(dht_ctx));
    if (!context) {
        return;
    }

    //! get dht pin's GPIO interface
    context->io_iface = gpio_iface_get(config->port);
    if (!context->io_iface) {
        cell_free(context);
        context = NULL;
        return;
    }

    //! get timer interface used by driver
    context->tim_iface = timer_iface_get(config->timer);
    if (!context->tim_iface) {
        cell_free(context);
        context = NULL;
        return;
    }

    dht_iface *iface = &context->iface;

    //! iinitialize interface functions
    iface->read            = dht_read;
    iface->get_temperature = dht_get_temperature;
    iface->get_humidity    = dht_get_humidity;
    iface->destroy         = dht_destroy;

    context->pin = config->pin;

    //! initialize GPIO pin
    context->io_iface->init(context->io_iface, context->pin,
        GPIO_OTYPE_PUSH_PULL, GPIO_MODE_OUTPUT, GPIO_SPEED_HIGH, GPIO_PUPD_NO,
            0);

    context->temperature = 0;
    context->humidity    = 0;
}

dht_iface *dht_iface_get(void) {
    return &context->iface;
}
