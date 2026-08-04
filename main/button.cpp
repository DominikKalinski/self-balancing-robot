
#include "button.h"
#include "sdkconfig.h"
#include "gpio-controller.h"

Button::Button() : _gpio(CONFIG_BUTTON_GPIO), _button_queue{ }
{
    gpio_config_t button_config;
    _button_queue = xQueueCreate(10, sizeof(uint8_t));
    button_config.pin_bit_mask = 1ULL << _gpio;
    button_config.mode = GPIO_MODE_INPUT;
    button_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    button_config.pull_up_en = GPIO_PULLUP_ENABLE;
    button_config.intr_type = GPIO_INTR_NEGEDGE;

    ESP_ERROR_CHECK(gpio_config(&button_config));
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    
   
}

void Button::button_isr_handler(void* arg)
{
    Button* button = static_cast<Button*>(arg);
    uint8_t gpio = button->gpio();
    BaseType_t higher_priority_task_woken = pdFALSE; //int

    xQueueSendFromISR(button->button_queue(), &gpio, &higher_priority_task_woken);

    if(higher_priority_task_woken)
    {
        portYIELD_FROM_ISR();
    }
}

void Button::button_task(void* parameter)
{
    
    Button* button = static_cast<Button*>(parameter);
    uint32_t gpio;
    int64_t last_press = 0; //esp_timer_get_time();
    while(true)
    {
        if(xQueueReceive(button->button_queue(), &gpio, portMAX_DELAY))
        {
            int64_t now = esp_timer_get_time();
            if( (now - last_press) < 50000)
            {
                continue;
            }
            last_press = now;
            printf("interrupt detected\n");
        }

    }
}

void Button::run_task()
{
    ESP_ERROR_CHECK(gpio_isr_handler_add( (gpio_num_t)_gpio, button_isr_handler, this));
    xTaskCreate(button_task, "button_task", 2048, this, 5, nullptr);
}

QueueHandle_t Button::button_queue() const
{
    return _button_queue;
}

uint8_t Button::gpio() const
{
    return _gpio;
}

