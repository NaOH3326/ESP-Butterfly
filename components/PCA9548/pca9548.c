#include "pca9548.h"
#include <stdio.h>   
    








void pca9548_init(i2c_master_bus_config_t bus_cfg,i2c_device_config_t pca9548_cfg){
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &pca9548_cfg, &pca9548_handle));
};
void pca9548_setchannel(int channel){
    //uint8_t ch[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
    uint8_t ch[] = {0x10,0x20,0x40,0x80};
    ESP_ERROR_CHECK(i2c_master_transmit(pca9548_handle, &ch[channel], 1, -1));
};