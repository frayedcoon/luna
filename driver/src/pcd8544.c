#include "driver/pcd8544.h"
#include "kernel/memory.h"
#include "lib/font.h"

#define PCD8544_SCREEN_WIDTH     84
#define PCD8544_SCREEN_HEIGTH    48

#define PCD8544_BYTE_BUFFER_SIZE 504
#define PCD8544_CHR_PER_SCREEN   84

/**
 * lcd sriver context
 */
typedef struct pcd8544_ctx_t {
    pcd8544_iface iface;

    gpio_iface *io_iface;
    uint32_t rst_pin;
    uint32_t ce_pin;
    uint32_t dc_pin;
    uint32_t din_pin;
    uint32_t clk_pin;
} pcd8544_ctx;

/**
 * driver context ptr
 */
static pcd8544_ctx *context = NULL;

/**
 * lcd command types
 */
typedef enum pcd8544_cmd_type_t {
    PCD8544_CMD_TYPE_BASIC                = 0x20,
    PCD8544_CMD_TYPE_EXTENDED             = 0x21
} __attribute__((packed)) pcd8544_cmd_type;

/**
 * @brief      send byte to lcd in reversed bit order
 *
 * @param      ctx   lcd driver context
 * @param[in]  data  data to be sent
 */
static void pcd8544_byte_cycle_write(pcd8544_ctx *ctx, uint8_t data) {
    for (uint8_t i = 0; i < 8; i++)  {
        ctx->io_iface->write(ctx->io_iface, ctx->din_pin,
            !!(data & (0x1 << (7 - i))));

        ctx->io_iface->write(ctx->io_iface, ctx->clk_pin, 1);
        ctx->io_iface->write(ctx->io_iface, ctx->clk_pin, 0);
    }
}

/**
 * @brief      reset lcd display
 *
 * @param      ctx   lcd driver context
 */
static void pcd8544_reset(pcd8544_ctx *ctx) {
    ctx->io_iface->write(ctx->io_iface, ctx->rst_pin, 0);
    ctx->io_iface->write(ctx->io_iface, ctx->rst_pin, 1);
}

/**
 * @brief      inform lcd about comming stream of data
 *
 * @param      ctx   lcd driver context
 */
static void pcd8544_init_data_transmit(pcd8544_ctx *ctx) {
    ctx->io_iface->write(ctx->io_iface, ctx->dc_pin, 1);
}

/**
 * @brief      inform lcd about end on transmition
 *
 * @param      ctx   lcd driver context
 */
static void pcd8544_start_data_transmit(pcd8544_ctx *ctx) {
    ctx->io_iface->write(ctx->io_iface, ctx->ce_pin, 0);
}

/**
 * @brief      inform lcd about start on transmition
 *
 * @param      ctx   lcd driver context
 */
static void pcd8544_end_data_transmit(pcd8544_ctx *ctx) {
    ctx->io_iface->write(ctx->io_iface, ctx->ce_pin, 1);
}

/**
 * @brief      transmit data to lcd
 *
 * @param      ctx   lcd driver context
 * @param[in]  data  data to be sent
 */
static void pcd8544_transmit_data(pcd8544_ctx *ctx, uint8_t data){
    pcd8544_start_data_transmit(ctx);
    pcd8544_byte_cycle_write(ctx, data);
    pcd8544_end_data_transmit(ctx);
}

/**
 * @brief      inform lcd about comming command
 *
 * @param      ctx   lcd driver context
 */
static void pcd8544_init_cmd_transmit(pcd8544_ctx *ctx) {
    ctx->io_iface->write(ctx->io_iface, ctx->dc_pin, 0);
}

/**
 * @brief      send command to lcd
 *
 * @param      ctx   lcd driver context
 * @param[in]  cmd   command code
 */
static void pcd8544_execute_cmd(pcd8544_ctx *ctx, uint8_t cmd) {
    pcd8544_init_cmd_transmit(ctx);
    pcd8544_transmit_data(ctx, cmd);
}

/**
 * @brief      inform lcd about following command
 *
 * @param      ctx       lcd driver context
 * @param[in]  cmd_type  command type (basic/extended)
 */
static void pcd8544_send_cmd(pcd8544_ctx *ctx,
    pcd8544_cmd_type cmd_type) {
    pcd8544_execute_cmd(ctx, cmd_type);
}

/**
 * @brief      set lcd mux rate
 *
 * @param      ctx       lcd driver context
 * @param[in]  mux_rate  mux rate value
 */
static void pcd8544_set_mux_rate(pcd8544_ctx *ctx,
    pcd8544_mux_rate mux_rate) {
    pcd8544_send_cmd(ctx, PCD8544_CMD_TYPE_EXTENDED);
    pcd8544_execute_cmd(ctx , mux_rate);
}

/**
 * @brief      set lcd contrast
 *
 * @param      ctx       lcd driver context
 * @param[in]  contrast  contrast value [0;127]
 */
static void pcd8544_set_contrast(pcd8544_ctx *ctx, int8_t contrast) {
    if (contrast < 0) {
        return;
    }

    pcd8544_send_cmd(ctx, PCD8544_CMD_TYPE_EXTENDED);

    const uint16_t least_value = 128;
    pcd8544_execute_cmd(ctx, (uint8_t)(least_value + contrast));
}

/**
 * @brief      set lcd display mode
 *
 * @param      ctx   lcd driver context
 * @param[in]  mode  The mode
 */
static void pcd8544_set_display_mode(pcd8544_ctx *ctx,
    pcd8544_display_mode mode) {
    pcd8544_send_cmd(ctx, PCD8544_CMD_TYPE_BASIC);
    pcd8544_execute_cmd(ctx, mode);
}

/**
 * @brief      clear lcd screen
 *
 * @param      iface  lcd driver interface
 */
static void pcd8544_clear(pcd8544_iface *iface) {
    pcd8544_ctx *ctx = (pcd8544_ctx*) iface;

    pcd8544_init_data_transmit(ctx);

    int i = PCD8544_BYTE_BUFFER_SIZE;
    while (i > 0) {
        pcd8544_transmit_data(ctx, 0x00);

        i--;
    }
}

/**
 * @brief      draw image on lcd screen
 *
 * @param      iface       lcd driver interface
 * @param[in]  buffer      image buffer
 * @param[in]  buffer_len  image buffer length
 */
static void pcd8544_draw_buffer(pcd8544_iface *iface, const uint8_t *buffer,
    uint32_t buffer_len) {
    pcd8544_ctx *ctx = (pcd8544_ctx*) iface;
    pcd8544_init_data_transmit(ctx);

    unsigned int i = 0;
    while (i < buffer_len && i < PCD8544_BYTE_BUFFER_SIZE) {
        pcd8544_transmit_data(ctx, buffer[i]);

        i++;
    }
}

/**
 * @brief      deallocate lcd driver context
 *
 * @param      iface  lcd driver interface
 */
static void pcd8544_destroy(pcd8544_iface *iface) {
    pcd8544_ctx *ctx = (pcd8544_ctx*) iface;

    cell_free(ctx);
}

/**
 * @brief      put character to lcd screen
 *
 * @param      iface  lcd driver interface
 * @param[in]  chr    desired character
 */
static void pcd8544_putc(pcd8544_iface *iface, char chr) {
    const uint8_t *character_buffer = get_charater_code(chr);
    if (character_buffer) {
        iface->draw(iface, character_buffer, FONT_BYTE_SIZE);
    }
}

/**
 * @brief      print string on lcd
 *
 * @param      iface  lcd driver interface
 * @param[in]  str    target string
 */
static void pcd8544_puts(pcd8544_iface *iface, const char *str) {
    iface->clear(iface);
    uint32_t i = 0;
    while (str[i] && i < PCD8544_CHR_PER_SCREEN) {
        pcd8544_putc(iface, str[i++]);
    }
    while (i < PCD8544_CHR_PER_SCREEN) {
        pcd8544_putc(iface, ' ');
        i++;
    }
}

pcd8544_iface *pcd8544_iface_get(void) {
    return &context->iface;
}

void pcd8544_init(const pcd8544_config *config) {
    if (context) {
        pcd8544_iface *iface = (pcd8544_iface*) context;
        iface->destroy(iface);
        context = NULL;
    }
    context = cell_alloc(sizeof(pcd8544_ctx));
    if (!context) {
        return;
    }

    pcd8544_iface *iface = &context->iface;

    context->io_iface = gpio_iface_get(config->port);
    if (!context->io_iface) {
        cell_free(context);
        context = NULL;
        return;
    }

    context->rst_pin = config->rst_pin;
    context->ce_pin  = config->ce_pin;
    context->dc_pin  = config->dc_pin;
    context->din_pin = config->din_pin;
    context->clk_pin = config->clk_pin;

    context->io_iface->init(context->io_iface, context->rst_pin,
        GPIO_OTYPE_PUSH_PULL, GPIO_MODE_OUTPUT, GPIO_SPEED_HIGH,
            GPIO_PUPD_NO, 0);
    context->io_iface->init(context->io_iface, context->ce_pin,
        GPIO_OTYPE_PUSH_PULL, GPIO_MODE_OUTPUT, GPIO_SPEED_HIGH,
            GPIO_PUPD_NO, 0);
    context->io_iface->init(context->io_iface, context->dc_pin,
        GPIO_OTYPE_PUSH_PULL, GPIO_MODE_OUTPUT, GPIO_SPEED_HIGH,
            GPIO_PUPD_NO, 0);
    context->io_iface->init(context->io_iface, context->din_pin,
        GPIO_OTYPE_PUSH_PULL, GPIO_MODE_OUTPUT, GPIO_SPEED_HIGH,
            GPIO_PUPD_NO, 0);
    context->io_iface->init(context->io_iface, context->clk_pin,
        GPIO_OTYPE_PUSH_PULL, GPIO_MODE_OUTPUT, GPIO_SPEED_HIGH,
            GPIO_PUPD_NO, 0);

    iface->draw    = pcd8544_draw_buffer;
    iface->puts    = pcd8544_puts;
    iface->clear   = pcd8544_clear;
    iface->destroy = pcd8544_destroy;

    pcd8544_reset(context);
    pcd8544_clear(iface);
    pcd8544_set_display_mode(context, config->display_mode);
    pcd8544_set_mux_rate(context, config->mux_rate);
    pcd8544_set_contrast(context, config->contrast);
}
