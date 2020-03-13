// Imię i Nazwisko: Mateusz Groblicki
// Grupa: Czwartek 11:15
// Zadanie: 1.2. Czas wciśnięcia przycisku

#include "Basic_Button.h"

const long SECOND = 1000;
const long MINUTE = 60*SECOND;
const long HOUR = 60*MINUTE;

Button buttonGreen(4);

void setup()
{
    Serial.begin(9600);
    Serial.println("Arduino is ready!");
    pinMode(LED_PIN, OUTPUT);
}

void loop()
{
    if(buttonGreen.wasPushed())
    {
        unsigned long time_start = millis();

        while(buttonGreen.isPressed()){}

        unsigned long time_end = millis() - time_start;

        Serial.print("button pressed for: " + String(time_end) + " ms -> ");
        printClockTime(time_end);
        Serial.println();
    }   
}

void printClockTime(unsigned long time_ms)
{
    unsigned long ms_left = time_ms;
    int hours = time_ms / HOUR;
    ms_left -= hours * HOUR;
    int minutes = ms_left / MINUTE;
    ms_left -= minutes * MINUTE;
    int seconds = ms_left / SECOND;
    ms_left -= seconds * SECOND;

    Serial.print(String(hours) + ":" + ((minutes < 10)?"0":"") 
                    + String(minutes) + ":" + ((seconds < 10)?"0":"") + String(seconds)
                    + "." + String(ms_left));
}
