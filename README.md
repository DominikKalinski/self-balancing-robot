# Self balancing robot


## Classes:
- **AngleEstimator** -> Does PID, updates angles and estimates correct angle based on different inputs and stored calibrated balance point
- **Application** -> Does initializations, owns robot and starts balancing loop
- **Button** -> handles button presses and interrupt when it button is pressed
- **Buzzer** -> handles a non-blocking buzz task that beeps for required amount of time
- **FlashStorage** -> Stores data to non-volatile memory
- **GpioController** -> handles setting GPIO pins to input/output and states to high/low
- **ImuSensor** -> handles sensor readings from the mpu6050 (IMU) sensor and calibration of a average value when button pressed, stores calibrated value in flash
- **Led** -> Setups built in RGB LED on the esp32, then handles on/off and colors
- **Motor** -> Handles both motors, switches direction, sets pwm, counts rpm
- **PcntController** -> Handles hardware counting of rising and falling edges, (motor class uses this calss for counting pulses per second through sending impulses through its hall sensors)
- **PwmController** -> Motor class uses this to set pwm for motors
- **Robot** -> Initializes IMU-sensor, button ISR, starts autocalibration task and runs main loop for PID based balancing
- **Voltage** -> Not yet implemented - will handle voltage readings.


