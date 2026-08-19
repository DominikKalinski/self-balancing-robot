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
    void set_pwm(float pwm);
    uint8_t pwm_pin();
    PcntController& pcntController();
    float rpm() const;
    int64_t& previous_time_us();
    int encoder_a_pin() const;
    void start_rpm_task();
    static void update_rpm(void*);
    
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
    int64_t _previous_time;
    uint8_t _motor_number;
    int _previous_pulses;
    static uint8_t _motor_count;
};

