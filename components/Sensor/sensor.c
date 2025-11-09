#include "sensor.h"
#include "stdio.h"
#include "esp_log.h"


void as5600_init(i2c_master_bus_config_t bus_cfg,i2c_device_config_t as5600_cfg){
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus_handle));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &as5600_cfg, &as5600_handle));   
}

void as5600_read_raw_angle(i2c_master_dev_handle_t as5600_handle, uint16_t *raw_angle){
    uint8_t reg_addr = AS5600_RAW_ANGLE_ADDR_H;
    uint8_t data[2] = {0};
    // 先写入要读取的寄存器地址，然后读取2个字节（高字节+低字节）
    ESP_ERROR_CHECK(i2c_master_transmit_receive(as5600_handle, &reg_addr, 1, data, 2, -1));
    // 组合高字节和低字节形成12位原始角度值
    *raw_angle = ((data[0] & 0x0F) << 8) | data[1];
}

void as5600_read_angle(i2c_master_dev_handle_t as5600_handle, float *angle){
    uint16_t raw_angle;
    as5600_read_raw_angle(as5600_handle, &raw_angle);
    *angle = (raw_angle * 360.0f) / AS5600_RESOLUTION;
}

esp_err_t as5600_read_status(i2c_master_dev_handle_t as5600_handle, uint8_t *status) {
    uint8_t reg_addr = 0x0B;
    i2c_master_transmit_receive(as5600_handle, &reg_addr, 1, status, 1, 1000);
    if ((*status & (1<<5)) || (*status & (1<<6))) {
    // 磁铁太强或太弱，认为数据不可靠
    return ESP_ERR_INVALID_STATE;
    }
return ESP_OK;
}

void as5600_deinit(i2c_master_dev_handle_t as5600_handle){
    ESP_ERROR_CHECK( i2c_master_bus_rm_device(as5600_handle));
}

void pca9548_init(i2c_master_bus_config_t bus_cfg,i2c_device_config_t pca9548_cfg){
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &pca9548_cfg, &pca9548_handle));
};
void pca9548_setchannel(int channel){
    //uint8_t ch[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
    uint8_t ch[] = {0x10,0x20,0x40,0x80};
    ESP_ERROR_CHECK(i2c_master_transmit(pca9548_handle, &ch[channel], 1, -1));
};