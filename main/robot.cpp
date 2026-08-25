#include <cmath>
#include <algorithm>
#include "robot.h"
#include "esp_timer.h"
#include "driver/gpio.h"

#define PULSES_PER_REVOLUTION 3200
constexpr float MAX_PULSES_PER_SECOND = 10666.f;

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
        int64_t time_now = esp_timer_get_time();
        float delta_time_seconds = (time_now - robot->_previous_time_calibrate_task) / 1000000.f;
        robot->_previous_time_calibrate_task = time_now;

        int current_pulses = robot->_motor1.pcntController().pulses();
        float pulses_per_second = robot->_motor1.pulses_per_second();
        
        
        // if(std::abs(pulses_per_second) > 2000.f)
        // { 
        //     pulses_per_second = robot->brake(pulses_per_second); 
        // }
        //if(robot->_counter++ > 5) {printf("PPS: %f\n", pulses_per_second); robot->_counter = 0;}
        if(std::abs(robot->_previous_pulses_per_second) >= std::abs(pulses_per_second) + 8.f)
        {
            robot->_previous_pulses_per_second = pulses_per_second; 
            robot->_previous_pulses = current_pulses;
            vTaskDelay(pdMS_TO_TICKS(50));
            robot->_led.set_color(0, 0, 30);
            continue;
        }
        robot->_led.set_color(30, 0, 0);
        robot->_angle_goal += pulses_per_second * 0.000001f * delta_time_seconds;
      
        
        robot->_previous_pulses_per_second = pulses_per_second;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

float Robot::brake(float initial_pulses_per_second)
{
    int speed_goal = 0;
    float current_pulses = _motor1.pcntController().pulses();
    float previous_pulses = current_pulses;
    float pulses_per_second = initial_pulses_per_second;
    float speed_P = 4 / MAX_PULSES_PER_SECOND;
    float angle_rate_limit = 1.f;
    float delta_time_seconds = 0.005f; //same as vTaskDelay in this function
    int64_t time_now = esp_timer_get_time();
    float previous_time_seconds = time_now / 1000000.f;
    _led.set_color(50, 0, 0);
    while(true)
    {
        
        int speed_error = speed_goal + pulses_per_second;
        float desired_angle =  speed_P * speed_error;
        float max_change = angle_rate_limit * delta_time_seconds;
        
        if(desired_angle > _angle_goal_braking_offset)
        {
            _angle_goal_braking_offset += std::min(desired_angle - _angle_goal_braking_offset, max_change);
        }
        else if (desired_angle < _angle_goal_braking_offset)
        {
            _angle_goal_braking_offset -= std::min(_angle_goal_braking_offset - desired_angle, max_change);
        }

        if(initial_pulses_per_second < 0.f)
        {
            if(desired_angle < 0.1f && _angle_goal_braking_offset < 0.1f) 
            {
                _angle_goal_braking_offset = 0.f;
                //_led.set_color(0, 50, 0);
                return pulses_per_second;
            }
        }
        else
        {
            if(desired_angle > -0.1f && _angle_goal_braking_offset > -0.1f) 
            {
                _angle_goal_braking_offset = 0.f;
                //_led.set_color(0, 50, 0);
                return pulses_per_second;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5));
        time_now = esp_timer_get_time();
        float time_now_seconds = time_now / 1000000.f;
        delta_time_seconds = time_now_seconds - previous_time_seconds;
        previous_time_seconds = time_now_seconds;
        int current_pulses = _motor1.pcntController().pulses();
        pulses_per_second = (current_pulses - previous_pulses) / delta_time_seconds;
        previous_pulses = current_pulses;
        
    }

    return 0.f;
}
