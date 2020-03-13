#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "CyclicBuffer.h"
#include "Basic_Button.h"

/************************ CONSTANTS **************************/

#define LCD_LENGTH 16
#define LCD_HEIGHT 2
#define POTENTIOMETER_PIN A0
const unsigned int MAIN_LOOP_DELAY = 33;
const byte BUFFER_SIZE = 11;

//scroll speed constants
const byte MAX_SCROLL_SPEED = 1;
const byte MIN_SCROLL_SPEED = 32;
const int MAX_SCROLL_SPEED_ADC = 1023;
const int MIN_SCROLL_SPEED_ADC = 10;

/************************ STRUCTURES **************************/

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

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
CyclicBuffer<String> buffer(BUFFER_SIZE);
DisplayLine displayLines[2] = {DisplayLine(0, "", false), DisplayLine(1, "", false)};
Button buttonGreen(4), buttonRed(2);

unsigned int scrollSpeed = 0;
unsigned int potentiometer_prev_reading = 0;
unsigned long frames = 0;
String emptyLine;

/****************************** MAIN ******************************/

void setup()
{
    Serial.begin(9600);

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

    if (abs(reading - potentiometer_prev_reading) > 1)
    {
        potentiometer_prev_reading = reading;
        if (reading < MIN_SCROLL_SPEED_ADC)
            scrollSpeed = 0;
        else
            scrollSpeed =
                map(reading, MIN_SCROLL_SPEED_ADC, MAX_SCROLL_SPEED_ADC, MIN_SCROLL_SPEED, MAX_SCROLL_SPEED);
    }
}

void scrollLogsUp(Button &button)
{
    String line1;
    String line2;

    if (button.wasReleased())
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

    if (button.wasReleased())
    {
        if (buffer.getPointer() != buffer.getTailPosition())
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
