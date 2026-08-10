#include "pwm-controller.h"

#define PWM_GPIO        18
#define PWM_FREQUENCY   20000
#define PWM_RESOLUTION  LEDC_TIMER_10_BIT
#define LEDC_SPEED_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_0

PwmController::PwmController()
{
       ledc_timer_config_t timer = 
        {
        .speed_mode = LEDC_SPEED_MODE,
        .duty_resolution = PWM_RESOLUTION,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = PWM_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
        };

    ESP_ERROR_CHECK(ledc_timer_config(&timer));
}


void PwmController::pwm_pin_init(uint8_t pin, PwmController::CHANNEL CHANNEL)
{
    ledc_channel_t chnl = LEDC_CHANNEL_0;
    switch(CHANNEL)
    {
        case PwmController::CHANNEL::CHANNEL0: chnl = LEDC_CHANNEL_0; break;
        case PwmController::CHANNEL::CHANNEL1: chnl = LEDC_CHANNEL_1; break;
    }
    ledc_channel_config_t channel = {
        .gpio_num = (int)pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = chnl,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };

    ESP_ERROR_CHECK (ledc_channel_config(&channel));
}

void PwmController::pwm_set(float pwm_duty_percentage, PwmController::CHANNEL CHANNEL)
{
    int percentage = pwm_duty_percentage;
    if(pwm_duty_percentage < 0)
    {
        pwm_duty_percentage *= -1;
    }
    ledc_channel_t chnl = LEDC_CHANNEL_0;
    switch(CHANNEL)
    {
        case PwmController::CHANNEL::CHANNEL0: chnl = LEDC_CHANNEL_0; break;
        case PwmController::CHANNEL::CHANNEL1: chnl = LEDC_CHANNEL_1; break;
    }
    if(pwm_duty_percentage > 100) {pwm_duty_percentage = 100;}
    else if(pwm_duty_percentage < 0) {pwm_duty_percentage = 0;}

    int resolution = (1 << PWM_RESOLUTION) - 1;
    int duty = (int)((pwm_duty_percentage * resolution) / 100.f);
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_SPEED_MODE, chnl, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_SPEED_MODE, chnl));
}