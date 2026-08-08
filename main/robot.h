#pragma once
#include "motor.h"
#include "pwm-controller.h"
#include "button.h"
#include "imu-sensor.h"
class Robot
{
    public:
    Robot();
    void set_motor_pwm(Motor&, uint8_t);
    void switch_direction(Motor&);
    // void start_rpm_task();
    void calibrate_imu();
    void pid();
    void start_balance_task();
    Motor& motor1();
    Motor& motor2();
    const ImuSensor* imuSensor() const;
    int64_t &previous_time();
    float &output();
    private:
    static void balance(void*);
    PwmController _pwmController;
    Motor _motor1;
    Motor _motor2;
    Button _button;
    ImuSensor _imuSensor;
    float _output;
    float _previous_error;
    float _integral;
    int64_t _previous_time;
    TaskHandle_t _task_handle;
};