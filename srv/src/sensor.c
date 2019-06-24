#include "srv/sensor.h"
#include "driver/dht.h"
#include "target/cfg.h"
#include "kernel/thread.h"
#include "kernel/syscall.h"
#include "platform/clock.h"
#include "platform/timer.h"
#include "common/log.h"

#define SENSOR_MAX_FAILURES 10

#define DEFAULT_SENSOR_PERIOD 100

#define SENSOR_NAME_DHT "dht"

/**
 * DHT sensor configuration
 */
static const
dht_config dht_sensor_config = {
    .port  = DHT_GPIO_PORT,
    .pin   = DHT_GPIO_PIN,
    .timer = DHT_TIMER
};

/**
 * timer TIM3 configuration (used by DHT sensor)
 */
static const
timer_config tim3_config = {
    .counter_mode = TIMER_COUNTER_MODE_UP,
    .prescaler    = 0x3f,
    .period       = 0xff
};

static uint32_t sensor_period = DEFAULT_SENSOR_PERIOD;

static int dht_enabled = 1;

static void sensor_loop(void) {
    timer_init(TIM3, &tim3_config);

    dht_init(&dht_sensor_config);

    dht_iface *iface_dht = dht_iface_get();

    uint32_t dht_failure_counter = 0;

    while (1) {

        if (iface_dht && dht_enabled) {
            if (!iface_dht->read(iface_dht)) {
                dht_failure_counter++;
            } else {
                dht_failure_counter = 0;
            }
        }

        if (dht_failure_counter > SENSOR_MAX_FAILURES) {
            LOG_ERR("[DHT] too many consequent failures, disabling sensor");
            dht_failure_counter = 0;
            dht_enabled = 0;
        }

        sleep(sensor_period);
    }
}

void sensors_set_period(uint32_t seconds) {
    sensor_period = seconds;
}

void sensors_set_enabled(char *sensor_name, int enabled) {
    if (strcmp(sensor_name, SENSOR_NAME_DHT) == 0) {
        dht_enabled = enabled;
    }
}

uint32_t sensors_get_period(void) {
    return sensor_period;
}

void sensors_print_stat(char *sensor_name) {
    if (strcmp(sensor_name, SENSOR_NAME_DHT) == 0) {
        printf("[DHT]\n");
        printf("Enabled: ");
        if (dht_enabled) {
            printf("yes\n");
        } else {
            printf("no\n");
        }
    } else {
        LOG_ERR("Unknown sensor");
    }
}

KERNEL_SRV(sensor, sensor_loop, 128, 2);
