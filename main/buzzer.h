#pragma once
#include <inttypes.h>
class Buzzer
{
    public:
    Buzzer();
    void beep_ms(int);
    int ms() const;
    uint8_t gpio() const;
    private:
    static void beep(void*);
    uint8_t _gpio;
    int _ms;
};