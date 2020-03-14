// Imię i Nazwisko: Mateusz Groblicki
// Grupa: Czwartek 11:15
// Zadanie: 3.1. Miganie LED ze ściemniaczem

#include "Basic_Button.h"
#include "Basic_LED_PWM.h"
#include "CyclicBuffer.h"

#define OFFSET 10
#define POTENTIOMETER A0

Button buttonRed(2), buttonGreen(4);
LED_PWM redLED(6), greenLED(5), blueLED(3);
CyclicBuffer<LED_PWM *> bufferLED(3);

int blinkDelay = 500;
unsigned long lastBlink = millis();
bool pause = false;
byte brightness = MAX_BRIGHTNESS;

void setup()
{
    Serial.begin(9600);
    bufferLED.push(&redLED).push(&greenLED).push(&blueLED);
}

void loop()
{
    blinkLEDs();
    updateBlinkSpeed();
    updateBrightness();
}

void blinkLEDs()
{
    if (millis() - lastBlink > blinkDelay && !pause)
    {
        bufferLED.get()->on();

        for (int i = 0; i < bufferLED.getSize() - 1; ++i)
            bufferLED.next().get()->off();

        //set pointer on LED next to the one that was turned on
        bufferLED.next().next();
        lastBlink = millis();
    }
}

void updateBlinkSpeed()
{
    if (buttonGreen.wasReleased())
    {
        if (blinkDelay == 0)
            pause = false;

        if (blinkDelay + OFFSET < UINT16_MAX)
        {
            blinkDelay += OFFSET;
            Serial.println("blinkDelay increased -> " + String(blinkDelay));
        }
    }
    else if (buttonRed.wasReleased())
        if (blinkDelay - OFFSET >= 0)
        {
            blinkDelay -= OFFSET;
            Serial.println("blinkDelay decreased -> " + String(blinkDelay));
            if (blinkDelay == 0)
                pause = true;
        }
}

void updateBrightness()
{
    brightness = map(analogRead(POTENTIOMETER), 0, 1023, 0, 255);

    redLED.setBrightness(brightness);
    greenLED.setBrightness(brightness);
    blueLED.setBrightness(brightness);
}