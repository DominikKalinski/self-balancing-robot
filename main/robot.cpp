#include <algorithm>
#include "robot.h"

Robot::Robot() : _motor1(CONFIG_MOTOR1_DIR_GPIO, 
        CONFIG_MOTOR1_PWM_GPIO, 
        CONFIG_MOTOR1_ENCODER_A_YELLOW, 
        CONFIG_MOTOR1_ENCODER_B_WHITE,
        PwmController::CHANNEL::CHANNEL0), 

        _motor2(CONFIG_MOTOR2_DIR_GPIO, 
        CONFIG_MOTOR2_PWM_GPIO, 
        CONFIG_MOTOR2_ENCODER_A_YELLOW, 
        CONFIG_MOTOR2_ENCODER_B_WHITE,
        PwmController::CHANNEL::CHANNEL1),
        _button(this),_imuSensor(), _angleEstimator(&_imuSensor),_output(0.f), _previous_error(0.f), _integral(0.f), _previous_time(0), _counter(0)
{       
    
}

void Robot::balance()
{
   
    while(true)
    {
        PID();
        set_motor_pwm(_motor1, _output);
        set_motor_pwm(_motor2, _output);
        choose_direction();
        vTaskDelay(pdMS_TO_TICKS(5));

        if(_button.button_pressed())
        {
            _angleEstimator.calibrate_balance_point();
            _button.button_pressed() = false;
        }
    }
}



void Robot::choose_direction()
{
    if(_output > 0)
    {
        _motor1.set_dir(Motor::DIRECTION::FORWARD);
        _motor2.set_dir(Motor::DIRECTION::FORWARD);
    }
    else
    {
        _motor1.set_dir(Motor::DIRECTION::REVERSE);
        _motor2.set_dir(Motor::DIRECTION::REVERSE);
    }
}

void Robot::start_rpm_task()
{
    _motor1.start_rpm_task();
    _motor2.start_rpm_task();
}


void Robot::button_init() const
{
    _button.isr_init();
}

void Robot::imu_sensor_init()
{
    _imuSensor.init();
}





void Robot::PID()
{
    float P = 29.8f;
    float I = 1200.f;
    float D = 0.f;
    _output = _angleEstimator.PID(P, I, D);
}

void Robot::set_motor_pwm(Motor& motor, float percentage)
{
    motor.set_pwm(percentage);
}