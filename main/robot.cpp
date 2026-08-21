#include <cmath>
#include <algorithm>
#include "robot.h"
#include "esp_timer.h"
#include "driver/gpio.h"

#define PULSES_PER_REVOLUTION 3200

Robot::Robot() : 
        _motor1(CONFIG_MOTOR1_DIR_GPIO, 
        CONFIG_MOTOR1_PWM_GPIO, 
        CONFIG_MOTOR1_ENCODER_A_YELLOW, 
        CONFIG_MOTOR1_ENCODER_B_WHITE,
        PwmController::CHANNEL::CHANNEL0), 

        _motor2(CONFIG_MOTOR2_DIR_GPIO, 
        CONFIG_MOTOR2_PWM_GPIO, 
        CONFIG_MOTOR2_ENCODER_A_YELLOW, 
        CONFIG_MOTOR2_ENCODER_B_WHITE,
        PwmController::CHANNEL::CHANNEL1),
        _button(this),_imuSensor(), _angleEstimator(&_imuSensor),_output(0.f), _previous_error(0.f), _integral(0.f), _braking_integral(0.f) , _time_now(0),
         _previous_time(0), _previous_time_calibrate_task(0), _angle_goal(0.f), _angle_goal_braking_offset(0.f), _previous_pulses_per_second(0.f) ,_counter(0), 
         _direction(Robot::DIRECTION::NONE), _is_being_controlled(false), _is_braking(false), _brake_timer_set(false), _previous_pulses(0)
{       
}

void Robot::imu_sensor_init()
{
    _imuSensor.init();
}

void Robot::button_init()
{
    _button.isr_init();
}

void Robot::start_rpm_task()
{
    _motor1.start_rpm_task();
    _motor2.start_rpm_task();
}

void Robot::start_auto_calibration_task()
{
    xTaskCreate(auto_calibrate, "auto_calibrate", 1024 * 6, this, 5, nullptr);
}

void Robot::load_balance_point_from_flash()
{
    _angleEstimator.load_balance_points_from_flash();
}

void Robot::balance()
{
   
    while(true)
    {
        _time_now = esp_timer_get_time();
        float delta_time_seconds = (_time_now - _previous_time) / 1000000.f;
        _previous_time = _time_now;
        PID(delta_time_seconds, _angle_goal, _angle_goal_braking_offset);
        set_motor_pwm(_motor1, _output);
        set_motor_pwm(_motor2, _output);
        choose_direction();
        //move(_direction);
        vTaskDelay(pdMS_TO_TICKS(5));

        //if(_counter++ > 20) {printf("Motor1 Pulses: %d\nMotor2 Pulses: %d\n\n", _motor1.pcntController().pulses(), _motor2.pcntController().pulses()); _counter = 0;}
        
        if(_button.is_pressed())
        {
            set_motor_pwm(_motor1, 0.f);
            set_motor_pwm(_motor2, 0.f);
            _angleEstimator.calibrate_balance_point(); //and store to flash
            _button.is_pressed() = false;
        }
    }
}


void Robot::move(Robot::DIRECTION direction)
{
    switch (direction)
    {
        case Robot::DIRECTION::FORWARD: _angle_goal = 0.5f; break;
        case Robot::DIRECTION::REVERSE: _angle_goal = -0.5f; break;
        case Robot::DIRECTION::NONE: _angle_goal = 0.f; break;
    }
    if(std::abs(_motor1.rpm()) > 500.f || std::abs(_motor2.rpm()) > 500.f)
    {
        _angle_goal = 0.f;
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




void Robot::nullifier(float* P, float *I, float* D)
{
    *P = 0.f;
    *I = 0.f;
    *D = 0.f;
}




void Robot::PID(float delta_time_seconds, float angle_goal, float angle_goal_braking_offset)
{
    float P = 10.8f;  //10.8
    float I = 200.f;  // 200
    float D = 0.0f;
    //nullifier(&P, &I, &D);
    _output = _angleEstimator.PID(P, I, D, delta_time_seconds, angle_goal, angle_goal_braking_offset);
}

void Robot::set_motor_pwm(Motor& motor, float percentage)
{
    motor.set_pwm(percentage);
}


void Robot::auto_calibrate(void* arg)
{
    Robot* robot = static_cast<Robot*>(arg);
    while(true)
    {
        if(robot->_is_braking) { robot->_led.set_color(120, 0, 0); }
        else {robot->_led.set_color(0, 120, 0);}
        int64_t time_now = esp_timer_get_time();
        int current_pulses = robot->_motor1.pcntController().pulses();
        float delta_time_seconds = (time_now - robot->_previous_time_calibrate_task) / 1000000.f;
        robot->_previous_time_calibrate_task = time_now;
        float pulses_per_second = (current_pulses - robot->_previous_pulses) / delta_time_seconds;
        if(std::abs(robot->_previous_pulses_per_second) >= std::abs(pulses_per_second) || robot->_is_braking)
        {
            if(std::abs(pulses_per_second) > 2000.f){robot->_is_braking = true; }

            if(robot->_is_braking) { robot->brake(pulses_per_second, &time_now); }
            
            robot->_previous_pulses_per_second = pulses_per_second;

            robot->_previous_pulses = current_pulses;
            vTaskDelay(pdMS_TO_TICKS(25));
            continue;
        }
        if(robot->_counter++ > 20) {printf("Delta time: %f\n\n", delta_time_seconds); robot->_counter = 0; } 
        
        if(!robot->_is_braking) {robot->_angle_goal += pulses_per_second * 0.00001f;}
      
        robot->_previous_pulses = current_pulses;
        robot->_previous_pulses_per_second = pulses_per_second;
        vTaskDelay(pdMS_TO_TICKS(25));
    }
}

void Robot::brake(float pulses_per_second, const int64_t* time_now)
{
    static float time_start_seconds = 0.f;
    float time_now_seconds = *time_now / 1000000.f;
    if(!_brake_timer_set)
    {
        time_start_seconds = *time_now / 1000000.f;
        _brake_timer_set = true;
    }
    float speed_factor = std::abs(pulses_per_second / 10666.f);
    if(pulses_per_second < 0.f)
    {
        _angle_goal_braking_offset = (powf(speed_factor, 2.7f)) * 4.f * -1.f;
    }
    else
    {
        _angle_goal_braking_offset = powf(speed_factor, 2.7f) * 4.f;
    }
    if(std::abs(pulses_per_second) < 1000 || (time_now_seconds - time_start_seconds) > 2.f) 
    {
        _is_braking = false;
        _angle_goal_braking_offset = 0.f;
        _brake_timer_set = false;
    }
}
