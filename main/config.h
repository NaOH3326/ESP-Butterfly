#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "sensor_n.h"
#include "motor.h"
#include "crsf.h"
#include "mpu6050.h"




int uart_buff = 1024;

#define ADDR MPU6050_I2C_ADDRESS_HIGH
mpu6050_dev_t dev = { 0 };

//iic总线配置
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
    .scl_speed_hz = 400000,
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


// 电机配置
float duty = 0;
motor_handle_t *motor1,*motor2,*motor3,*motor4 = NULL;
    
    motor_config_t motor_config1 = {
        .mcpwm_unit = MCPWM_UNIT_0,           // 使用MCPWM单元0:cite[1]
        .timer_num = MCPWM_TIMER_0,           // 使用定时器0:cite[1]
        .io_signal = MCPWM0A,                 // 使用PWM0A输出
        .pwm_num = 11,  
        .c0_num  = 9,
        .c1_num  = 10,           // 根据实际接线修改
        .pwm_freq_hz = 60000,                 // 25kHz PWM频率
    };
        motor_config_t motor_config2 = {
        .mcpwm_unit = MCPWM_UNIT_0,           // 使用MCPWM单元0:cite[1]
        .timer_num = MCPWM_TIMER_0,           // 使用定时器0:cite[1]
        .io_signal = MCPWM0A,                 // 使用PWM0A输出
        .pwm_num = 8,  
        .c0_num  = 3,
        .c1_num  = 46,           // 根据实际接线修改
        .pwm_freq_hz = 60000,                 // 25kHz PWM频率
    };
        motor_config_t motor_config3 = {
        .mcpwm_unit = MCPWM_UNIT_1,           // 使用MCPWM单元0:cite[1]
        .timer_num = MCPWM_TIMER_0,           // 使用定时器0:cite[1]
        .io_signal = MCPWM0A,                 // 使用PWM0A输出
        .pwm_num = 18,  
        .c0_num  = 16,
        .c1_num  = 17,           // 根据实际接线修改
        .pwm_freq_hz = 60000,                 // 25kHz PWM频率
    };
        motor_config_t motor_config4 = {
        .mcpwm_unit = MCPWM_UNIT_1,           // 使用MCPWM单元0:cite[1]
        .timer_num = MCPWM_TIMER_0,           // 使用定时器0:cite[1]
        .io_signal = MCPWM0A,                 // 使用PWM0A输出
        .pwm_num = 6,  
        .c0_num  = 7,
        .c1_num  = 15,           // 根据实际接线修改
        .pwm_freq_hz = 60000,                 // 25kHz PWM频率
    };