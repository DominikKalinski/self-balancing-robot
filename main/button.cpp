#include "robot.h"
#include "button.h"
#include "sdkconfig.h"
#include "gpio-controller.h"

Button::Button(Robot* robot) : _gpio(CONFIG_BUTTON_GPIO), _button_pressed(false), _button_queue{ }, _robot(robot)
{
    
}

void Button::isr_init() const
{
    gpio_config_t button_config;
    //_button_queue = xQueueCreate(10, sizeof(uint8_t));
    button_config.pin_bit_mask = 1ULL << _gpio;
    button_config.mode = GPIO_MODE_INPUT;
    button_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    button_config.pull_up_en = GPIO_PULLUP_ENABLE;
    button_config.intr_type = GPIO_INTR_NEGEDGE;

    ESP_ERROR_CHECK(gpio_config(&button_config));
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add( (gpio_num_t)_gpio, button_isr_handler, NULL));
}

void Button::button_isr_handler(void* arg)
{
    Button* button = static_cast<Button*>(arg);
    button->_button_pressed = true;
}



QueueHandle_t Button::button_queue() const
{
    return _button_queue;
}

uint8_t Button::gpio() const
{
    return _gpio;
}

Robot* Button::robot() const
{
    return _robot;
}

volatile bool &Button::button_pressed()
{
    return _button_pressed;
}