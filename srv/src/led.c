#include "platform/gpio.h"
#include "platform/clock.h"
#include "target/cfg.h"
#include "common/def.h"
#include "lib/string.h"
#include "kernel/syscall.h"
#include "kernel/thread.h"
#include "common/log.h"
#include "common/sys.h"

#define LED_SOCK_BUFFER_SIZE       32

#define LED_CONNECTION_BUFFER_SIZE 32

#define LED_CONNECTIONS_MAX        (sizeof(uint32_t) * 8)

#define LED_PORT_PATH        "/srv/led"

/**
 * GPIO interface with DHT pin
 */
static gpio_iface *led_gpio_iface = NULL;

static int         led_active     = 1;

static uint32_t    led_interval   = 250;

/**
 * @brief      initializes GPIO pin connected to LED
 */
static void led_init(void) {
    //! create GPIO port interface
    led_gpio_iface = gpio_iface_get(LED_GPIO_PORT);

    //! init GPIO pin connected to led
    led_gpio_iface->init(led_gpio_iface, LED_GPIO_PIN, GPIO_OTYPE_PUSH_PULL,
        GPIO_MODE_OUTPUT, GPIO_SPEED_FAST, GPIO_PUPD_NO, 0);
}

void led_loop(void) {
    led_init();

    int ret = socket(LED_PORT);
    ASSERT(!ret);

    while (1) {
        const void *client = listen(LED_PORT);
        if (client) {
            const void * conn = accept(LED_PORT, client, 0);
            if (!conn) {
                LOG_DBG("Failed to accept sock connection");
            }
        }

        char io_buffer[LED_SOCK_BUFFER_SIZE];

        const void *conn = select(LED_PORT);
        if (conn) {
            int32_t reply = 0;

            int ret = timed_read(conn, io_buffer, 8, 100);
            if (ret > 0) {
                if (strncmp(io_buffer, "1", 2) == 0) {
                    LOG_DBG("Enable control received");

                    led_active = 1;
                } else if (strncmp(io_buffer, "0", 2) == 0) {
                    LOG_DBG("Disable control received");

                    led_active = 0;
                } else {
                    LOG_ERR("Illegal control received");

                    reply = -1;
                }

                ret = write(conn, (char*) &reply, sizeof(int32_t));
                if (ret < 0) {
                    LOG_ERR("Failed to respond control signal");
                }
            }
        }

        if (led_active) {
            led_gpio_iface->toggle(led_gpio_iface, LED_GPIO_PIN);
        }

        sleep(led_interval);
    }
}

KERNEL_SRV(led, led_loop, 128, 2);
