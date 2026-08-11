#pragma once
#include "imu-sensor.h"
#include "buzzer.h"
class AngleEstimator
{
    public:
    AngleEstimator(ImuSensor*);
    void load_balance_points_from_flash();
    float PID(float, float, float);
    void update_angles();
    void calibrate_balance_point();
    private:
    float _integral;
    float _average_acceleration_y_axis;
    float _average_acceleration_z_axis;
    float _average_rotation_x_axis;
    float _corrected_angle_x;
    float _corrected_rotation_x;
    int64_t _previous_time;
    ImuSensor* _imuSensor;
    Buzzer _buzzer;
    FlashStorage& _flashStorage;
    static constexpr float _PI = 3.1415927f;
};