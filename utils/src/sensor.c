#include "app/terminal.h"
#include "driver/dht.h"
#include "lib/string.h"
#include "srv/sensor.h"

#define SENSOR_CMD_SHOW    "show"
#define SENSOR_CMD_PERIOD  "period"
#define SENSOR_CMD_DISABLE "disable"
#define SENSOR_CMD_ENABLE  "enable"
#define SENSOR_CMD_STAT    "stat"

#define SENSOR_NAME_DHT "dht"

static int sensor_cmd_handler(list_ifc *args) {
    if (args->size(args) > 3) {
        return -1;
    }

    const list_node *node = args->get_front(args)->nxt;

    if (strcmp((char*) node->ptr, SENSOR_CMD_SHOW) == 0) {
        node = node->nxt;
        if (!node) {
            return -1;
        }

        if (strcmp((char*) node->ptr, SENSOR_NAME_DHT) == 0) {
            dht_iface *iface_dht = dht_iface_get();
            if (iface_dht) {
                printf("temperature:   %d C\n"
                    "humidity:      %d %%\n",
                        iface_dht->get_temperature(iface_dht),
                            iface_dht->get_humidity(iface_dht));
            } else {
                printf("DHT driver error\n");
            }
            return 0;
        }
    } else if (strcmp((char*) node->ptr, SENSOR_CMD_PERIOD) == 0) {
        node = node->nxt;

        if (node) {
            sensors_set_period(atoi((char*) node->ptr));
        } else {
            printf("sensors read period:   %d\n", sensors_get_period());
        }
        return 0;
    } else if (strcmp((char*) node->ptr, SENSOR_CMD_DISABLE) == 0) {
        node = node->nxt;
        if (!node) {
            return -1;
        }

        sensors_set_enabled(SENSOR_NAME_DHT, 0);
        return 0;
    } else if (strcmp((char*) node->ptr, SENSOR_CMD_ENABLE) == 0) {
        node = node->nxt;
        if (!node) {
            return -1;
        }

        sensors_set_enabled(SENSOR_NAME_DHT, 1);
        return 0;
    } else if (strcmp((char*) node->ptr, SENSOR_CMD_STAT) == 0) {
        node = node->nxt;
        if (!node) {
            return -1;
        }

        sensors_print_stat((char*) node->ptr);
        return 0;
    }

    return -1;
}

TERMINAL_CMD(sensor, sensor_cmd_handler);
