idf_component_register(SRCS "application.cpp" "angle-estimator.cpp" "flash-storage.cpp" "button.cpp" "buzzer.cpp" "pcnt-controller.cpp" "main.cpp" "gpio-controller.cpp" "motor.cpp" "pwm-controller.cpp" "imu-sensor.cpp" "robot.cpp"
"voltage.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES mpu6050 esp_driver_ledc esp_driver_pcnt esp_timer esp_adc nvs_flash
                    )
