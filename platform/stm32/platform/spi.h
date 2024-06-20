#ifndef PLATFORM_SPI_H
#define PLATFORM_SPI_H

#include "common/common.h"
#include "target/cfg.h"
#include "platform/peripheral.h"
#include "platform/gpio.h"

/**
 * SPI base registers enumeration
 */
typedef enum spi_t {
#ifdef HAS_SPI1
    SPI1  = SPI1_BASE
    ,
#endif
#ifdef HAS_SPI2
    SPI2  = SPI2_BASE
    ,
#endif
#ifdef HAS_SPI3
    SPI3  = SPI3_BASE
    ,
#endif
#ifdef HAS_SPI4
    SPI4  = SPI4_BASE
    ,
#endif
#ifdef HAS_SPI5
    SPI5  = SPI5_BASE
#endif
} spi;

typedef enum spi_prescaler_t {
    SPI_PRESCALER_2     = 0x00,
    SPI_PRESCALER_4     = 0x01,
    SPI_PRESCALER_8     = 0x02,
    SPI_PRESCALER_16    = 0x03,
    SPI_PRESCALER_32    = 0x04,
    SPI_PRESCALER_64    = 0x05,
    SPI_PRESCALER_128   = 0x06,
    SPI_PRESCALER_256   = 0x07,
} spi_prescaler;

/**
 * SPI interface
 */
typedef struct spi_iface_t {
    /**
     * @brief      enable/disable SPI interface
     *
     * @param      iface    SPI interface
     * @param[in]  enable   boolean value to be set to CS pin
     */
    void (*enable)(struct spi_iface_t *iface, int enable);
    /**
     * @brief      set CS pin to value
     *
     * @param      iface    SPI interface
     * @param[in]  cs_value boolean value to be set to CS pin
     */
    void (*select)(struct spi_iface_t *iface, int cs_value);
    /**
     * @brief      change SPI baud rate
     *
     * @param      iface     SPI interface
     * @param[in]  prescaler prescaler value
     */
    void (*set_prescaler)(struct spi_iface_t *iface, spi_prescaler prescaler);
    /**
     * @brief      transmit single byte
     *
     * @param      iface    SPI interface
     * @param[in]  byte     byte to be sent
     */
    void (*tx_byte)(struct spi_iface_t *iface, uint8_t byte);
    /**
     * @brief      transmit single byte
     *
     * @param      iface    SPI interface
     *
     * @return     received byte
     */
    uint8_t (*rx_byte)(struct spi_iface_t *iface);
    /**
     * @brief      transmit and receive single byte
     *
     * @param      iface    SPI interface
     * @param[in]  byte     byte to be sent
     *
     * @return     received byte
     */
    uint8_t (*txrx_byte)(struct spi_iface_t *iface, uint8_t byte);
    /**
     * @brief      transmit buffer
     *
     * @param      iface  SPI interface
     * @param[in]  data   buffer to be sent
     * @param[in]  len    buffer length
     *
     * @return     0 in case of success, otherwise negative
     */
    int  (*transmit)(struct spi_iface_t *iface, const uint8_t *data, uint32_t len);
    /**
     * @brief      synchronously transmit & receive data
     *
     * @param      iface  SPI interface
     * @param[in]  tx_buf buffer to be sent
     * @param[in]  rx_buf buffer to store received data
     * @param[in]  len    number of bytes to be sent/received
     *
     * @return     0 in case of success, otherwise negative
     */
    int  (*transmit_receive)(struct spi_iface_t *iface, const uint8_t *tx_buf, uint8_t *rx_buf, uint32_t len);
    /**
     * @brief      receive data into buffer
     *
     * @param      iface  SPI interface
     * @param[in]  data   buffer to store received data
     * @param[in]  len    number of bytes to be read
     *
     * @return     0 in case of success, otherwise negative
     */
    int  (*receive)(struct spi_iface_t *iface, uint8_t *data, uint32_t len);
    /**
     * @brief      deallocate/deinitialize interface
     *
     * @param      iface  SPI interface
     */
    void  (*destroy)(struct spi_iface_t *iface);
} spi_iface;

typedef enum spi_mode_t {
    SPI_MODE_SLAVE  = 0x00,
    SPI_MODE_MASTER = 0x01,
} spi_mode;

/**
 * SPI configuration struct
 */
typedef struct spi_config_t {
    spi_prescaler prescaler;
    spi_mode      mode;
    uint8_t       cpol;
    uint8_t       cpha;

    gpio_port     port;
    uint32_t      cs_pin;
    uint32_t      sck_pin;
    uint32_t      miso_pin;
    uint32_t      mosi_pin;
} spi_config;

/**
 * @brief      get SPI interface by it's base
 *
 * @param[in]  spi_base  SPI base address
 *
 * @return     SPI interface
 */
spi_iface *spi_iface_get(spi spi_base);

/**
 * @brief      initialize SPI by give configuration
 *
 * @param[in]  spi_base  SPI base address
 * @param[in]  config      SPI configuration
 */
void spi_init(spi spi_base, const spi_config *config);

#endif
