#include "config.h"






void app_main()
{
    

    as5600_init(bus_cfg,as5600_cfg);
    pca9548_init(bus_cfg,pca9548_cfg);
    bldc_motor_init(&motor_config1,&motor1);
    bldc_motor_init(&motor_config2,&motor2);
    bldc_motor_init(&motor_config3,&motor3);
    bldc_motor_init(&motor_config4,&motor4);

    crsf_init(UART_NUM_1,13,14,uart_buff);



    while (1)
    {
        
        crsf_get_ch(UART_NUM_1);
        duty = (((float)channel_data.ch3-960)/815)*60;
        printf("spd %f",duty);
        bldc_motor_set_speed(motor1,duty);
        bldc_motor_set_speed(motor2,duty);
        bldc_motor_set_speed(motor3,duty);
        bldc_motor_set_speed(motor4,duty);
        for (size_t i = 0; i < 4; i++)
        {
            pca9548_setchannel(i);
            as5600_read_raw_angle(as5600_handle,&raw_angle);
            printf("angle %d",raw_angle);
        }
        printf("\n");


    }
}
