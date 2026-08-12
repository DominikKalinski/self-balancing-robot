#pragma once
#include "motor.h"
#include "pwm-controller.h"
#include "button.h"
#include "angle-estimator.h"
class Robot
{
    public:
    Robot();
    void balance();
    void choose_direction();
    void start_rpm_task();
    void button_init();
    void load_balance_point_from_flash();
    void imu_sensor_init();
    void nullifier(float*, float*, float*);
    private:
    PwmController _pwmController;
    Motor _motor1;
    Motor _motor2;
    Button _button;
    ImuSensor _imuSensor;
    AngleEstimator _angleEstimator;
    float _output;
    float _previous_error;
    float _integral;
    int64_t _previous_time;
    int _counter;
    
    
    void set_motor_pwm(Motor&, float);
    void PID(float);
};