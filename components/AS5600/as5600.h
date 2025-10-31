#ifndef AS5600_H
#define AS5600_H

#include "esp_log.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

//在这定义你的iic接口参数
#define I2C_MASTER_SCL_IO           4
#define I2C_MASTER_SDA_IO           5
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          100000


// AS5600寄存器地址定义:cite[6]:cite[10]
#define AS5600_RAW_ANGLE_ADDR_H    0x0C    // 原始角度高字节寄存器:cite[10]
#define AS5600_RAW_ANGLE_ADDR_L    0x0D    // 原始角度低字节寄存器
#define AS5600_WHO_AM_I            0x0F    // 设备ID寄存器

// AS5600默认I2C地址:cite[3]:cite[6]
#define AS5600_I2C_ADDR_DEFAULT    0x36

// AS5600分辨率
#define AS5600_RESOLUTION          (1<<12)    // 12位分辨率:cite[3]
/**
 * @brief 初始化AS5600传感器
 * 
 * @param bus_cfg 总线配置参数
 * @param dev_cfg    设备句柄指针
 * @return esp_err_t 
 */
void as5600_init(i2c_master_bus_config_t bus_cfg,i2c_device_config_t dev_cfg);

/**
 * @brief 读取原始角度值 (0-4095)
 * 
 * @param as5600_handle as5600设备句柄
 * @param raw_angle 原始角度值指针
 * @return esp_err_t 
 */
void as5600_read_raw_angle(i2c_master_dev_handle_t as5600_handle, uint16_t *raw_angle);

/**
 * @brief 读取角度值 (0-360度)
 * 
 * @param as5600_handle as5600设备句柄
 * @param angle 角度值指针 (度)
 * @return esp_err_t 
 */
void as5600_read_angle(i2c_master_dev_handle_t as5600_handle, float *angle);

/**
 * @brief 销毁AS5600传感器并释放资源
 * 
 * @param as5600_handle as5600设备句柄
 * @return esp_err_t 
 */
void as5600_deinit(i2c_master_dev_handle_t as5600_handle);



extern i2c_master_bus_handle_t bus_handle;
extern i2c_master_dev_handle_t as5600_handle;










#ifdef __cplusplus
}
#endif
#endif // AS5600_H