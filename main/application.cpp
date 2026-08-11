#include <assert.h>
#include "application.h"
Application::Application()
{
   
}

void Application::init()
{
    _robot.button_init();
    _robot.imu_sensor_init();
}

void Application::run()
{
    _robot.balance();
}