#include "motor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void motor_control_task(void *pvParameter) {
    motor_handle_t *motor = NULL;
    
    // 电机配置
    motor_config_t motor_config = {
        .mcpwm_unit = MCPWM_UNIT_0,           // 使用MCPWM单元0:cite[1]
        .timer_num = MCPWM_TIMER_0,           // 使用定时器0:cite[1]
        .io_signal = MCPWM0A,                 // 使用PWM0A输出
        .pwm_num = 20,  
        .c0_num  = 12,
        .c1_num  = 11,           // 根据实际接线修改
        .pwm_freq_hz = 25000,                 // 25kHz PWM频率
    };
    
    // 初始化电机
    esp_err_t ret = bldc_motor_init(&motor_config, &motor);
    if (ret != ESP_OK) {
        ESP_LOGE("MAIN", "电机初始化失败");
        vTaskDelete(NULL);
        return;
    }
    
    while (1) {
        // 加速阶段
        ESP_LOGI("MAIN", "加速阶段");
        for (int duty = 0; duty <= 80; duty += 10) {
            bldc_motor_set_speed(motor, (float)duty);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // 减速阶段
        ESP_LOGI("MAIN", "减速阶段");
        for (int duty = 80; duty >= 0; duty -= 10) {
            bldc_motor_set_speed(motor, (float)duty);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // 高速测试
        ESP_LOGI("MAIN", "高速测试");
        bldc_motor_set_speed(motor, 95.0f);
        vTaskDelay(pdMS_TO_TICKS(3000));
        
        // 紧急停止测试
        ESP_LOGI("MAIN", "紧急停止");
        bldc_motor_stop(motor);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    
    // 清理资源
    bldc_motor_deinit(motor);
    vTaskDelete(NULL);
}

void app_main() {
    // 创建电机控制任务
    xTaskCreate(motor_control_task, "motor_control", 4096, NULL, 5, NULL);
}