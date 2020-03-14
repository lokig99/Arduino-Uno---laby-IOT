#include "Basic_LED_PWM.h"

LED_PWM::LED_PWM(byte ledPin)
{
    pin = ledPin;
    pinMode(pin, OUTPUT);
    reset();
}

void LED_PWM::on() { analogWrite(pin, brightness); }

void LED_PWM::off() { analogWrite(pin, LOW); }

void LED_PWM::blink(int delayMillis)
{
    fix_time();
    if (millis() - timeOfLastBlink > delayMillis)
    {
        state = !state;
        if (state)
            analogWrite(pin, MAX_BRIGHTNESS);
        else
            analogWrite(pin, LOW);

        timeOfLastBlink = millis();
    }
}

void LED_PWM::reset()
{
    state = digitalRead(pin);
    brightness = MAX_BRIGHTNESS;
}

byte LED_PWM::getBrightness() { return brightness; }

void LED_PWM::setBrightness(byte newBrightness) { brightness = newBrightness; on(); }

void LED_PWM::fix_time()
{
    //fix time of last blink when arduino time clock overflows
    if (millis() < timeOfLastBlink)
        timeOfLastBlink = ULONG_MAX - timeOfLastBlink;
}