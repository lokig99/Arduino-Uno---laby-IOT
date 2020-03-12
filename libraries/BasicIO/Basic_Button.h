#ifndef BASIC_BUTTON_H
#define BASIC_BUTTON_H
#define ULONG_MAX powl(2, 32) - 1;

#include <Arduino.h>

//button class
class Button
{
private:
    const byte DELAY = 30;
    byte pin;
    bool state;
    bool update;
    unsigned long timeOfLastUpdate;

    void fix_time();

public:
    Button(byte buttonPin, bool buttonState);
    Button(byte buttonPin);

    bool hasStateChanged();
    bool wasReleased();
    bool wasPushed();
    bool isPressed();
};

#endif