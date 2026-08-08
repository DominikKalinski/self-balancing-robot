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
    float angle_accelerator_x() const;
    float angle_gyroscope_x() const;
    float corrected_angle_x() const;
    SemaphoreHandle_t _imu_mutex = nullptr;
    private:
    mpu6050_dev_t _dev;
    Buzzer _buzzer;
    static void mpu6050_test(void *pvParameters);
    float _average_acceleration_y_axis;
    float _average_acceleration_z_axis;
    float _average_rotation_x_axis;
    float _angle_accelerator_x;
    float _angle_gyroscope_x;
    float _corrected_angle_x;
    TaskHandle_t _task_handle;
    FlashStorage& _flashStorage;
    int64_t _previous_time;
    static constexpr float _PI = 3.1415927f;
};