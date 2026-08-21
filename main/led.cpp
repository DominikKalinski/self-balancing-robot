#include "led.h"
#include "sdkconfig.h"


Led::Led() : _gpio(CONFIG_LED_GPIO), _led_strip(nullptr)
{
    



led_strip_config_t strip_config = {
    .strip_gpio_num = static_cast<gpio_num_t>(_gpio),
    .max_leds = 1,
};

led_strip_rmt_config_t rmt_config = {
    .resolution_hz = 10 * 1000 * 1000,
};

led_strip_new_rmt_device(&strip_config, &rmt_config, &_led_strip);
}

void Led::set_color(uint8_t red, uint8_t green, uint8_t blue)
{
    led_strip_set_pixel(_led_strip, 0, red, green, blue);
    led_strip_refresh(_led_strip);
}

void Led::turn_off()
{
    led_strip_clear(_led_strip);
}