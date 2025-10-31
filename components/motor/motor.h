#ifndef BLDC_MOTOR_H
#define BLDC_MOTOR_H


#include "driver/gpio.h"
#include "driver/mcpwm.h"
#include "esp_err.h"
#include "esp_log.h"
#include <math.h>
#ifdef __cplusplus
extern "C" {
#endif





// 电机配置结构
typedef struct {
    mcpwm_unit_t mcpwm_unit;      // MCPWM单元
    mcpwm_timer_t timer_num;      // 定时器
    mcpwm_io_signals_t io_signal; // IO信号
    gpio_num_t pwm_num;           //方向控制1
    gpio_num_t c0_num;            //方向控制2
    gpio_num_t c1_num;            // GPIO引脚
    uint32_t pwm_freq_hz;         // PWM频率
} motor_config_t;

// 电机句柄
typedef struct {
    motor_config_t config;
    float current_duty;
    bool is_initialized;
} motor_handle_t;

/**
 * @brief 初始化电机
 * @param config 电机配置
 * @param handle 返回的电机句柄
 * @return esp_err_t 
 */
esp_err_t bldc_motor_init(const motor_config_t *config, motor_handle_t **handle);

/**
 * @brief 设置电机速度
 * @param handle 电机句柄
 * @param duty_cycle 占空比0-100%
 * @return esp_err_t 
 */
esp_err_t bldc_motor_set_speed(motor_handle_t *handle, float duty_cycle);

/**
 * @brief 紧急停止电机
 * @param handle 电机句柄
 * @return esp_err_t 
 */
esp_err_t bldc_motor_stop(motor_handle_t *handle);

/**
 * @brief 释放电机资源
 * @param handle 电机句柄
 * @return esp_err_t 
 */
esp_err_t bldc_motor_deinit(motor_handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif