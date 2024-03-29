// Imię i Nazwisko: Mateusz Groblicki
// Grupa: Czwartek 11:15
// Zadanie: 1.3. Sterowanie LED z terminala

#include "Basic_LED_PWM.h"

#define LED_OFF 0
#define LED_ON 1
#define LED_BLINK 2
const String COMMAND_LED_ON = "LEDON";
const String COMMAND_LED_OFF = "LEDOFF";
const String COMMAND_LED_BLINK = "LEDBLINK";

LED_PWM led(3);
byte ledStatus = LED_OFF;
int ledBlinkRate = 1000;
String command = "";

void setup()
{
    Serial.begin(9600);
    Serial.println("Arduino is ready!");
}

void loop()
{
    if (ledStatus == LED_BLINK)
        led.blink(ledBlinkRate);

    checkForCommands();
}

void checkForCommands()
{
    if (Serial.available() > 0)
    {
        command = Serial.readStringUntil('\n');
        command.replace(" ", "");
        command.toUpperCase();

        if (command.equals(COMMAND_LED_OFF))
        {
            led.off();
            Serial.println("LED status - off");
            ledStatus = LED_OFF;
        }
        else if (command.equals(COMMAND_LED_ON))
        {
            led.on();
            Serial.println("LED status - on");
            ledStatus = LED_ON;
        }
        //check for "LED BLINK"
        else if (command.indexOf(COMMAND_LED_BLINK) == 0)
        {
            if (command.equals(COMMAND_LED_BLINK))
            {
                Serial.println("LED status - blinking at previous rate of: " + String(ledBlinkRate));
                ledStatus = LED_BLINK;
            }
            else
            {
                int blinkRate = command.substring(COMMAND_LED_BLINK.length()).toInt();
                if (blinkRate <= 0)
                    Serial.println("Error - invalid blink rate: " + String(blinkRate));
                else
                {
                    ledBlinkRate = blinkRate;
                    Serial.println("LED status - blinking at rate of: " + String(ledBlinkRate));
                    ledStatus = LED_BLINK;
                }
            }
        }
        else
            Serial.println("Error - Unknown command: " + command);
    }
}