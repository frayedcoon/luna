#ifndef DRIVER_SD_SPI_H
#define DRIVER_SD_SPI_H

#include "common/common.h"
#include "platform/spi.h"
#include "driver/disk.h"

disk_ifc *sd_spi_init(const spi_config *cfg);

#endif