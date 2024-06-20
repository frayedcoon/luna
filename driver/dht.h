#ifndef DRIVER_DHT_H
#define DRIVER_DHT_H

#include "common/common.h"
#include "platform/gpio.h"
#include "platform/timer.h"

typedef struct dht_iface_t {
    /**
     * @brief      read data cyles from dht sensor
     *
     * @param      iface  dht driver interface
     *
     * @return     success of read
     */
    int (*read)(struct dht_iface_t *iface);
    /**
     * @brief      get last valid temperature value
     *
     * @param      iface  dht driver interface
     *
     * @return     temperature value
     */
    uint8_t (*get_temperature)(struct dht_iface_t *iface);
    /**
     * @brief      get last valid humidity value
     *
     * @param      iface  dht driver interface
     *
     * @return     humidity value
     */
    uint8_t (*get_humidity)(struct dht_iface_t *iface);
    /**
     * @brief      deallocate dht driver context
     *
     * @param      iface  dht driver interface
     */
    void (*destroy)(struct dht_iface_t *iface);
} dht_iface;

/**
 * dht driver configuration
 */
typedef struct dht_config_t {
    gpio_port port;
    uint32_t  pin;
    timer     timer;
} dht_config;

/**
 * @brief      get dht driver interface
 *
 * @return     dht driver interface if initialized, otherwise NULL
 */
dht_iface *dht_iface_get(void);

/**
 * @brief      initialize dht driver
 *
 * @param[in]  config  dht driver configuration
 */
void dht_init(const dht_config *config);

#endif
