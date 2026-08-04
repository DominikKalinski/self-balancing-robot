#pragma once
#include <inttypes.h>
#include "pwm-controller.h"
#include "pcnt-controller.h"
class Motor
{
    public:
    
    Motor(uint8_t, uint8_t, uint8_t, uint8_t, PwmController::CHANNEL);
    void switch_dir();
    void set_pwm(uint8_t pwm);
    uint8_t pwm_pin();
    void start_rpm_task();
    PcntController& pcntController();
    double& rpm();
    int64_t& previous_time_us();
    int encoder_a_pin() const;
    
    private:
    static void update_rpm_task_A(void*);
    PwmController _pwmController;
    bool _forward;
    uint8_t _dir_pin;
    uint8_t _pwm_pin;
    uint8_t _encoder_A_pin;
    uint8_t _encoder_B_pin;
    double _rpm;
    PwmController::CHANNEL _channel;
    PcntController _pcntController;
    int64_t _previous_time_us;
};