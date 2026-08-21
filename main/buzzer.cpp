#include <esp_log.h>
#include <mpu6050.h>
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "buzzer.h"
#include "gpio-controller.h"
Buzzer::Buzzer() : _gpio(CONFIG_BUZZER_GPIO)
{
    GpioController::setDirection(_gpio, GpioController::DIRECTION::OUTPUT);
    GpioController::setState(_gpio, GpioController::STATE::LOW);
}

void Buzzer::beep_ms(int ms, int amount)
{
    _ms = ms;
    _amount = amount;
    xTaskCreate(beep, "mpu6050_test", 1536 * 6, this, 5, NULL);
}

int Buzzer::ms() const
{
    return _ms;
}

uint8_t Buzzer::gpio() const
{
    return _gpio;
}

void Buzzer::beep(void* parameter)
{
    Buzzer* buzzer = static_cast<Buzzer*>(parameter);
    for(int i = 0; i < buzzer->_amount; i++)
    {
        GpioController::setState( buzzer->gpio(), GpioController::STATE::HIGH);
        vTaskDelay( pdMS_TO_TICKS( buzzer->ms() ) );
        GpioController::setState( buzzer->gpio(), GpioController::STATE::LOW);
        vTaskDelay( pdMS_TO_TICKS( buzzer->ms() ) );
    }
    
    vTaskDelete(NULL);
}