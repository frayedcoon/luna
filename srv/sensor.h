#ifndef SRV_SENSORS_H
#define SRV_SENSORS_H

#include "common/common.h"

/**
 * @brief      set periad of sensor reading
 *
 * @param[in]  seconds  The seconds
 */
void sensors_set_period(uint32_t seconds);

/**
 * @brief      enable/disable sensor by it's name
 *
 * @param      sensor_name  sensor name str
 * @param[in]  enabled      enable/disable flag
 */
void sensors_set_enabled(char *sensor_name, int enabled);

/**
 * @brief      get current sensor read period
 *
 * @return     sensor read period
 */
uint32_t sensors_get_period(void);

/**
 * @brief      print sensor stats
 *
 * @param      sensor_name  sensor name
 */
void sensors_print_stat(char *sensor_name);

#endif
