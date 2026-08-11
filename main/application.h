#pragma once
#include <inttypes.h>
#include "robot.h"

class Application
{
    public:
    Application();
    void init();
    void run();
    private:
    Robot _robot;
};