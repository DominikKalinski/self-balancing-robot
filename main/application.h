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
    int64_t _previous_time;
};