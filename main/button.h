#pragma once
#include <inttypes.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_timer.h"


class Button
{
    public:
    Button();
    static void IRAM_ATTR button_isr_handler(void*);
    static void button_task(void*);
    void run_task();
    QueueHandle_t button_queue() const;
    uint8_t gpio() const;
    private:
    uint8_t _gpio;
    QueueHandle_t _button_queue;
};