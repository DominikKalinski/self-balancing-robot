#include "pcnt-controller.h"
#include "driver/gpio.h"
#include "gpio-controller.h"
#include "motor.h"
#include "esp_timer.h"
PcntController::PcntController(uint8_t pinA, uint8_t pinB, Motor* motor) : _pinA(pinA), _pinB(pinB), _pcnt_unit(nullptr), _channel_A(nullptr),
_channel_B(nullptr), _motor(motor)
{
    ESP_ERROR_CHECK(gpio_set_pull_mode(static_cast<gpio_num_t>(_pinB), GPIO_PULLDOWN_ONLY));
    ESP_ERROR_CHECK(gpio_set_pull_mode(static_cast<gpio_num_t>(_pinA), GPIO_PULLDOWN_ONLY));
    GpioController::setDirection(pinA, GpioController::DIRECTION::INPUT);
    GpioController::setDirection(pinB, GpioController::DIRECTION::INPUT);
    pcnt_unit_config_t unit_config = {};

        unit_config.low_limit = -30000;
        unit_config.high_limit = 30000;
        unit_config.flags.accum_count = 1;
        unit_config.intr_priority = 0;
    
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &_pcnt_unit));
    // ESP_ERROR_CHECK(pcnt_unit_add_watch_point(_pcnt_unit, 30000));
    // ESP_ERROR_CHECK(pcnt_unit_add_watch_point(_pcnt_unit, -30000));

    pcnt_chan_config_t channel_config1 = {};
    channel_config1.edge_gpio_num = _pinA;
    channel_config1.level_gpio_num = _pinB;

    pcnt_chan_config_t channel_config2 = {};
    channel_config2.edge_gpio_num = _pinB;
    channel_config2.level_gpio_num = _pinA;

    
    ESP_ERROR_CHECK(
        pcnt_new_channel(
            _pcnt_unit,
            &channel_config1,
            &_channel_A
        )
    );

     ESP_ERROR_CHECK( 
        pcnt_new_channel(
            _pcnt_unit,
            &channel_config2,
            &_channel_B
        )
    );

  

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(
            _channel_A,
            PCNT_CHANNEL_EDGE_ACTION_INCREASE,
            PCNT_CHANNEL_EDGE_ACTION_DECREASE
        )
    );
ESP_ERROR_CHECK(pcnt_channel_set_level_action(_channel_A, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE ));

     ESP_ERROR_CHECK(pcnt_channel_set_edge_action(
            _channel_B,
            PCNT_CHANNEL_EDGE_ACTION_DECREASE,
            PCNT_CHANNEL_EDGE_ACTION_INCREASE
        )
    );

    
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(_channel_B, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE ));

    ESP_ERROR_CHECK(pcnt_unit_enable(_pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(_pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(_pcnt_unit));

    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(_pcnt_unit, 32));
    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(_pcnt_unit, -32));

    pcnt_event_callbacks_t callbacks = {
    .on_reach = pcnt_callback
};

pcnt_unit_register_event_callbacks(_pcnt_unit, &callbacks, this);
}

bool PcntController::pcnt_callback(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx)
{
    int64_t time_now = esp_timer_get_time();
    PcntController* controller = static_cast<PcntController*>(user_ctx);
    if(edata->watch_point_value == 32)
    {
        controller->_motor->store_timestamps_for_rotation_speed(time_now, Motor::DIRECTION::FORWARD);
    }
    if(edata->watch_point_value == -32)
    {
        controller->_motor->store_timestamps_for_rotation_speed(time_now, Motor::DIRECTION::REVERSE);
    }
    controller->clear_pulses();
    return false;
}

int PcntController::pulses() const
{
    int pulse_count = 0;

    ESP_ERROR_CHECK( pcnt_unit_get_count(_pcnt_unit, &pulse_count) );
   
    return pulse_count;
}

void PcntController::clear_pulses()
{
    ESP_ERROR_CHECK(pcnt_unit_clear_count(_pcnt_unit));
}