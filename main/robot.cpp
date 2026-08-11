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
        _button(this), _imuSensor(),_output(0.f), _previous_error(0.f), _integral(0.f), _previous_time(0), _counter(0)
{       
       _button.run_task();
       _imuSensor.init();
       //_imuSensor.start_test();
}

void Robot::balance()
{
   
    while(true)
    {
        _imuSensor.update_angles();
        pid();
        set_motor_pwm(_motor1, _output);
        set_motor_pwm(_motor2, _output);
        choose_direction();
        vTaskDelay(pdMS_TO_TICKS(5));
        // _counter++;
        // if(_counter > 20)
        // {
        //     _counter = 0;
        //     printf("Output: %f\n", _output);
        // }
    }
}

void Robot::set_motor_pwm(Motor& motor, float percentage)
{
    motor.set_pwm(percentage);
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

void Robot::calibrate_imu()
{
    _imuSensor.calibrate();
}

// void Robot::pid()
// {
//         int64_t time_now = esp_timer_get_time();
//         float delta_time = static_cast<float>((time_now - _previous_time ) / 1000000.f);
//         float error = _imuSensor.corrected_angle_x();
//         // printf("Delta time: %f\n", delta_time);
//         float Kp = 10.f;
//         float Ki = 0.0f;
//         float Kd = 1.0f;
//         float rotation = _imuSensor.correcter_rotation_x();
//         //printf("P: %f\nI: %f\nD: %f\n", Kp * error, Ki*(_integral += error * delta_time), Kd * (error - _previous_error / delta_time));
//         // printf("Error: %f\n", error);
//         // printf("Integral: %f\n", _integral);
//         float delta_error = error - _previous_error;
//         _integral = std::clamp(_integral, -10.f, 10.f);
//         _output = (Kp * error) + (Ki * (_integral += error * delta_time)) + Kd * rotation;
//         if(_output < 0.f)
//         {
//             _output -= 3.5f;
//         }
//         else if( _output > 0.f)
//         {
//             _output += 3.5f;
//         }
//         _previous_error = error;
//         _previous_time = time_now;

//         //printf("Integral %f\n", _integral);
       
//         //printf("Positive: %f\nNegative: %f\n", _testing_positive, _testing_negative);
        
//         //printf("PID output: %f\n", _output);
// }

void Robot::pid()
{
        int64_t time_now = esp_timer_get_time();
        float delta_time = static_cast<float>((time_now - _previous_time ) / 1000000.f);
        float error = _imuSensor.corrected_angle_x();
        // printf("Delta time: %f\n", delta_time);
        float Kp = 29.8f;
        float Ki = 1800.f;
        float Kd = 0.f;
        if(error > 1.f || error < -1.f)
        {
            Kd = 2.0f;
        }
        else
        {
            Kd = 0.f;
        }
        float rotation = _imuSensor.correcter_rotation_x();
        //printf("P: %f\nI: %f\nD: %f\n", Kp * error, Ki*(_integral += error * delta_time), Kd * (error - _previous_error / delta_time));
        // printf("Error: %f\n", error);
        // printf("Integral: %f\n", _integral);
        //_output = (Kp * error) + (Ki * (_integral += error * delta_time)) + (Kd * (error - _previous_error / delta_time));
       
        _output = (Kp * error) + (Ki * (_integral += error * delta_time)) + (Kd * rotation);
       if(_output < 3.f && _output > 0.f)
       {
        _output = 4.f;
       }
       else if (_output > -3.f && _output < 0.f)
       {
        _output = -4.f;
       }
       _integral = std::clamp(_integral, -0.17f, 0.17f);
       //printf("integral: %f\n", _integral);
        _previous_error = error;
        _previous_time = time_now;

        //printf("PID output: %f\n", _output);
}


Motor& Robot::motor1()
{
    return _motor1;
}

Motor& Robot::motor2()
{
    return _motor2;
}

const ImuSensor* Robot::imuSensor() const
{
    return &_imuSensor;
}
int64_t &Robot::previous_time()
{
    return _previous_time;
}
float &Robot::output()
{
    return _output;
}

