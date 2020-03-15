#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include "Basic_Button.h"
#include <limits.h>

#define ONE_WIRE_BUS A1
#define TEMP_PREC 9
#define DISPLAY_INTERVAL 5000
#define DISPLAY_CURRENT_TEMP 0
#define DISPLAY_MINMAX_IN_TEMP 1
#define DISPLAY_MINMAX_OUT_TEMP 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress outsideTemp = {0x28, 0xFF, 0xBF, 0x44, 0x50, 0x19, 0x01, 0xFD};
DeviceAddress insideTemp = {0x28, 0xFF, 0xBF, 0x44, 0x50, 0x19, 0x01, 0xFD};
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Button redButton(2);

byte degree[8] = {
    B00110,
    B01001,
    B01001,
    B00110,
    B00000,
    B00000,
    B00000,
};

const byte DEGREE_CHAR = 0;
int prevTempOutRaw;
int prevTempInRaw;
int inMaxTempRaw;
int inMinTempRaw;
int outMaxTempRaw;
int outMinTempRaw;
int inTempRaw;
int outTempRaw;
unsigned long lastDisplayUpdate;
byte displayMode = DISPLAY_CURRENT_TEMP;
byte lastDisplayMode = 255;
bool minmax_updated = false;

void setup()
{
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.clear();
    lcd.createChar(DEGREE_CHAR, degree);

    if (oneWire.search(outsideTemp))
        Serial.println(F("outside thermometer found"));
    else
        Serial.println(F("Unable to find outside thermometer"));

    if (oneWire.search(insideTemp))
        Serial.println(F("inside thermometer found"));
    else
        Serial.println(F("Unable to find inside thermometer"));

    sensors.setResolution(TEMP_PREC);
    lastDisplayUpdate = millis();
    updateTempReadings();
    resetMinMax();
    printCurrentTemp();

    attachInterrupt(digitalPinToInterrupt(2), onRedButtonPressed, FALLING);
}

void loop()
{
    updateTempReadings();
    updateMinMax();

    //update display mode
    fix_time();
    if (millis() - lastDisplayUpdate > DISPLAY_INTERVAL)
    {
        displayMode = (displayMode + 1) % 3;
        lastDisplayUpdate = millis();
    }

    if (displayMode == DISPLAY_CURRENT_TEMP)
        printCurrentTemp();
    else if (displayMode == DISPLAY_MINMAX_IN_TEMP)
        printMinMaxInside();
    else
        printMinMaxOutside();
}

void printCurrentTemp()
{
    if (lastDisplayMode != DISPLAY_CURRENT_TEMP)
    {
        lcd.clear();
        //print inside temp C
        lcd.print(F("INSIDE:"));
        lcd.setCursor(9, 0);
        lcd.print(sensors.rawToCelsius(inTempRaw), 1);
        lcd.write(DEGREE_CHAR);
        lcd.print(F("C"));

        //print outside temp C
        lcd.setCursor(0, 1);
        lcd.print(F("OUTSIDE:"));
        lcd.setCursor(9, 1);
        lcd.print(sensors.rawToCelsius(outTempRaw), 1);
        lcd.write(DEGREE_CHAR);
        lcd.print(F("C"));

        lastDisplayMode = DISPLAY_CURRENT_TEMP;
    }
    else
    {
        if (inTempRaw != prevTempInRaw)
        {
            lcd.setCursor(9, 0);
            lcd.print(F("       "));
            lcd.setCursor(9, 0);
            lcd.print(sensors.rawToCelsius(inTempRaw), 1);
            lcd.write(DEGREE_CHAR);
            lcd.print(F("C"));

            prevTempInRaw = inTempRaw;
        }

        if (outTempRaw != prevTempOutRaw)
        {
            lcd.setCursor(9, 1);
            lcd.print(F("       "));
            lcd.setCursor(9, 1);
            lcd.print(sensors.rawToCelsius(outTempRaw), 1);
            lcd.write(DEGREE_CHAR);
            lcd.print(F("C"));

            prevTempOutRaw = outTempRaw;
        }
    }
}

void printMinMaxInside()
{
    if (lastDisplayMode != DISPLAY_MINMAX_IN_TEMP)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("IN min/max"));
        lcd.setCursor(0, 1);
        lcd.print(sensors.rawToCelsius(inMinTempRaw), 1);
        lcd.write(DEGREE_CHAR);
        lcd.print(F("C/"));
        lcd.print(sensors.rawToCelsius(inMaxTempRaw), 1);
        lcd.write(DEGREE_CHAR);
        lcd.print(F("C"));

        lastDisplayMode = DISPLAY_MINMAX_IN_TEMP;
    }
    else if (minmax_updated)
    {
        lcd.setCursor(0, 1);
        lcd.print(F("                "));
        lcd.setCursor(0, 1);
        lcd.print(sensors.rawToCelsius(inMinTempRaw), 1);
        lcd.write(DEGREE_CHAR);
        lcd.print(F("C/"));
        lcd.print(sensors.rawToCelsius(inMaxTempRaw), 1);
        lcd.write(DEGREE_CHAR);
        lcd.print(F("C"));
    }
}

void printMinMaxOutside()
{
    if (lastDisplayMode != DISPLAY_MINMAX_OUT_TEMP)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("OUT min/max"));
        lcd.setCursor(0, 1);
        lcd.print(sensors.rawToCelsius(outMinTempRaw), 1);
        lcd.write(DEGREE_CHAR);
        lcd.print(F("C/"));
        lcd.print(sensors.rawToCelsius(outMaxTempRaw), 1);
        lcd.write(DEGREE_CHAR);
        lcd.print(F("C"));

        lastDisplayMode = DISPLAY_MINMAX_OUT_TEMP;
    }
    else if (minmax_updated)
    {
        lcd.setCursor(0, 1);
        lcd.print(F("                "));
        lcd.setCursor(0, 1);
        lcd.print(sensors.rawToCelsius(outMinTempRaw), 1);
        lcd.write(DEGREE_CHAR);
        lcd.print(F("C/"));
        lcd.print(sensors.rawToCelsius(outMaxTempRaw), 1);
        lcd.write(DEGREE_CHAR);
        lcd.print(F("C"));
    }
}

void updateMinMax()
{
    bool updated = false;

    //update inside min/max
    if (outTempRaw > outMaxTempRaw)
    {
        outMaxTempRaw = outTempRaw;
        updated = true;
    }

    else if (outTempRaw < outMinTempRaw)
    {
        outMinTempRaw = outTempRaw;
        updated = true;
    }

    //update outside min/max
    if (inTempRaw > inMaxTempRaw)
    {
        inMaxTempRaw = inTempRaw;
        updated = true;
    }
    else if (inTempRaw < inMinTempRaw)
    {
        inMinTempRaw = inTempRaw;
        updated = true;
    }

    minmax_updated = updated;
}

void resetMinMax()
{
    inMaxTempRaw = inTempRaw;
    inMinTempRaw = inTempRaw;
    outMaxTempRaw = outTempRaw;
    outMinTempRaw = outTempRaw;
}

void onRedButtonPressed()
{
    if (redButton.onInterrupt())
        resetMinMax();
}

void fix_time()
{
    //fix time of last update when arduino time clock overflows
    if (millis() < lastDisplayUpdate)
        lastDisplayUpdate = ULONG_MAX - lastDisplayUpdate;
}

void updateTempReadings()
{
    sensors.requestTemperatures();
    inTempRaw = sensors.getTemp(insideTemp);
    outTempRaw = sensors.getTemp(outsideTemp);
}
