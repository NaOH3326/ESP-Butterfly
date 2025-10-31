#include "MyServo.h"
//初始化函数，参数为引脚和信道
void servo_init(int pin, int channel) {
    ledc_timer_config_t ledc_timer = {
    .speed_mode = SG90_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_13_BIT,
    .timer_num = SG90_TIMER,
    .freq_hz = SG90_FREQ_HZ,
    .clk_cfg = LEDC_AUTO_CLK
    };
ledc_timer_config(&ledc_timer);

ledc_channel_config_t ledc_channel = {
    .channel = channel,
    .duty = 0,
    .gpio_num = pin,
    .speed_mode = SG90_SPEED_MODE,
    .hpoint = 0,
    .timer_sel = SG90_TIMER
    };
ledc_channel_config(&ledc_channel);
    }

//设置舵机角度，参数为角度和信道
void servo_set_angle(float angle,int channel) {
    uint32_t duty = (500 + (angle / 180.0) * 2000) * (1 << 13) / 20000;
    ledc_set_duty(SG90_SPEED_MODE, channel, duty);
    ledc_update_duty(SG90_SPEED_MODE, channel);
    }