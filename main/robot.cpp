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
        _button(this), _imuSensor(),_output(0.f), _previous_error(0.f), _integral(0.f), _previous_time(0),
        _task_handle(nullptr)
{       
       _button.run_task();
       _imuSensor.init();
       _imuSensor.start_test();
}

void Robot::set_motor_pwm(Motor& motor, uint8_t percentage)
{
    motor.set_pwm(percentage);
}

void Robot::switch_direction(Motor& motor)
{
    motor.switch_dir();
}

// void Robot::start_rpm_task()
// {
//     _motor1.start_rpm_task();
//     _motor2.start_rpm_task();
// }

void Robot::calibrate_imu()
{
    _imuSensor.calibrate();
}

void Robot::pid()
{
        int64_t time_now = esp_timer_get_time();
        float delta_time = static_cast<float>((time_now - _previous_time ) / 1000000.f);
        float error = _imuSensor.corrected_angle_x();
        printf("Delta time: %f\n", delta_time);
        int Kp = 100;
        int Ki = 0;
        int Kd = 100;
        printf("P: %f\nI: %f\nD: %f\n", Kp * error, Ki*(_integral += error * delta_time), Kd * (error - _previous_error / delta_time));
        printf("Error: %f\n", error);
        printf("Integral: %f\n", _integral);
        _output = (Kp * error) + (Ki * (_integral += error * delta_time)) + (Kd * (error - _previous_error / delta_time));
        _previous_error = error;
        _previous_time = time_now;
        printf("PID output: %f\n", _output);
}

void Robot::start_balance_task()
{
    BaseType_t result = xTaskCreate(balance, "balance_task", configMINIMAL_STACK_SIZE * 6, this, 5, &_task_handle);
    

    configASSERT(result == pdPASS);
    configASSERT(_task_handle != nullptr);
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

void Robot::balance(void* arg)
{
    while(true)
    {
        Robot* robot = static_cast<Robot*>(arg);
        robot->_imuSensor.update_angles();
        robot->pid();
        
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}