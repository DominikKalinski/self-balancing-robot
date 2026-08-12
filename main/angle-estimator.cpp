#include <cmath>
#include "angle-estimator.h"
#include "esp_timer.h"
AngleEstimator::AngleEstimator(ImuSensor* imuSensor) : _integral(0.f), _average_acceleration_y_axis(0), _average_acceleration_z_axis(0),
_average_rotation_x_axis(0), _corrected_angle_x(0), _corrected_rotation_x(0), _previous_time(0), _imuSensor(imuSensor), _flashStorage(FlashStorage::instance())
{
     _buzzer.beep_ms(200);
}

void AngleEstimator::load_balance_points_from_flash()
{
    _average_acceleration_y_axis = _flashStorage.load_from_flash("acceleration_y");
    _average_acceleration_z_axis = _flashStorage.load_from_flash("acceleration_z");
    _average_rotation_x_axis = _flashStorage.load_from_flash("rotation_x");
}

float AngleEstimator::PID(float P, float I, float D)
{
    update_angles();
    int64_t time_now = esp_timer_get_time();
    float delta_time = static_cast<float>((time_now - _previous_time ) / 1000000.f);
    float error = _corrected_angle_x;
    float rotation = _corrected_rotation_x;

    if(error < 0.3f && error > -0.3f){ D = 0.0f; }
        
    float output = (P * error) + (I * (_integral += error * delta_time)) + (D * rotation);

    _previous_time = time_now;
    return output;
}

void AngleEstimator::update_angles()
{
    mpu6050_acceleration_t accel = {  };
    mpu6050_rotation_t rotation = {  };
    _imuSensor->metrics(&accel, &rotation);
    int64_t time_now = esp_timer_get_time();
    

    _corrected_rotation_x = rotation.x - _average_rotation_x_axis;
    float corrected_acceleration_y = accel.y - _average_acceleration_y_axis;
    float corrected_acceleration_z = accel.z + (1 -_average_acceleration_z_axis);
    float delta_time_seconds = (time_now - _previous_time) / 1000000.f;
    _previous_time = time_now;
    float angle_gyroscope_x = (delta_time_seconds * _corrected_rotation_x);
    float angle_accelerator_x = ( (atan2f(corrected_acceleration_y, corrected_acceleration_z) * 180) / _PI);
    
    _corrected_angle_x = 0.98f * (_corrected_angle_x + angle_gyroscope_x) + 0.02f * angle_accelerator_x;
}

void AngleEstimator::calibrate_balance_point()
{
    _buzzer.beep_ms(40);
    vTaskDelay(pdMS_TO_TICKS(10000));
    _buzzer.beep_ms(200);
    mpu6050_acceleration_t accel;
    mpu6050_rotation_t rotation;
    float total_acceleration_y_axis = 0.f;
    float total_acceleration_z_axis = 0.f;
    float total_rotation_x_axis = 0.f;
    for(int i = 0; i < 1000; i++)
    {
        _imuSensor->metrics(&accel, &rotation);
        total_acceleration_y_axis += accel.y;
        total_acceleration_z_axis += accel.z;
        total_rotation_x_axis += rotation.x;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    _average_acceleration_y_axis = total_acceleration_y_axis / 1000.f;
    _average_acceleration_z_axis = total_acceleration_z_axis / 1000.f;
    _average_rotation_x_axis = total_rotation_x_axis / 1000.f;
    _buzzer.beep_ms(500);
    _flashStorage.save_to_flash("acceleration_y", _average_acceleration_y_axis);
    _flashStorage.save_to_flash("acceleration_z", _average_acceleration_z_axis);
    _flashStorage.save_to_flash("rotation_x", _average_rotation_x_axis);
}