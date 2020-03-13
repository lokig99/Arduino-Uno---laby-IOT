// Imię i Nazwisko: Mateusz Groblicki
// Grupa: Czwartek 11:15
// Zadanie: 1.1. Licznik na dwa przyciski

#include "Basic_Button.h"
const int led_delay = 500;
const int button_delay = 20;
int counter = 0;

Button buttonRed(2), buttonGreen(4);

void setup()
{
    Serial.begin(9600);
    Serial.println("Arduino is ready!");
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
    if(buttonRed.isPressed() && buttonGreen.isPressed())
    {
        Serial.println("Both buttons pushed, counter = " + String(counter));

        for(int i = 0; i < counter; ++i)
        {
            Serial.println("blink " + String(i));
            digitalWrite(LED_BUILTIN, HIGH);
            delay(led_delay);
            digitalWrite(LED_BUILTIN, LOW);
            delay(led_delay);
        }

        buttonRed.reset(); buttonGreen.reset();
    }
    else if(buttonGreen.wasReleased())
        Serial.println("counter increased -> " + String(++counter));
    else if(buttonRed.wasReleased())
        Serial.println("counter decreased -> " + String(--counter));
}
