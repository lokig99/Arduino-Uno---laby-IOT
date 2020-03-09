#include <LiquidCrystal.h>
#include <Wire.h>
#include "CyclicBuffer.h"

/************************ CONSTANTS **************************/

#define BUTTON_GREEN_PIN 10
#define BUTTON_RED_PIN 11
#define LCD_LED_PIN 9 //tinkercad
#define LCD_LENGTH 16
#define LCD_HEIGHT 2
#define POTENTIOMETER_PIN A0
const unsigned int MAIN_LOOP_DELAY = 33;
const unsigned int BUTTON_INTERVAL = 20;
const byte BUFFER_SIZE = 5;

//scroll speed constants
const byte MAX_SCROLL_SPEED = 1;
const byte MIN_SCROLL_SPEED = 32;
const int MAX_SCROLL_SPEED_ADC = 1023;
const int MIN_SCROLL_SPEED_ADC = 100;
const float A = (float)(MIN_SCROLL_SPEED - MAX_SCROLL_SPEED) / (float)(MIN_SCROLL_SPEED_ADC - MAX_SCROLL_SPEED_ADC);
const float B = (float)MIN_SCROLL_SPEED - (float)(A * MIN_SCROLL_SPEED_ADC);

struct Button
{
    Button(byte buttonPin, bool buttonState)
    {
        pin = buttonPin;
        state = buttonState;
        update = false;
    }

    byte pin;
    bool state;
    bool update;
};

struct DisplayLine
{
    DisplayLine(byte displayLine, String displayText, bool textAutoscroll)
    {
        line = displayLine;
        setText(displayText);
        autoscroll = textAutoscroll;
    }

    void setText(String displayText)
    {
        prev_size = size;
        size = displayText.length();
        text = displayText;
        offset = 0;
    }

    byte line;
    byte size;
    byte prev_size = 0;
    byte offset = 0;
    String text;
    bool autoscroll;
};


/*************************** VARIABLES ***************************/

LiquidCrystal lcd(8, 2, 3, 4, 5, 6, 7);
CyclicBuffer<String> buffer(BUFFER_SIZE);
DisplayLine displayLines[2] = {DisplayLine(0, "", false), DisplayLine(1, "", false)};
Button buttonGreen(BUTTON_GREEN_PIN, HIGH), buttonRed(BUTTON_RED_PIN, HIGH);

unsigned int scrollSpeed = 0;
unsigned int potentiometer_prev_reading = 0;
unsigned long frames = 0;
String emptyLine;

/****************************** MAIN ******************************/

void setup()
{

    Serial.begin(9600);
    pinMode(BUTTON_GREEN_PIN, INPUT_PULLUP);
    pinMode(BUTTON_RED_PIN, INPUT_PULLUP);
    pinMode(LCD_LED_PIN, OUTPUT);
    digitalWrite(LCD_LED_PIN, HIGH); //tinkercad

    lcd.begin(LCD_LENGTH, LCD_HEIGHT);
    lcd.clear();

    emptyLine = "";
    for (int i = 0; i < LCD_LENGTH; ++i)
        emptyLine += " ";
}

void loop()
{
    scrollLogsUp(buttonGreen);
    scrollLogsDown(buttonRed);
    updateDisplayLines();
    updateScrollSpeed();
    printLineOnDisplay(displayLines[0]);
    printLineOnDisplay(displayLines[1]);

    //END OF MAIN LOOP
    ++frames;
    delay(MAIN_LOOP_DELAY);
}

/*************************** METHODS *********************************/

bool hasButtonStateChanged(Button &button)
{
    if (digitalRead(button.pin) != button.state)
    {
        //check for button vibrations
        if (button.update)
        {
            button.state = !button.state;
            button.update = false;
            return true;
        }
        else
            button.update = true;
    }

    return false;
}

void updateDisplayLines()
{
    String tmp;

    if (updateBuffer())
    {
        //set previous log
        if (buffer.getSize() > 1)
        {
            buffer.tail().prev().get(tmp);
            displayLines[0].setText(tmp);
        }

        //set new log
        buffer.tail().get(tmp);
        displayLines[1].setText(tmp);
        updateAutoscrollState(displayLines[1]);
    }
}

bool updateBuffer()
{
    if (Serial.available() > 0)
    {
        String message = Serial.readStringUntil('\n');
        buffer.push(message);
        return true;
    }

    return false;
}

void printLineOnDisplay(DisplayLine &displayLine)
{
    if (displayLine.autoscroll)
    {
        String text = emptyLine + String(displayLine.text) + emptyLine;

        if (scrollSpeed > 0)
        {
            if (frames % scrollSpeed == 0)
            {
                clearLineOnDisplay(displayLine);
                for (int i = displayLine.offset; i <= LCD_LENGTH + displayLine.offset; ++i)
                    lcd.print(text[i]);

                displayLine.offset =
                    (displayLine.offset >= text.length() - LCD_LENGTH) ? 0 : displayLine.offset + 1;
            }
        }
        else if (displayLine.offset == 0)
        {
            clearLineOnDisplay(displayLine);
            displayLine.offset = emptyLine.length();
            lcd.print(displayLine.text);
        }
    }
    else
    {
        if (displayLine.prev_size > displayLine.size)
        {
            clearLineOnDisplay(displayLine);
            displayLine.prev_size = 0;
        }
        else
            lcd.setCursor(0, displayLine.line);
        lcd.print(displayLine.text);
    }
}

void clearLineOnDisplay(DisplayLine &displayLine)
{
    lcd.setCursor(0, displayLine.line);
    lcd.print(emptyLine);
    lcd.setCursor(0, displayLine.line);
}

void updateScrollSpeed()
{
    int reading = analogRead(POTENTIOMETER_PIN);

    if (abs(reading - potentiometer_prev_reading) > 10)
    {
        potentiometer_prev_reading = reading;
        if (reading < MIN_SCROLL_SPEED_ADC)
            scrollSpeed = 0;
        else
            //speed is a f(x) = a * x + b function
            //the higher scrollSpeed value the slower text will be scrolled
            scrollSpeed = A * reading + B;
    }
}

void scrollLogsUp(Button &button)
{
    String line1;
    String line2;

    if (hasButtonStateChanged(button) && button.state == HIGH) //if button was released
    {
        buffer.prev().get(line2);
        buffer.prev().get(line1);
        buffer.next();

        if (buffer.getPointer() == buffer.getHeadPosition())
            displayLines[0].setText("");
        else
            displayLines[0].setText(line1);

        displayLines[1].setText(line2);
        updateAutoscrollState(displayLines[1]);
    }
}

void scrollLogsDown(Button &button)
{
    String line1;
    String line2;

    if (hasButtonStateChanged(button) && button.state == HIGH) //if button was released
    {
        if (!(buffer.getPointer() == buffer.getTailPosition()))
            buffer.get(line1);
        buffer.next().get(line2);

        displayLines[0].setText(line1);
        displayLines[1].setText(line2);
        updateAutoscrollState(displayLines[1]);
    }
}

bool updateAutoscrollState(DisplayLine &displayLine)
{
    if (displayLine.size > LCD_LENGTH)
    {
        clearLineOnDisplay(displayLine);
        displayLine.autoscroll = true;
    }
    else
        displayLine.autoscroll = false;
}
