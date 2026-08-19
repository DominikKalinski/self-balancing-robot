#pragma once
#include "driver/pulse_cnt.h"
#include "esp_err.h"
class PcntController
{
    public:
    PcntController(uint8_t, uint8_t);
    int pulses() const;
    void clear_pulses();
    private:
    uint8_t _pinA;
    uint8_t _pinB;
    pcnt_unit_handle_t _pcnt_unit;
    pcnt_channel_handle_t _channel_A;
    pcnt_channel_handle_t _channel_B;
};