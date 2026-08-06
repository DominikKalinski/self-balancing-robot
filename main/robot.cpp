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
        _button(this), _imuSensor()
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

void Robot::start_rpm_task()
{
    _motor1.start_rpm_task();
    _motor2.start_rpm_task();
}

void Robot::calibrate_imu()
{
    _imuSensor.calibrate();
}

void Robot::balance()
{
    
}

Motor& Robot::motor1()
{
    return _motor1;
}

Motor& Robot::motor2()
{
    return _motor2;
}