# Self balancing robot
Attempt of creating a self balancing robot

## Classes:

- **Balance** -> will take a ImuSensor pointer to get angle readings and handle the physical balancing
- **Button** -> handles button presses and interrupt when it button is pressed
- **Buzzer** -> handles a non-blocking buzz task that beeps for required amount of time
- **FlashStorage** -> Stores data to non-volatile memory
- **GpioController** -> handles setting GPIO pins to input/output and states to high/low
- **ImuSensor** -> handles sensor readings from the mpu6050 (IMU) sensor and calibration of a average value when button pressed, stores calibrated value in flash
- **Motor** -> Handles both motors, switches direction, sets pwm, counts rpm
- **PcntController** -> Handles hardware counting of rising edges (motor class uses this calss for counting rpm through sending impulses through its hall sensors)
- **PwmController** -> Motor class uses this to set pwm for motors
- **Robot** -> Will control everything, motors, sensor, button, buzzer and check voltage (voltage not yet implemented)
- **Voltage** -> Will handle reading of voltage


