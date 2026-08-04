
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "imu-sensor.h"
#include "robot.h"
#include "pcnt-controller.h"
#include "buzzer.h"
#include "button.h"
extern "C" void app_main()
{
    static ImuSensor sensor;
    sensor.init();
    sensor.start_test();
    Robot robot;
    robot.start_rpm_task();
    Button button(&robot);
    button.run_task();
    
    while(true)
    {
        for(int i = 0; i <= 70; i+=1)
        {
            vTaskDelay(pdMS_TO_TICKS(100));
             robot.set_motor_pwm(robot.motor1(), i);
             robot.set_motor_pwm(robot.motor2(), i);
          
    //        printf(
    // "\033[H"
    // "RPM1: %10.2f\033[K\n"
    // "RPM2: %10.2f\033[K\n",
    // robot.motor1().rpm(),
    // robot.motor2().rpm()
//);
        }
        for(int i = 70; i >= 0; i--)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
            robot.set_motor_pwm(robot.motor1(), i);
            robot.set_motor_pwm(robot.motor2(), i);
    //         printf(
    // "\033[H"
    // "RPM1: %10.2f\033[K\n"
    // "RPM2: %10.2f\033[K\n",
    // robot.motor1().rpm(),
    // robot.motor2().rpm()
//);
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
        
        robot.switch_direction(robot.motor1());
        robot.switch_direction(robot.motor2());
        
    }
}
