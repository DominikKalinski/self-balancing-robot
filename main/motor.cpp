#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>
#include <cmath>
#include "motor.h"
#include "driver/gpio.h"
#include "gpio-controller.h"
#include "pcnt-controller.h"

uint8_t Motor::_motor_count = 0;
constexpr uint8_t PULSE_RESET_COUNT = 4;

Motor::Motor(uint8_t dir_pin, uint8_t pwm_pin, uint8_t encoder_A_pin, uint8_t encoder_B_pin, PwmController::CHANNEL channel) : 
_forward(true), _dir_pin(dir_pin), _pwm_pin(pwm_pin), _encoder_A_pin(encoder_A_pin), _encoder_B_pin(encoder_B_pin), 
_rpm(0), _channel(channel), _pcntController(encoder_A_pin, encoder_B_pin, this), _timestamp1(0), _timestamp2(0), _write_to_timestamp1(false) , _motor_number(++_motor_count)
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

Motor::DIRECTION Motor::direction()
{
    return _direction;
}

void Motor::set_pwm(float pwm_percentage)
{
    _pwmController.pwm_set(pwm_percentage, _channel);
}

float Motor::pulses_per_second()
{
    float delta_time_seconds = 0.f;
    
    static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);
    int64_t timestamp1_temp = _timestamp1;
    int64_t timestamp2_temp = _timestamp2;
    bool write_to_timestamp1_temp = _write_to_timestamp1;
    Motor::DIRECTION direction_temp = _direction;
    portEXIT_CRITICAL(&mux);
    int64_t time_now = esp_timer_get_time();
    if(time_now - timestamp1_temp > 400000)
    {
        return 0.f;
    }
    if(write_to_timestamp1_temp)
    {
        delta_time_seconds = (timestamp1_temp - timestamp2_temp) / 1000000.f;
    }
    else
    {
        delta_time_seconds = (timestamp2_temp - timestamp1_temp) / 1000000.f;
    }
    if(delta_time_seconds == 0.f)
    {
        return 0.f;
    }
    float pulses_per_second = PULSE_RESET_COUNT / delta_time_seconds;  //pulse reset count is when the PCNT counter resets, each motor revolution gives 64 pulses
    if(direction_temp == Motor::DIRECTION::FORWARD)
    {
        pulses_per_second *= -1.f;
    }

    if (std::fabsf(pulses_per_second) > 8000.f)
    {
        printf("t1=%lld\n t2=%lld\n flag=%d\n dt=%lld\n\n",
               timestamp1_temp,
               timestamp2_temp,
               write_to_timestamp1_temp,
               (long long)(delta_time_seconds * 1000000.f));
    }
    
    return pulses_per_second;
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

void Motor::store_timestamps_for_rotation_speed(int64_t timestamp, DIRECTION direction)
{
    if(_write_to_timestamp1)
    {
        _timestamp1 = timestamp;
    }
    else
    {
        _timestamp2 = timestamp;
    }
    _direction = direction;
    _write_to_timestamp1 = !_write_to_timestamp1;
}

