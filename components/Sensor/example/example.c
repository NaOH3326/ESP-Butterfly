#include "sensor.h"



//iic总线配置结构体初始化
 
static i2c_master_bus_config_t bus_cfg = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = I2C_NUM_0,
    .sda_io_num = GPIO_NUM_4,
    .scl_io_num = GPIO_NUM_5,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
};
static i2c_device_config_t pca9548_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = 0x70,
    .scl_speed_hz = 100000,
};
static i2c_device_config_t as5600_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = AS5600_I2C_ADDR_DEFAULT,
    .scl_speed_hz = I2C_MASTER_FREQ_HZ,
};

i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t as5600_handle;
i2c_master_dev_handle_t pca9548_handle;

uint16_t raw_angle;
float angle;

void app_main(void)
{ 
    as5600_init(bus_cfg, as5600_cfg);
    pca9548_init(bus_cfg,pca9548_cfg);
    vTaskDelay(100);
    
while (1)
{
    for (size_t i = 0; i < 3; i++)
    {
        pca9548_setchannel(i);
        as5600_read_raw_angle(as5600_handle, &raw_angle);
        as5600_read_angle(as5600_handle, &angle);
        printf("channel%d",i);
        printf("原始角度: %d, 实际角度: %.2f°\n", raw_angle, angle);
        vTaskDelay(1);
    }
}
}

