#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>
#include "motor.h"
#include "driver/gpio.h"
#include "gpio-controller.h"
#include "pcnt-controller.h"


Motor::Motor(uint8_t dir_pin, uint8_t pwm_pin, uint8_t encoder_A_pin, uint8_t encoder_B_pin, PwmController::CHANNEL channel) : 
_forward(true), _dir_pin(dir_pin), _pwm_pin(pwm_pin), _encoder_A_pin(encoder_A_pin), _encoder_B_pin(encoder_B_pin), 
_rpm(0), _channel(channel), _pcntController(encoder_A_pin, encoder_B_pin), _counter(0)
{
    GpioController::setDirection(_dir_pin, GpioController::DIRECTION::OUTPUT);
    GpioController::setDirection(_pwm_pin, GpioController::DIRECTION::OUTPUT);
    GpioController::setDirection(_encoder_A_pin, GpioController::DIRECTION::INPUT);
    GpioController::setDirection(_encoder_B_pin, GpioController::DIRECTION::INPUT);
    GpioController::setState(_dir_pin, GpioController::STATE::LOW);
    GpioController::setState(_pwm_pin, GpioController::STATE::LOW);
     _pwmController.pwm_pin_init(_pwm_pin, _channel);
}

void Motor::set_dir(Motor::DIRECTION direction)
{
    switch(direction)
    {
        case Motor::DIRECTION::FORWARD:
        {
            GpioController::setState(_dir_pin, GpioController::STATE::LOW);
            _forward = true;
            break;
        }
        case Motor::DIRECTION::REVERSE:
        {
            GpioController::setState(_dir_pin, GpioController::STATE::HIGH);
            _forward = false;
            break;
        }
    }
}

void Motor::switch_dir()
{
    _forward = !_forward;
    if(_forward)
    {
        GpioController::setState(_dir_pin, GpioController::STATE::HIGH);
    }
    else
    {
        GpioController::setState(_dir_pin, GpioController::STATE::LOW);
    }
    
}

void Motor::set_pwm(float pwm_percentage)
{
    _pwmController.pwm_set(pwm_percentage, _channel);
}

uint8_t Motor::pwm_pin()
{
    return _pwm_pin;
}



PcntController& Motor::pcntController()
{
    return _pcntController;
}

float Motor::rpm() const
{
    return _rpm;
}



int Motor::encoder_a_pin() const
{
    return static_cast<int>(_encoder_A_pin);
}

void Motor::update_rpm(float delta_time_seconds)
{
    int current_pulses = _pcntController.get_pulses();
    _rpm = current_pulses / 16.f / ((delta_time_seconds / 60.f));
    _pcntController.clear_pulses();
    //motor->rpm() = (static_cast<float>(current_pulses) / (static_cast<float>(difference_us) / 60000000)) / 16;
}

