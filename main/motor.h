#pragma once
#include <inttypes.h>
#include <atomic>
#include "pwm-controller.h"
#include "pcnt-controller.h"


class Motor
{
    public:
    enum class DIRECTION
    {
        FORWARD,
        REVERSE
    };
  
    Motor(uint8_t, uint8_t, uint8_t, uint8_t, PwmController::CHANNEL);
    void set_dir(Motor::DIRECTION);
    void switch_dir();
    Motor::DIRECTION direction();
    void set_pwm(float pwm);
    float pulses_per_second();
    uint8_t pwm_pin();
    PcntController& pcntController();
    float rpm() const;
    int64_t& previous_time_us();
    int encoder_a_pin() const;
    void store_timestamps_for_rotation_speed(int64_t, DIRECTION);
    
    private:
    PwmController _pwmController;
    bool _forward;
    uint8_t _dir_pin;
    uint8_t _pwm_pin;
    uint8_t _encoder_A_pin;
    uint8_t _encoder_B_pin;
    std::atomic<float> _rpm;
    PwmController::CHANNEL _channel;
    PcntController _pcntController;
    int64_t _timestamp1;
    int64_t _timestamp2;
    bool _write_to_timestamp1;
    uint8_t _motor_number;
    static uint8_t _motor_count;
    DIRECTION _direction;
};

