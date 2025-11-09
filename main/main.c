#include "config.h"






void app_main()
{
    i2cdev_init();

    as5600_init(bus_cfg,as5600_cfg);
    pca9548_init(bus_cfg,pca9548_cfg);
    bldc_motor_init(&motor_config1,&motor1);
    bldc_motor_init(&motor_config2,&motor2);
    bldc_motor_init(&motor_config3,&motor3);
    bldc_motor_init(&motor_config4,&motor4);

    crsf_init(UART_NUM_1,14,13,uart_buff);

    ESP_ERROR_CHECK(mpu6050_init_desc(&dev, ADDR, 1, 21, 12));
    ESP_ERROR_CHECK(mpu6050_init(&dev));

    while (1)
    {
        
        crsf_get_ch(UART_NUM_1);
        duty = (((float)channel_data.ch3-960)/815)*100;
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

        float temp;
        mpu6050_acceleration_t accel = { 0 };
        mpu6050_rotation_t rotation = { 0 };
        ESP_ERROR_CHECK(mpu6050_get_temperature(&dev, &temp));
        ESP_ERROR_CHECK(mpu6050_get_motion(&dev, &accel, &rotation));
        printf( "Acceleration: x=%.4f   y=%.4f   z=%.4f\n", accel.x, accel.y, accel.z);
        printf( "Rotation:     x=%.4f   y=%.4f   z=%.4f\n", rotation.x, rotation.y, rotation.z);
        printf( "Temperature:  %.1f\n", temp);

    }
}
