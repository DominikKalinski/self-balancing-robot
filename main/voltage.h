#pragma once
#include <inttypes.h>
#include "esp_adc/adc_oneshot.h"
class Voltage
{
    public:
    Voltage();
    float voltage() const;
    private:
    uint8_t _gpio;
    adc_oneshot_unit_handle_t _adc1_handle;
    adc_oneshot_unit_init_cfg_t _init_config1;
    adc_oneshot_chan_cfg_t _config;
};