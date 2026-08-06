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
    void start_test();
    void calibrate();
    void update_angles();
    void print_averages() const;
    float angle_accelerator_y() const;
    float angle_gyroscope_y() const;
    SemaphoreHandle_t _imu_mutex = nullptr;

    private:
    mpu6050_dev_t _dev;
    Buzzer _buzzer;
    static void mpu6050_test(void *pvParameters);
    float _average_acceleration_y_axis;
    float _average_acceleration_z_axis;
    float _average_rotation_y_axis;
    float _angle_accelerator_y;
    float _angle_gyroscope_y;
    TaskHandle_t _task_handle;
    FlashStorage& _flashStorage;
    int64_t _previous_time;
    static constexpr float _PI = 3.1415927f;
};