# Self balancing robot
<img width="2048" height="1536" alt="image" src="https://github.com/user-attachments/assets/04780936-6189-4543-8727-90c1e1994af1" />

To build and flash use the command idf.py build flash

If you need to know how to set it up, follow this guide: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html

Components:
* 2 Motors 12V gear ratio 50:1 max 10000 rpm (or 200 on the shaft)
* 2 Motor drivers for controlling direction and pwm
* 1 ESP-32c6
* 1 electrical swtich with LED
* Fuse holder
* 5A fuse
* Multiple resistors (explained further down)
* 4 WAGO-lever 5 point connectors
* 1 LIPO 12V 7300 mAh battery (overkill)
* 3d-printed body (3 parts)

How to use:
* Place the robot on a flat surface as leveled as possible
* Switch the electrical switch (top left if battery XT60 connector is on the left)
* Check if robot is balancing well, if it drifts too much from the start then calibrate it**
* Enjoy

**Calibration is done by pressing the yellow button on the perfboard, once pressed a interrupt changes a flag which then enters the robot in calibration mode. The calibration starts with a 40 ms buzzer beep followed by 10 seconds delay so you have time to find the balance point then another buzzer beep, this time 200ms beeps and then the calibration have started, hold the balance point until you hear another longer beep after approximately 10 seconds (500ms). Now there will be a 7 seconds delay with a 100 ms beep in between, after that the balancing and powering the motors starts again.




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


