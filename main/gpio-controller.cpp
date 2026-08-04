#include "gpio-controller.h"


void GpioController::setDirection(uint8_t pin, GpioController::DIRECTION DIRECTION)
{
    gpio_mode_t direction;
    gpio_num_t gpioPin;
    gpioPin = pinNumber(pin);
    direction = gpioDirection(DIRECTION);
    ESP_ERROR_CHECK(gpio_set_direction(gpioPin, direction));
}

void GpioController::setState(uint8_t pin, GpioController::STATE MODE)
{
    gpio_num_t gpioPin;
    gpioPin = pinNumber(pin);
    switch(MODE)
    {
        case GpioController::STATE::HIGH: ESP_ERROR_CHECK(gpio_set_level(gpioPin, 1)); break;
        case GpioController::STATE::LOW: ESP_ERROR_CHECK(gpio_set_level(gpioPin, 0)); break;
    }
}

 



gpio_mode_t GpioController::gpioDirection(GpioController::DIRECTION direction)
{
    switch(direction)
    {
        case GpioController::DIRECTION::INPUT:
        {
            return GPIO_MODE_INPUT;
        }
        break;
        case GpioController::DIRECTION::OUTPUT:
        {
            return GPIO_MODE_OUTPUT;
        }
        break;
    }
    return GPIO_MODE_DISABLE;
}

gpio_num_t GpioController::pinNumber (uint8_t pin)
{
    switch(pin)
    {
        case 1: return GPIO_NUM_1; break;
        case 2: return GPIO_NUM_2; break;
        case 3: return GPIO_NUM_3; break;
        case 4: return GPIO_NUM_4; break;
        case 5: return GPIO_NUM_5; break;
        case 6: return GPIO_NUM_6; break;
        case 7: return GPIO_NUM_7; break;
        case 8: return GPIO_NUM_8; break;
        case 9: return GPIO_NUM_9; break;
        case 10: return GPIO_NUM_10; break;
        case 11: return GPIO_NUM_11; break;
        case 12: return GPIO_NUM_12; break;
        case 13: return GPIO_NUM_13; break;
        case 14: return GPIO_NUM_14; break;
        case 15: return GPIO_NUM_15; break;
        case 16: return GPIO_NUM_16; break;
        case 17: return GPIO_NUM_17; break;
        case 18: return GPIO_NUM_18; break;
        case 19: return GPIO_NUM_19; break;
        case 20: return GPIO_NUM_20; break;
        case 21: return GPIO_NUM_21; break;
        case 22: return GPIO_NUM_22; break;
        case 23: return GPIO_NUM_23; break;
        case 24: return GPIO_NUM_24; break;
        case 25: return GPIO_NUM_25; break;
    }
    return GPIO_NUM_NC;
}