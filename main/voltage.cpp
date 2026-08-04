#include "voltage.h"
#include "sdkconfig.h"

Voltage::Voltage() : _gpio(CONFIG_VOLTAGE_GPIO)
{
    _init_config1.unit_id = ADC_UNIT_1;
    _init_config1.ulp_mode = ADC_ULP_MODE_DISABLE;

ESP_ERROR_CHECK(adc_oneshot_new_unit(&_init_config1, &_adc1_handle));
ESP_ERROR_CHECK(adc_oneshot_del_unit(_adc1_handle));

_config.bitwidth = ADC_BITWIDTH_12;
_config.atten = ADC_ATTEN_DB_12;

ESP_ERROR_CHECK(adc_oneshot_config_channel(_adc1_handle, ADC_CHANNEL_0, &_config));

}

float Voltage::voltage() const
{
    return 0.f;
}