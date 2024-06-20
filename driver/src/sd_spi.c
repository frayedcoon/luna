#include "driver/sd_spi.h"
#include "target/cfg.h"
#include "kernel/syscall.h"
#include "platform/clock.h"

#define CMD_RECV_TRY_CNT   0x800
#define CMD_NUM_TRY        0x80

#define CMD_OFFSET         0x40
#define CMD_TX_SIZE        0x06
#define CMD_RX_SIZE        0x04

#define CMD8_PARAM         0x000001AA

#define ACMD41_TMOUT_MSEC  1000
#define READ_TIMEOUT_MSEC  100
#define WRITE_TIMEOUT_MSEC 250

#define OCR_HC_FLAG        (1 << 30)

#define REG_SIZE           0x10

#define START_BLOCK_TOK    0xfe
#define DATA_ACCEPTED_TOK  0x05

#define SD_BLOCK_SIZE 512

#define FLAG_GET(_resp, _flag) ((_resp) & (_flag))

//! CS low
#define SPI_ON(_ifc)  ((_ifc)->select((_ifc), 0))
//! CS high
#define SPI_OFF(_ifc) ((_ifc)->select((_ifc), 1))

typedef enum sd_spi_type_t {
    SD_NONE    = 0x00,
    SD_V1      = 0x01,
    SD_V2      = 0x02,
    SD_MMC     = 0x03,
    SD_SDHC    = 0x04,
} sd_spi_type;

typedef struct sd_spi_stat_t {
    sd_spi_type   type;
    uint32_t      size_mb;
} sd_spi_stat;

typedef struct sd_spi_ctx_t {
    disk_ifc         ifc;
    sd_spi_stat      stat;

    spi_iface       *spi;
} sd_spi_ctx;

typedef enum sd_r1_resp_t {
    SD_R1_IDLE          = (1 << 0),
    SD_R1_ERASE_RESET   = (1 << 1),
    SD_R1_ILLEGAL_CMD   = (1 << 2),
    SD_R1_CRC_ERR       = (1 << 3),
    SD_R1_ERASE_ERR     = (1 << 4),
    SD_R1_ADDR_ERR      = (1 << 5),
    SD_R1_PARAM_ERROR   = (1 << 6),
    SD_R1_INVALID       = (1 << 7),
} sd_r1_resp;

static uint8_t crc7_update(uint8_t crc, uint8_t data) {

    crc ^= data;
    for (int i = 0; i < 8; i++) {
        if (crc & 0x80) {
            crc ^= 0x89;
        }
        crc <<= 1;
    }

    return crc;
}

static uint8_t crc7(const uint8_t *buf, uint32_t buf_size) {
    uint8_t crc = 0;

    while (buf_size--) {
        crc = crc7_update(crc, *buf++);
    }

    //! last bit should be set
    return (crc | 0x01);
}

uint16_t crc16_update(uint16_t crc, uint8_t data) {
    crc  = (uint8_t)(crc >> 8)|(crc << 8);
    crc ^=  data;
    crc ^= (uint8_t)(crc & 0xff) >> 4;
    crc ^= (crc << 8) << 4;
    crc ^= ((crc & 0xff) << 4) << 1;

    return crc;
}

uint16_t crc16(const uint8_t *buf, uint32_t buf_size) {
    uint16_t crc = 0;

    while (buf_size--) {
        crc = crc16_update(crc, *buf++);
    }

    return crc;
}

static uint8_t sd_cmd(spi_iface *spi, uint8_t cmd, uint32_t arg, uint8_t *res, uint32_t res_len) {
    uint8_t cmd_buf[CMD_TX_SIZE] = {
        cmd | CMD_OFFSET,
        (arg >> 0x18) & 0xff,
        (arg >> 0x10) & 0xff,
        (arg >> 0x08) & 0xff,
        (arg >> 0x00) & 0xff,
        0,
    };
    cmd_buf[CMD_TX_SIZE - 1] = crc7(cmd_buf, CMD_TX_SIZE - 1);

    spi->select(spi, 0);

    spi->transmit(spi, cmd_buf, CMD_TX_SIZE);

    uint32_t try_cnt = CMD_RECV_TRY_CNT;
    uint8_t ret = SD_R1_INVALID;
    while (--try_cnt && FLAG_GET(ret, SD_R1_INVALID)) {
        ret = spi->rx_byte(spi);
    }

    if (!try_cnt) {
        //! CRC
        spi->rx_byte(spi);
        spi->select(spi, 1);
        //! timeout
        return SD_R1_INVALID;
    }

    while (res && res_len--) {
        *res++ = spi->rx_byte(spi);
    }

    //! CRC
    spi->rx_byte(spi);

    spi->select(spi, 1);

    return ret;
}

static int read_reg(spi_iface *spi, uint8_t *buf, uint32_t buf_size) {
    spi->select(spi, 0);

    int now = clock_get();
    uint8_t ret = 0xff;
    while (ret == 0xff && (clock_get() - now < READ_TIMEOUT_MSEC)) {
        ret = spi->rx_byte(spi);
    }

    if (clock_get() - now >= READ_TIMEOUT_MSEC || ret != START_BLOCK_TOK) {
        spi->rx_byte(spi);
        spi->select(spi, 1);
        return -1;
    }

    spi->receive(spi, buf, buf_size);

    //! CRC
    spi->rx_byte(spi);
    spi->rx_byte(spi);

    spi->rx_byte(spi);

    spi->select(spi, 1);

    return 0;
}

static int sd_init_sequence(sd_spi_ctx *ctx) {
    ctx->spi->select(ctx->spi, 1);

    uint32_t tmout = CMD_NUM_TRY;
    while (tmout--) {
        ctx->spi->rx_byte(ctx->spi);
    }

    tmout = CMD_NUM_TRY;
    int ret = 0;
    while (tmout-- && !FLAG_GET(ret, SD_R1_IDLE)) {
        ret = sd_cmd(ctx->spi, 0x00, 0x00, NULL, 0);
    }

    if (!tmout) {
        return -1;
    }

    uint8_t recv_buf[CMD_RX_SIZE];
    ret = sd_cmd(ctx->spi, 0x08, CMD8_PARAM, recv_buf, CMD_RX_SIZE);
    if (ret == (SD_R1_ILLEGAL_CMD | SD_R1_IDLE)) {
        tmout = CMD_NUM_TRY;
        ret = SD_R1_ILLEGAL_CMD;
        while (tmout-- && ret) {
            ret = sd_cmd(ctx->spi, 0x37, 0x00, NULL, 0);
            if (FLAG_GET(ret, SD_R1_ILLEGAL_CMD)) {
                break;
            }
            ret = sd_cmd(ctx->spi, 0x29, 0x00, recv_buf, CMD_RX_SIZE);
        }

        if (!tmout || ret & SD_R1_ILLEGAL_CMD) {
            tmout = CMD_NUM_TRY;
            ret = SD_R1_ILLEGAL_CMD;
            while (tmout-- && ret) {
                ret = sd_cmd(ctx->spi, 0x01, 0x00, NULL, 0);
            }

            if (!tmout) {
                return -2;
            } else {
                ctx->stat.type = SD_MMC;
            }
        } else {
            ctx->stat.type = SD_V1;
        }
    } else {
        int now = clock_get();
        do {
            ret = sd_cmd(ctx->spi, 0x37, 0x00, NULL, 0);
            ret = sd_cmd(ctx->spi, 0x29, OCR_HC_FLAG, recv_buf, CMD_RX_SIZE);
        } while (ret && (clock_get() - now < ACMD41_TMOUT_MSEC));
        if (clock_get() - now >= ACMD41_TMOUT_MSEC)  {
            return -3;
        }

        ctx->stat.type = SD_V2;

        ret = sd_cmd(ctx->spi, 0x3A, 0x0, recv_buf, CMD_RX_SIZE);
        if (ret == 0x00) {
            uint32_t ocr_reg = (recv_buf[0] << 0x18) | (recv_buf[1] << 0x10) |
                (recv_buf[2] << 0x08) | (recv_buf[3] << 0x0);
                if (FLAG_GET(ocr_reg, OCR_HC_FLAG)) {
                    ctx->stat.type = SD_SDHC;
                }
        } else {
            return -4;
        }
    }

    switch (ctx->stat.type) {
        case SD_NONE:
            return -5;
        case SD_V1:
        case SD_V2:
        case SD_MMC:
            //! set block size
            if (sd_cmd(ctx->spi, 0x10, SD_BLOCK_SIZE, NULL, 0)) {
                return -6;
            }
        default:
            break;
    }

    ret = sd_cmd(ctx->spi, 0x09, 0x00, NULL, 0);
    if (ret) {
        return -7;
    }

    uint8_t csd[REG_SIZE];
    if (read_reg(ctx->spi, csd, REG_SIZE)) {
        return -8;
    }

    if (ctx->stat.type == SD_MMC) {
        uint32_t size       = ((((uint32_t)(csd[6] & 0x03) << 0x08) + csd[7]) << 0x02) +
            (csd[8] >> 0x06) + 0x01;
        uint32_t block_size = 0x01 << (csd[5] & 0x0f);
        uint32_t size_mul   = 0x01 << (((csd[9] & 0x03) << 0x01) + ((csd[10] & 0x80) >> 0x07) + 2);
        ctx->stat.size_mb   = (size * size_mul * block_size) / 1024 / 1024;
    } else {
        uint8_t ver = csd[0] >> 0x06;
        if (ver == 0) {
            uint32_t size = (((uint32_t)(csd[6] & 0x03) << 0x0a) | ((uint32_t)(csd[7] << 0x02)) |
                ((csd[8] & 0x0c) >> 0x06)) + 0x01;
            uint32_t size_mul = (0x01 << ((((csd[9] & 0x03) << 0x01) | ((csd[10] & 0x80) >> 0x07)) +
                0x02));
            uint32_t block_size = (0x01 << (csd[5] & 0x0f));

            ctx->stat.size_mb = (size * size_mul * block_size) / 1024 / 1024;
        } else {
            uint32_t size = (((uint32_t)(csd[7] & 0x3f) << 0x10) | ((uint32_t)csd[8] << 0x08) |
                csd[9]) + 0x01;
            ctx->stat.size_mb = size * SD_BLOCK_SIZE / 1024;
        }
    }

    return 0;
}

static int sd_spi_read(disk_ifc *ifc, uint32_t blk, uint8_t *buf) {
    sd_spi_ctx *ctx = (sd_spi_ctx *) ifc;
    if (!ctx) {
        return -1;
    }

    if (ctx->stat.type != SD_SDHC) {
        //! byte addr
        blk <<= 9;
    }

    uint8_t ret = sd_cmd(ctx->spi, 0x11, blk, NULL, 0);
    if (ret) {
        return -2;
    }

    ctx->spi->select(ctx->spi, 0);

    int now = clock_get();
    ret = 0xff;
    while (ret == 0xff && (clock_get() - now < READ_TIMEOUT_MSEC)) {
        ret = ctx->spi->rx_byte(ctx->spi);
    }

    if ((clock_get() - now >= READ_TIMEOUT_MSEC) || ret != START_BLOCK_TOK) {
        ctx->spi->rx_byte(ctx->spi);
        ctx->spi->select(ctx->spi, 1);
        return -3;
    }

    ctx->spi->receive(ctx->spi, buf, SD_BLOCK_SIZE);

    //! CRC
    uint16_t crc = ctx->spi->rx_byte(ctx->spi);
    crc <<= 8;
    crc |= ctx->spi->rx_byte(ctx->spi);

    ctx->spi->rx_byte(ctx->spi);

    ctx->spi->select(ctx->spi, 1);

    return !(crc == crc16(buf, SD_BLOCK_SIZE));
}

static int sd_spi_write(disk_ifc *ifc, uint32_t blk, const uint8_t *buf) {
    sd_spi_ctx *ctx = (sd_spi_ctx *) ifc;
    if (!ctx) {
        return -1;
    }

    if (ctx->stat.type != SD_SDHC) {
        //! byte addr
        blk <<= 9;
    }

    uint16_t crc = crc16(buf, SD_BLOCK_SIZE);

    uint8_t ret = sd_cmd(ctx->spi, 0x18, blk, NULL, 0);
    if (ret) {
        return -2;
    }

    ctx->spi->select(ctx->spi, 0);

    ctx->spi->tx_byte(ctx->spi, START_BLOCK_TOK);
    ctx->spi->transmit(ctx->spi, buf, SD_BLOCK_SIZE);

    ctx->spi->tx_byte(ctx->spi, crc >> 0x08);
    ctx->spi->tx_byte(ctx->spi, crc && 0xff);

    ret = ctx->spi->rx_byte(ctx->spi);
    ret &= 0x1f;
    if (ret != DATA_ACCEPTED_TOK) {
        ctx->spi->rx_byte(ctx->spi);
        ctx->spi->select(ctx->spi, 1);

        return -3;
    }

    int now = clock_get();
    ret = 0;
    while (ret == 0 && (clock_get() - now < WRITE_TIMEOUT_MSEC)) {
        ret = ctx->spi->rx_byte(ctx->spi);
    }

    ctx->spi->rx_byte(ctx->spi);

    ctx->spi->select(ctx->spi, 1);

    return 0;
}

static int sd_spi_get_stat(disk_ifc *ifc, disk_stat *stat) {
    sd_spi_ctx *ctx = (sd_spi_ctx *) ifc;
    if (!ctx || !stat) {
        return -1;
    }

    stat->size_mb = ctx->stat.size_mb;
    stat->block_size = SD_BLOCK_SIZE;

    return 0;
}

static void sd_spi_destroy(disk_ifc *ifc) {
    sd_spi_ctx *ctx = (sd_spi_ctx *) ifc;
    if (!ctx) {
        return;
    }

    SPI_OFF(ctx->spi);

    free(ctx);
}

disk_ifc *sd_spi_init(const spi_config *cfg) {
    if (!cfg) {
        return NULL;
    }

    spi_init(SD_SPI, cfg);

    spi_iface *spi = spi_iface_get(SD_SPI);
    if (!spi) {
        return NULL;
    }

    sd_spi_ctx *ctx = malloc(sizeof(sd_spi_ctx));
    if (!ctx) {
        return NULL;
    }

    memset(&ctx->stat, 0, sizeof(sd_spi_stat));

    ctx->spi          = spi;

    ctx->ifc.read     = sd_spi_read;
    ctx->ifc.write    = sd_spi_write;
    ctx->ifc.get_stat = sd_spi_get_stat;
    ctx->ifc.destroy  = sd_spi_destroy;

    spi->enable(spi, 1);

    if (sd_init_sequence(ctx)) {
        sd_spi_destroy(&ctx->ifc);
        spi->enable(spi, 0);
        return NULL;
    }

    return &ctx->ifc;
}
