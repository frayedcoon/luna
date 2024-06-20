#ifndef PLATFORM_USART_H
#define PLATFORM_USART_H

#include "common/common.h"
#include "target/cfg.h"
#include "platform/peripheral.h"
#include "platform/gpio.h"

/**
 * usart base registers enumeration
 */
typedef enum usart_t {
#ifdef HAS_USART1
    USART1  = USART1_BASE
    ,
#endif
#ifdef HAS_USART2
    USART2  = USART2_BASE
    ,
#endif
#ifdef HAS_USART3
    USART3  = USART3_BASE
    ,
#endif
#ifdef HAS_USART4
    USART4  = USART4_BASE
    ,
#endif
#ifdef HAS_USART5
    USART5  = USART5_BASE
#endif
} usart;

/**
 * usart interface
 */
typedef struct usart_iface_t {
    /**
     * @brief      put a single charater to usart transmitter
     *
     * @param      iface  usart interface
     * @param[in]  data   character to send
     */
    void  (*putc)(struct usart_iface_t *iface, char data);
    /**
     * @brief      put a sring to usart transmitter
     *
     * @param      iface  usart interface
     * @param[in]  data   string ptr
     */
    void  (*puts)(struct usart_iface_t *iface, const char *data);
    /**
     * @brief      put a sring to usart transmitter with given length
     *
     * @param      iface  usart interface
     * @param[in]  data   string ptr
     * @param[in]  len    string length
     */
    void  (*nputs)(struct usart_iface_t *iface, const char *data,
        uint32_t len);
    /**
     * @brief      get usart receiver buffer
     *
     * @param      iface  usart interface
     *
     * @return     usart receiver buffer
     */
    char* (*buffer_drain)(struct usart_iface_t *iface);
    /**
     * @brief      fill usart receiver buffer with incomming data
     *
     * @param      iface  usart interface
     *
     * @return     boolean value (ready to be processed)
     */
    int  (*buffer_consume)(struct usart_iface_t *iface);
    /**
     * @brief      check if transfer to usart buffer locked
     *
     * @param      iface  usart inteface
     *
     * @return     locked/unlocked flag
     */
    int  (*buffer_locked)(struct usart_iface_t *iface);
    /**
     * @brief      lock/unclok usart buffer
     *
     * @param      iface   usart interface
     * @param[in]  locked  lock/unlock flag
     */
    void  (*set_buffer_locked)(struct usart_iface_t *iface, int locked);
    void  (*destroy)(struct usart_iface_t *iface);
} usart_iface;

/**
 * usart configuration struct
 */
typedef struct usart_config_t {
    uint32_t  baud_rate;
    uint32_t  buffer_size;

    gpio_port port;
    uint32_t tx_pin;
    uint32_t rx_pin;
    const void *signal;
} usart_config;

/**
 * @brief      get usart interface by it's base
 *
 * @param[in]  usart_base  usart base address
 *
 * @return     usart interface
 */
usart_iface *usart_iface_get(usart usart_base);

/**
 * @brief      initialize usart by give configuration
 *
 * @param[in]  usart_base  usart base address
 * @param[in]  config      usart configuration
 */
void usart_init(usart usart_base, const usart_config *config);

#endif
