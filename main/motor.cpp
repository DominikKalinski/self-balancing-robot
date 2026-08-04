#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>
#include "motor.h"
#include "driver/gpio.h"
#include "gpio-controller.h"
#include "pcnt-controller.h"


Motor::Motor(uint8_t dir_pin, uint8_t pwm_pin, uint8_t encoder_A_pin, uint8_t encoder_B_pin, PwmController::CHANNEL channel) : 
_forward(true), _dir_pin(dir_pin), _pwm_pin(pwm_pin), _encoder_A_pin(encoder_A_pin), _encoder_B_pin(encoder_B_pin), 
_rpm(0), _channel(channel), _pcntController(encoder_A_pin, encoder_B_pin), _previous_time_us(0)
{
    GpioController::setDirection(_dir_pin, GpioController::DIRECTION::OUTPUT);
    GpioController::setDirection(_pwm_pin, GpioController::DIRECTION::OUTPUT);
    // GpioController::setDirection(_encoder_A_pin, GpioController::DIRECTION::INPUT);
    // GpioController::setDirection(_encoder_B_pin, GpioController::DIRECTION::INPUT);
    GpioController::setState(_dir_pin, GpioController::STATE::LOW);
    GpioController::setState(_pwm_pin, GpioController::STATE::LOW);
     //_pwmController.pwm_pin_init(_pwm_pin, _channel);
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

void Motor::set_pwm(uint8_t pwm_percentage)
{
    //_pwmController.pwm_set(pwm_percentage, _channel);
}

uint8_t Motor::pwm_pin()
{
    return _pwm_pin;
}

void Motor::start_rpm_task()
{
    xTaskCreate(update_rpm_task_A, "update_rpm_task", configMINIMAL_STACK_SIZE * 6, this, 5, NULL);
}

PcntController& Motor::pcntController()
{
    return _pcntController;
}

double& Motor::rpm()
{
    return _rpm;
}

int64_t& Motor::previous_time_us()
{
    return _previous_time_us;
}

int Motor::encoder_a_pin() const
{
    return static_cast<int>(_encoder_A_pin);
}

//PRIVATE       //PRIVATE       //PRIVATE       //PRIVATE       //PRIVATE
void Motor::update_rpm_task_A(void* parameter)
{
    Motor* motor = static_cast<Motor*>(parameter);
    while(true)
    {
        printf("Encoder Pin: %d\n", motor->encoder_a_pin());
        int current_pulses = motor->pcntController().get_pulses();
        int64_t current_time_us = esp_timer_get_time();
        int64_t difference_us = current_time_us - motor->previous_time_us();
        motor->rpm() = (static_cast<double>(current_pulses) / (static_cast<double>(difference_us) / 60000000)) / 16;
        motor->previous_time_us() = current_time_us;
        printf("Pulses: %d\n\n", motor->pcntController().get_pulses());
        motor->pcntController().clear_pulses();
        vTaskDelay(pdMS_TO_TICKS(100));
        
    }
}

