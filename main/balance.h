#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
class ImuSensor;
class Balance
{
    public:
    private:
    ImuSensor* _imuSensor;
};