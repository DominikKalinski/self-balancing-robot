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
    void start_rpm_task();
    void calibrate_imu();
    Motor& motor1();
    Motor& motor2();
    private:
    PwmController _pwmController;
    Motor _motor1;
    Motor _motor2;
    Button _button;
    ImuSensor _imuSensor;
};