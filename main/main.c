#include "motor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "config.h"
void motor_control_task(void *pvParameter) {

    // 初始化电机
    bldc_motor_init(&motor_config1, &motor1);
    bldc_motor_init(&motor_config2, &motor2);
    bldc_motor_init(&motor_config3, &motor3);
    bldc_motor_init(&motor_config4, &motor4);

    
    while (1) {
        // 加速阶段
        ESP_LOGI("MAIN", "正加速阶段");
        for (int duty = 0; duty <= 80; duty += 10) {
            bldc_motor_set_speed(motor1, (float)duty);
            bldc_motor_set_speed(motor2, (float)duty);
            bldc_motor_set_speed(motor3, (float)duty);
            bldc_motor_set_speed(motor4, (float)duty);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // 减速阶段
        ESP_LOGI("MAIN", "正减速阶段");
        for (int duty = 80; duty >= 0; duty -= 10) {
            bldc_motor_set_speed(motor1, (float)duty);
            bldc_motor_set_speed(motor2, (float)duty);
            bldc_motor_set_speed(motor3, (float)duty);
            bldc_motor_set_speed(motor4, (float)duty);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        
        vTaskDelay(pdMS_TO_TICKS(2000));
        // 加速阶段
        ESP_LOGI("MAIN", "负加速阶段");
        for (int duty = 0; duty >= -80; duty -= 10) {
            bldc_motor_set_speed(motor1, (float)duty);
            bldc_motor_set_speed(motor2, (float)duty);
            bldc_motor_set_speed(motor3, (float)duty);
            bldc_motor_set_speed(motor4, (float)duty);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        
        vTaskDelay(pdMS_TO_TICKS(2000));
        
        // 减速阶段
        ESP_LOGI("MAIN", "负减速阶段");
        for (int duty = -80; duty <= 0; duty += 10) {
            bldc_motor_set_speed(motor1, (float)duty);
            bldc_motor_set_speed(motor2, (float)duty);
            bldc_motor_set_speed(motor3, (float)duty);
            bldc_motor_set_speed(motor4, (float)duty);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        
        vTaskDelay(pdMS_TO_TICKS(2000));
        // 高速测试
        ESP_LOGI("MAIN", "高速测试");
        bldc_motor_set_speed(motor1, 95.0f);
        bldc_motor_set_speed(motor2, 95.0f);
        bldc_motor_set_speed(motor3, 95.0f);
        bldc_motor_set_speed(motor4, 95.0f);
        vTaskDelay(pdMS_TO_TICKS(3000));
        
        // 紧急停止测试
        ESP_LOGI("MAIN", "紧急停止");
        bldc_motor_stop(motor1);
        bldc_motor_stop(motor2);
        bldc_motor_stop(motor3);
        bldc_motor_stop(motor4);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    
    // 清理资源
    bldc_motor_deinit(motor1);
    bldc_motor_deinit(motor2);
    bldc_motor_deinit(motor3);
    bldc_motor_deinit(motor4);
    vTaskDelete(NULL);
}

void app_main() {
    // 创建电机控制任务
    xTaskCreate(motor_control_task, "motor_control", 4096, NULL, 5, NULL);
}