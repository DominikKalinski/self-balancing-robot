#pragma once
#include "motor.h"
#include "pwm-controller.h"
#include "button.h"
#include "angle-estimator.h"
class Robot
{
    public:
    enum class DIRECTION
    {
        FORWARD,
        REVERSE,
        NONE
    };
    Robot();
    void imu_sensor_init();
    void button_init();
    void start_rpm_task();
    void load_balance_point_from_flash();
    void balance();
    void move(Robot::DIRECTION);
    void choose_direction();
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
    float _angle_goal;
    int _counter;
    Robot::DIRECTION _direction;
    
    void set_motor_pwm(Motor&, float);
    void PID(float, float);

};