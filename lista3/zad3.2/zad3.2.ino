// Imię i Nazwisko: Mateusz Groblicki
// Grupa: Czwartek 11:15
// Zadanie: 3.2. Mikser RGB

#include <LiquidCrystal_I2C.h>
#include "Basic_Button.h"
#include "Basic_LED_PWM.h"
#include "CyclicBuffer.h"
#include "Basic_Encoder.h"
#include <Wire.h>

#define RED_POINTER 0
#define GREEN_POINTER 4
#define BLUE_POINTER 10

struct Color
{
    Color(LED_PWM *LED, byte colorDisplayPointer)
    {
        led = LED;
        pointer = colorDisplayPointer;
    }

    Color()
    {
        led = nullptr;
        pointer = 0;
    }

    LED_PWM *led;
    byte pointer;
};

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Button greenButton(4);
LED_PWM greenLED(5), redLED(6), blueLED(3);
CyclicBuffer<Color> colorBuffer(4);
Encoder encoder(A3, A2);

void setup()
{
    Serial.begin(9600);

    redLED.on();
    greenLED.on();
    blueLED.on();

    colorBuffer.push(Color(&redLED, RED_POINTER))
        .push(Color(&greenLED, GREEN_POINTER))
        .push(Color(&blueLED, BLUE_POINTER));

    lcd.begin(16, 2);
    lcd.clear();
    lcd.print(" RED GREEN BLUE");
    lcd.setCursor(RED_POINTER, 1);
    lcd.print(">");
    updateBrightnessReadings(*colorBuffer.get());
    updateBrightnessReadings(*colorBuffer.next().get());
    updateBrightnessReadings(*colorBuffer.next().get());
    colorBuffer.head();
}

void loop()
{
    updateBrightnessValue();
    updatePointer();
}

void updateBrightnessReadings(Color &color)
{
    lcd.setCursor(color.pointer + 1, 1);
    byte bs = color.led->getBrightness();
    lcd.print(((bs < 10) ? "  " : (bs < 100) ? " " : ""));
    lcd.print(bs);
}

void updatePointer()
{
    if (greenButton.wasReleased())
    {
        //clear old pointer
        lcd.setCursor(colorBuffer.get()->pointer, 1);
        lcd.print(" ");

        //print pointer
        lcd.setCursor(colorBuffer.next().get()->pointer, 1);
        lcd.print(">");
    }
}

void updateBrightnessValue()
{
    byte rotation = encoder.readRotation();

    if (rotation != ENCODER_IDLE)
    {
        LED_PWM *led = colorBuffer.get()->led;

        if (rotation == ENCODER_ROTATED_CW)
        {
            if (led->getBrightness() + 1 <= UINT8_MAX)
                led->setBrightness(led->getBrightness() + 1);
        }
        else
        {
            if (led->getBrightness() - 1 >= 0)
                led->setBrightness(led->getBrightness() - 1);
        }

        updateBrightnessReadings(*colorBuffer.get());
    }
}
