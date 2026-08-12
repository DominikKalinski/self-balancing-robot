#include <algorithm>
#include "robot.h"
#include "esp_timer.h"
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
        int64_t time_now = esp_timer_get_time();
        float delta_time_seconds = (time_now - _previous_time) / 1000000.f;
        _previous_time = time_now;
        PID(delta_time_seconds);
        set_motor_pwm(_motor1, _output);
        set_motor_pwm(_motor2, _output);
        choose_direction();
        vTaskDelay(pdMS_TO_TICKS(5));
        

        if(_button.button_pressed())
        {
            set_motor_pwm(_motor1, 0.f);
            set_motor_pwm(_motor2, 0.f);
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


void Robot::button_init()
{
    _button.isr_init();
}

void Robot::imu_sensor_init()
{
    _imuSensor.init();
}



void Robot::nullifier(float* P, float *I, float* D)
{
    *P = 0.f;
    *I = 0.f;
    *D = 0.f;
}

void Robot::PID(float delta_time_seconds)
{
    float P = 10.8f;
    float I = 100.f;
    float D = 0.f;
    //nullifier(&P, &I, &D);
    _output = _angleEstimator.PID(P, I, D, delta_time_seconds);
}

void Robot::set_motor_pwm(Motor& motor, float percentage)
{
    motor.set_pwm(percentage);
}