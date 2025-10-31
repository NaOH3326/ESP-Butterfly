
#ifndef PCA9548_H
#define PCA9548_H


#include "esp_log.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif
extern i2c_master_bus_handle_t bus_handle;
extern i2c_master_dev_handle_t pca9548_handle;



void pca9548_init(i2c_master_bus_config_t bus_cfg,i2c_device_config_t pca9548_cfg);
void pca9548_setchannel(int channel);

#ifdef __cplusplus
}
#endif
#endif