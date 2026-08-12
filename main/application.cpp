#include <assert.h>
#include "application.h"
Application::Application()
{
   
}

void Application::init()
{
    _robot.button_init();
    _robot.imu_sensor_init();
    _robot.load_balance_point_from_flash();
}

void Application::run()
{
    _robot.balance();
}