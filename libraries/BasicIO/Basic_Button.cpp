#include "Basic_Button.h"

Button::Button(byte buttonPin, bool buttonState)
{
    pin = buttonPin;
    state = buttonState;
    update = false;
    timeOfLastUpdate = millis();
}

Button::Button(byte buttonPin)
{
    pin = buttonPin;
    state = HIGH;
    update = false;
    timeOfLastUpdate = millis();
}

bool Button::hasStateChanged()
{
    if (digitalRead(pin) != state)
    {
        //check for button vibrations
        if (update)
        {
            fix_time();
            if (millis() - timeOfLastUpdate < DELAY)
            {
                state = !state;
                update = false;
                return true;
            }
        }
        else
        {
            update = true;
            timeOfLastUpdate = millis();
        }
    }

    return false;
}

bool Button::wasPushed()
{
    return hasStateChanged() && state == LOW;
}

bool Button::wasReleased()
{
    return hasStateChanged() && state == HIGH;
}

bool Button::isPressed()
{
    return !hasStateChanged() && state == LOW;
}

void Button::fix_time()
{
    //fix time of last update when arduino time clock overflows
    if (millis() < timeOfLastUpdate)
        timeOfLastUpdate = ULONG_MAX - timeOfLastUpdate;
}
