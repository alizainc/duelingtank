#include "mbed.h"

class TMP36
{
public:
    TMP36(PinName pin);
    TMP36();
    float read();
private:
//class sets up the AnalogIn pin
    AnalogIn _pin;
};