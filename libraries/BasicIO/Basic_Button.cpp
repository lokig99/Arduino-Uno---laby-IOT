#include "Basic_Button.h"

Button::Button(byte buttonPin)
{
    pin = buttonPin;
    pinMode(pin, INPUT_PULLUP);
    reset();
}

bool Button::hasStateChanged()
{
    if (digitalRead(pin) != state)
    {
        //check for button vibrations
        if (update)
        {
            fix_time();
            if (millis() - timeOfLastUpdate > DELAY)
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
    return !update && digitalRead(pin) == LOW;
}

void Button::fix_time()
{
    //fix time of last update when arduino time clock overflows
    if (millis() < timeOfLastUpdate)
        timeOfLastUpdate = ULONG_MAX - timeOfLastUpdate;
}

//set button.update status to FALSE and button.state to HIGH
void Button::reset()
{
    update = false;
    state = HIGH;
}
