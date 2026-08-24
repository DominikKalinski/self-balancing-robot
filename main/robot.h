#pragma once
#include <atomic>
#include "motor.h"
#include "pwm-controller.h"
#include "button.h"
#include "angle-estimator.h"
#include "led.h"
class Robot
{
    public:
    enum class DIRECTION
    {
        FORWARD,
        REVERSE,
        NONE
    };
    Robot();
    void imu_sensor_init();
    void button_init();
    void start_auto_calibration_task();
    void load_balance_point_from_flash();
    void balance();
    void move(Robot::DIRECTION);
    void choose_direction();
    void nullifier(float*, float*, float*);
    
    
    private:
    
    void set_motor_pwm(Motor&, float);
    void PID(float, float, float);
    static void auto_calibrate(void*);
    float brake(float);

    PwmController _pwmController;
    Motor _motor1;
    Motor _motor2;
    Button _button;
    ImuSensor _imuSensor;
    AngleEstimator _angleEstimator;
    float _output;
    float _previous_error;
    float _integral;
    float _braking_integral;
    int64_t _time_now;
    int64_t _previous_time;
    int64_t _previous_time_calibrate_task;
    std::atomic<float> _angle_goal;
    std::atomic<float> _angle_goal_braking_offset;
    float _previous_pulses_per_second;
    int _counter;
    Robot::DIRECTION _direction;
    bool _is_being_controlled;
    bool _is_braking;
    bool _brake_timer_set;
    int _previous_pulses;
    Buzzer _buzzer;
    Led _led;
  

};