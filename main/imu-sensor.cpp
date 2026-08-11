#include <esp_log.h>
#include <cmath>

#include "imu-sensor.h"
#include "macros.h"
static const char *TAG = "mpu6050_test";

ImuSensor::ImuSensor() : _task_handle(nullptr), _previous_time(0)
{

}



void ImuSensor::init()
{
    _dev = { };
    ESP_ERROR_CHECK(i2cdev_init());
    ESP_ERROR_CHECK( mpu6050_init_desc(&_dev, ADDR, (i2c_port_t)0, (gpio_num_t)(CONFIG_EXAMPLE_SDA_GPIO), (gpio_num_t)CONFIG_EXAMPLE_SCL_GPIO) );
    ESP_LOGI(TAG, "port=%d admpu6050_dev_t _dev;dr=0x%02X", _dev.i2c_dev.port, _dev.i2c_dev.addr);
    ESP_ERROR_CHECK(mpu6050_init(&_dev));
}


void ImuSensor::metrics(mpu6050_acceleration_t* accel, mpu6050_rotation_t* rotation)
{
    ESP_ERROR_CHECK(mpu6050_get_motion(&_dev, accel, rotation));
}



//PRIVATE//     //PRIVATE       //PRIVATE       //PRIVATE       //PRIVATE
