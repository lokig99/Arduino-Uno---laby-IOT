#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include "Basic_LED_PWM.h"

#define ONE_WIRE_BUS A1
#define TEMP_PREC 12
#define TEMP_24C_RAW 3072
#define TEMP_30C_RAW 3840

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress outsideTemp = {0x28, 0xFF, 0xBF, 0x44, 0x50, 0x19, 0x01, 0xFD};
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
LED_PWM blueLED(3), redLED(6);

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
int prevTempRaw;

void setup()
{
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.clear();
    lcd.createChar(DEGREE_CHAR, degree);

    redLED.on();
    blueLED.on();

    if (oneWire.search(outsideTemp))
        Serial.println("outside thermometer found");
    else
        Serial.println("Unable to find outside thermometer");

    sensors.setResolution(TEMP_PREC);
    prevTempRaw = -1000;
}

void loop()
{
    sensors.requestTemperatures();
    printTemp();
}

void printTemp()
{
    int16_t raw = sensors.getTemp(outsideTemp);

    if (raw != prevTempRaw)
    {
        lcd.clear();

        //print temp C
        lcd.print(sensors.rawToCelsius(raw), 4);
        lcd.write(DEGREE_CHAR);
        lcd.print("C");

        //print temp F
        lcd.setCursor(0, 1);
        lcd.print(sensors.rawToFahrenheit(raw), 4);
        lcd.write(DEGREE_CHAR);
        lcd.print("F");

        prevTempRaw = raw;

        updateLED(raw);
    }
}

void updateLED(int16_t raw)
{
    if (raw < TEMP_24C_RAW)
    {
        redLED.setBrightness(0);
        blueLED.setBrightness(MAX_BRIGHTNESS);
    }
    else if (raw > TEMP_30C_RAW)
    {
        redLED.setBrightness(MAX_BRIGHTNESS);
        blueLED.setBrightness(0);
    }
    else
    {
        redLED.setBrightness(map(raw, TEMP_24C_RAW, TEMP_30C_RAW, 0, MAX_BRIGHTNESS));
        blueLED.setBrightness(map(raw, TEMP_30C_RAW, TEMP_24C_RAW, 0, MAX_BRIGHTNESS));
    }
}
