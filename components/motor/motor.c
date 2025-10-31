#include "motor.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "BLDC_MOTOR";




// 配置MCPWM
static esp_err_t configure_mcpwm(motor_handle_t *handle) {
    esp_err_t ret; 
    
    // 配置MCPWM定时器
    mcpwm_config_t pwm_config = {
        .frequency = handle->config.pwm_freq_hz,
        .cmpr_a = 0.0,  // 初始占空比0%
        .cmpr_b = 0.0,
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0,
    };
    
    ret = mcpwm_init(handle->config.mcpwm_unit, handle->config.timer_num, &pwm_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "MCPWM初始化失败: %s", esp_err_to_name(ret));
        return ret;
    }
    
    return ESP_OK;
}

esp_err_t bldc_motor_init(const motor_config_t *config, motor_handle_t **handle) {
    if (config == NULL || handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // 验证GPIO引脚
    if (!GPIO_IS_VALID_OUTPUT_GPIO(config->pwm_num)) {
        ESP_LOGE(TAG, "无效的GPIO引脚: %d", config->pwm_num);
        return ESP_ERR_INVALID_ARG;
    }
    esp_rom_gpio_pad_select_gpio(config->c0_num);
    gpio_set_direction(config->c0_num, GPIO_MODE_OUTPUT);
    esp_rom_gpio_pad_select_gpio(config->c1_num);
    gpio_set_direction(config->c1_num, GPIO_MODE_OUTPUT);

    // 分配句柄内存
    motor_handle_t *new_handle = calloc(1, sizeof(motor_handle_t));
    if (new_handle == NULL) {
        return ESP_ERR_NO_MEM;
    }
    
    new_handle->config = *config;
    new_handle->current_duty = 0.0;
    new_handle->is_initialized = false;
    
    ESP_LOGI(TAG, "初始化电机: 单元=%d, 定时器=%d, GPIO=%d, 频率=%dHz", 
             config->mcpwm_unit, config->timer_num, config->pwm_num, config->pwm_freq_hz);
    
    // 设置MCPWM GPIO
    esp_err_t ret = mcpwm_gpio_init(config->mcpwm_unit, config->io_signal, config->pwm_num);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "MCPWM GPIO初始化失败: %s", esp_err_to_name(ret));
        free(new_handle);
        return ret;
    }
    
    // 配置MCPWM
    ret = configure_mcpwm(new_handle);
    if (ret != ESP_OK) {
        free(new_handle);
        return ret;
    }
    
    new_handle->is_initialized = true;
    *handle = new_handle;
    
    ESP_LOGI(TAG, "电机初始化成功");
    gpio_set_level(config->c0_num,0);
    gpio_set_level(config->c1_num,0);
    return ESP_OK;
}

esp_err_t bldc_motor_set_speed(motor_handle_t *handle, float duty_cycle) {
    if (handle == NULL || !handle->is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    if (duty_cycle >=0)
    {
    ESP_ERROR_CHECK(gpio_set_level(handle->config.c0_num,0));
    ESP_ERROR_CHECK(gpio_set_level(handle->config.c1_num,1));
       // 限制占空比范围
    if (duty_cycle < 0.0) duty_cycle = 0.0;
    if (duty_cycle > 100.0) duty_cycle = 100.0;
    
    // 设置占空比
    esp_err_t ret = mcpwm_set_duty(handle->config.mcpwm_unit, handle->config.timer_num, 
                                  MCPWM_OPR_A, duty_cycle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "设置占空比失败: %s", esp_err_to_name(ret));
        return ret;
    }
    
    handle->current_duty = duty_cycle;
    ESP_LOGI(TAG, "设置占空比: %.1f%%", duty_cycle);
    
    return ESP_OK;  
    }
    if (duty_cycle < 0)
    {
    float duty_cycle_a;
    ESP_ERROR_CHECK(gpio_set_level(handle->config.c0_num,1));
    ESP_ERROR_CHECK(gpio_set_level(handle->config.c1_num,0));
    duty_cycle_a = 0-(duty_cycle);
       // 限制占空比范围
    if (duty_cycle_a < 0.0) duty_cycle = 0.0;
    if (duty_cycle_a > 100.0) duty_cycle = 100.0;
    
    // 设置占空比
    esp_err_t ret = mcpwm_set_duty(handle->config.mcpwm_unit, handle->config.timer_num, 
                                  MCPWM_OPR_A, duty_cycle_a);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "设置占空比失败: %s", esp_err_to_name(ret));
        return ret;
    }
    
    handle->current_duty = duty_cycle;
    ESP_LOGI(TAG, "设置占空比: %.1f%%", duty_cycle_a);
    
    return ESP_OK; 
    }
     
   return ESP_OK; 
}

esp_err_t bldc_motor_stop(motor_handle_t *handle) {
    if (handle == NULL || !handle->is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    // 设置占空比为0%
    esp_err_t ret = mcpwm_set_duty(handle->config.mcpwm_unit, handle->config.timer_num, 
                                  MCPWM_OPR_A, 0.0);
                                  gpio_set_level(handle->config.c0_num,0);
                                  gpio_set_level(handle->config.c1_num,0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "停止电机失败: %s", esp_err_to_name(ret));
        return ret;
    }
    
    handle->current_duty = 0.0;
    ESP_LOGI(TAG, "电机已停止");
    
    return ESP_OK;
}

esp_err_t bldc_motor_deinit(motor_handle_t *handle) {
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    if (handle->is_initialized) {
        // 停止电机
        bldc_motor_stop(handle);
        
        // 停止MCPWM定时器
        mcpwm_stop(handle->config.mcpwm_unit, handle->config.timer_num);
        
        handle->is_initialized = false;
    }
    
    free(handle);
    ESP_LOGI(TAG, "电机资源已释放");
    
    return ESP_OK;
}