#pragma once
#include <inttypes.h>
#include "led_strip.h"
class Led
{
    public:
    Led();
    void set_color(uint8_t, uint8_t, uint8_t);
    void turn_off();
    private:
    uint8_t _gpio;
    led_strip_handle_t _led_strip;
};