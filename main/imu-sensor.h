#pragma once
#include <mpu6050.h>
#include "driver/gpio.h"
#include "buzzer.h"
#include "flash-storage.h"
class ImuSensor
{
    public:
    ImuSensor();
    void init();
    void metrics(mpu6050_acceleration_t*, mpu6050_rotation_t*);

    SemaphoreHandle_t _imu_mutex = nullptr;
    private:
    TaskHandle_t _task_handle;
    int64_t _previous_time;
    mpu6050_dev_t _dev;
    
};