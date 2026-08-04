#include <esp_log.h>
#include "imu-sensor.h"
#include "macros.h"
static const char *TAG = "mpu6050_test";

ImuSensor::ImuSensor() : _dev{  }, _buzzer(), _average_acceleration_y_axis(0), _average_rotation_y_axis(0)
{

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
        printf("\033[2J\033[H");
        ESP_ERROR_CHECK(mpu6050_get_temperature(&self->_dev, &temp));
        ESP_ERROR_CHECK(mpu6050_get_motion(&self->_dev, &accel, &rotation));

        ESP_LOGI(TAG, "**********************************************************************");
        ESP_LOGI(TAG, "Acceleration: x=%.4f   y=%.4f   z=%.4f", accel.x, accel.y, accel.z);
        ESP_LOGI(TAG, "Rotation:     x=%.4f   y=%.4f   z=%.4f", rotation.x, rotation.y, rotation.z);
        ESP_LOGI(TAG, "Temperature:  %.1f", temp);

        vTaskDelay(pdMS_TO_TICKS(100));
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
    xTaskCreate(mpu6050_test, "mpu6050_test", configMINIMAL_STACK_SIZE * 6, this, 5, NULL);
}


void ImuSensor::calibrate()
{
    _buzzer.beep_ms(40);
    mpu6050_acceleration_t accel;
    mpu6050_rotation_t rotation;
    float total_acceleration_y_axis = 0.f;
    float total_rotation_y_axis = 0.f;
    for(int i = 0; i < 100; i++)
    {
        ESP_ERROR_CHECK(mpu6050_get_motion(&_dev, &accel, &rotation));
        total_acceleration_y_axis += accel.y;
        total_rotation_y_axis += rotation.y;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    _average_acceleration_y_axis = total_acceleration_y_axis / 1000;
    _average_rotation_y_axis = total_rotation_y_axis / 1000;
    printf("Average acceleration: %f\n Average Rotation: %f\n", _average_acceleration_y_axis, _average_rotation_y_axis);
    _buzzer.beep_ms(500);
}



//PRIVATE//     //PRIVATE       //PRIVATE       //PRIVATE       //PRIVATE
