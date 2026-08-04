#pragma once

#include "driver/gpio.h"

class GpioController
{
    public:
    enum class DIRECTION
    {
        INPUT,
        OUTPUT
    };
    enum class STATE
    {
        LOW,
        HIGH
    };

    static void setDirection(uint8_t, GpioController::DIRECTION);
    static void setState(uint8_t, GpioController::STATE);
    static gpio_num_t pinNumber(uint8_t);
    static gpio_mode_t gpioDirection(GpioController::DIRECTION);
    private:
};