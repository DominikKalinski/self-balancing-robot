
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "imu-sensor.h"
#include "robot.h"
#include "pcnt-controller.h"
#include "buzzer.h"
#include "button.h"
#include "flash-storage.h"
extern "C" void app_main()
{
    static Robot robot;
    robot.start_balance_task();
}
