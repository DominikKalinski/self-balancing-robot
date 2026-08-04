#pragma once
#include <mpu6050.h>
#include "driver/gpio.h"
#include "buzzer.h"
class ImuSensor
{
    public:
    ImuSensor();
    void init();
    void start_test();
    void calibrate();
    private:
    mpu6050_dev_t _dev;
    Buzzer _buzzer;
    static void mpu6050_test(void *pvParameters);
    float _average_acceleration_y_axis;
    float _average_rotation_y_axis;
};