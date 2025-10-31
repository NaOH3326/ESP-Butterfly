#ifndef _MYSERVO_H_
#define _MYSERVO_H_

#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/ledc.h"

#define SG90_FREQ_HZ 5000
#define SG90_TIMER LEDC_TIMER_0
#define SG90_SPEED_MODE LEDC_LOW_SPEED_MODE

void servo_init(int pin, int channel) ;
void servo_set_angle(float angle,int channel) ;

#endif