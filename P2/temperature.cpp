#include "temperature.h"


TMP36::TMP36(PinName pin) : _pin(pin) {} //This is an initializer list … more to come in class
// _pin(pin) means pass pin to the AnalogIn constructor
float TMP36::read()
{
//convert sensor reading to temperature in degrees C
    return ((_pin.read()*3.3)-0.500)*100.0;
}