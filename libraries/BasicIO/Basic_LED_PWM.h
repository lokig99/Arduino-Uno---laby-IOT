#ifndef BASIC_LED_PWM_H
#define BASIC_LED_PWM_H

#include <Arduino.h>
#include <limits.h>

#define MAX_BRIGHTNESS 255

class LED_PWM
{
private:
    byte pin;
    byte brightness;
    bool state;
    unsigned long timeOfLastBlink;

    void fix_time();

public:
    LED_PWM(byte ledPin);
    LED_PWM();
    void on();
    void off();
    void blink(int delayMillis);
    void setBrightness(byte newBrightness);
    byte getBrightness();
    void reset();
};

#endif