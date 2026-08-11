
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "application.h"

extern "C" void app_main()
{
   Application app;
   app.init();
   app.run();
}
