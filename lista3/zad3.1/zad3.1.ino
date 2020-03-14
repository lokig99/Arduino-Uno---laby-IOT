// Imię i Nazwisko: Mateusz Groblicki
// Grupa: Czwartek 11:15
// Zadanie: 3.1. Miganie LED ze ściemniaczem

#include "Basic_Button.h"
#include "Basic_LED_PWM.h"
#include "CyclicBuffer.h"

#define RED 6
#define GREEN 5
#define BLUE 3
#define OFFSET 10
#define POTENTIOMETER A0

Button buttonRed(2), buttonGreen(4);
LED_PWM redLED(RED), greenLED(GREEN), blueLED(BLUE);
CyclicBuffer<byte> bufferLED(4);

int blinkDelay = 500;
unsigned long lastBlink = millis();
bool pause = false;
byte brightness = MAX_BRIGHTNESS;

void setup()
{
    Serial.begin(9600);

    bufferLED.push(RED);
    bufferLED.push(GREEN);
    bufferLED.push(BLUE);
}

void loop()
{
    blinkLEDs();
    updateBlinkSpeed();
    updateBrightness();
}

void blinkLEDs()
{
    byte currentLED;

    if (millis() - lastBlink > blinkDelay && !pause)
    {
        bufferLED.next().get(currentLED);

        switch (currentLED)
        {
        case RED:
        {
            redLED.on();
            greenLED.off();
            blueLED.off();
            break;
        }
        case GREEN:
        {
            redLED.off();
            greenLED.on();
            blueLED.off();
            break;
        }
        case BLUE:
        {
            redLED.off();
            greenLED.off();
            blueLED.on();
            break;
        }
        }

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