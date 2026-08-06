#include <esp_log.h>
#include <cmath>
#include "esp_timer.h"
#include "imu-sensor.h"
#include "macros.h"
static const char *TAG = "mpu6050_test";

ImuSensor::ImuSensor() : _dev{  }, _buzzer(), _average_acceleration_y_axis(0), _average_acceleration_z_axis(0),
 _average_rotation_y_axis(0), _task_handle(nullptr), _flashStorage(FlashStorage::instance()), _previous_time(0)
{
    _imu_mutex = xSemaphoreCreateMutex();
    configASSERT(_imu_mutex != nullptr);
    _average_acceleration_y_axis = _flashStorage.load_from_flash("acceleration_y");
    _average_acceleration_z_axis = _flashStorage.load_from_flash("acceleration_z");
    _average_rotation_y_axis = _flashStorage.load_from_flash("rotation");
}

void ImuSensor::mpu6050_test(void *pvParameters)
{
    ImuSensor* self = static_cast<ImuSensor*>(pvParameters); 
    while (1)
    {
        esp_err_t res = i2c_dev_probe(&self->_dev.i2c_dev, I2C_DEV_WRITE);
        if (res == ESP_OK)
        {
            ESP_LOGI(TAG, "Found MPU60x0 device");
            break;
        }
        ESP_LOGE(TAG, "MPU60x0 not found");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    ESP_ERROR_CHECK(mpu6050_init(&self->_dev));

    ESP_LOGI(TAG, "Accel range: %d", self->_dev.ranges.accel);
    ESP_LOGI(TAG, "Gyro range:  %d", self->_dev.ranges.gyro);

    while (1)
    {
        
        float temp;
        mpu6050_acceleration_t accel = { 0 };
        mpu6050_rotation_t rotation = { 0 };
       
        xSemaphoreTake(self->_imu_mutex, portMAX_DELAY);
        ESP_ERROR_CHECK(mpu6050_get_temperature(&self->_dev, &temp));
        ESP_ERROR_CHECK(mpu6050_get_motion(&self->_dev, &accel, &rotation));
        xSemaphoreGive(self->_imu_mutex);

         printf("\033[2J\033[H");
        ESP_LOGI(TAG, "**********************************************************************");
        ESP_LOGI(TAG, "Acceleration: x=%.4f   y=%.4f   z=%.4f", accel.x, accel.y, accel.z);
        ESP_LOGI(TAG, "Rotation:     x=%.4f   y=%.4f   z=%.4f", rotation.x, rotation.y, rotation.z);
        ESP_LOGI(TAG, "Temperature:  %.1f", temp);
        
        
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void ImuSensor::init()
{
    _dev = { };
    ESP_ERROR_CHECK(i2cdev_init());
    ESP_ERROR_CHECK( mpu6050_init_desc(&_dev, ADDR, (i2c_port_t)0, (gpio_num_t)(CONFIG_EXAMPLE_SDA_GPIO), (gpio_num_t)CONFIG_EXAMPLE_SCL_GPIO) );
    ESP_LOGI(TAG, "port=%d addr=0x%02X", _dev.i2c_dev.port, _dev.i2c_dev.addr);
}

void ImuSensor::start_test()
{
    int result = xTaskCreate(mpu6050_test, "mpu6050_test", configMINIMAL_STACK_SIZE * 6, this, 5, &_task_handle);
    print_averages();

    configASSERT(result == pdPASS);
    configASSERT(_task_handle != nullptr);
}


void ImuSensor::calibrate()
{
    print_averages();
    xSemaphoreTake(_imu_mutex, portMAX_DELAY);
    _buzzer.beep_ms(40);
    vTaskDelay(pdMS_TO_TICKS(10000));
    _buzzer.beep_ms(200);
    //vTaskSuspend(_task_handle);
    mpu6050_acceleration_t accel;
    mpu6050_rotation_t rotation;
    float total_acceleration_y_axis = 0.f;
    float total_acceleration_z_axis = 0.f;
    float total_rotation_y_axis = 0.f;
    for(int i = 0; i < 1000; i++)
    {
        ESP_ERROR_CHECK(mpu6050_get_motion(&_dev, &accel, &rotation));
        total_acceleration_y_axis += accel.y;
        total_acceleration_z_axis += accel.z;
        total_rotation_y_axis += rotation.y;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    _average_acceleration_y_axis = total_acceleration_y_axis / 1000.f;
    _average_acceleration_z_axis = total_acceleration_z_axis / 1000.f;
    _average_rotation_y_axis = total_rotation_y_axis / 1000.f;
     xSemaphoreGive(_imu_mutex);
    _buzzer.beep_ms(500);
    //vTaskResume(_task_handle);
    _flashStorage.save_to_flash("acceleration_y", _average_acceleration_y_axis);
    _flashStorage.save_to_flash("acceleration_z", _average_acceleration_z_axis);
    _flashStorage.save_to_flash("rotation", _average_rotation_y_axis);
    print_averages();
}

void ImuSensor::update_angles()
{
    mpu6050_acceleration_t accel = { 0 };
    mpu6050_rotation_t rotation = { 0 };
    
    xSemaphoreTake(_imu_mutex, portMAX_DELAY);
    ESP_ERROR_CHECK(mpu6050_get_motion(&_dev, &accel, &rotation));
    xSemaphoreGive(_imu_mutex);
    int64_t time_now = esp_timer_get_time();

    float corrected_rotation_y = rotation.y - _average_rotation_y_axis;
    float corrected_acceleration_y = accel.y - _average_acceleration_y_axis;
    float corrected_acceleration_z = accel.z - _average_acceleration_z_axis;
    
    float delta_time_seconds = (time_now - _previous_time) / 1000000.f;
    _previous_time = time_now;
    _angle_gyroscope_y += (delta_time_seconds * corrected_rotation_y);
    _angle_accelerator_y = ( (atan2f(corrected_rotation_y, corrected_acceleration_z) * 180) / _PI);
}


void ImuSensor::print_averages() const
{
    printf("Average acceleration: %f\nAverage rotation: %f\nAverage acceleration Z: %f", 
        _average_acceleration_y_axis, _average_rotation_y_axis, _average_acceleration_z_axis);
}

float ImuSensor::angle_accelerator_y() const
{
    return _angle_accelerator_y;
}

float ImuSensor::angle_gyroscope_y() const
{
    return _angle_gyroscope_y;
}


//PRIVATE//     //PRIVATE       //PRIVATE       //PRIVATE       //PRIVATE
