// Imię i Nazwisko: Mateusz Groblicki
// Grupa: Czwartek 11:15
// Zadanie: 2.1-2.2. Voltomierz LCD i wykres napięcia

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define BUTTON_PIN 4
#define POTENTIOMETER_PIN A0
const float MAX_VOLTAGE = 5.0;
const int MAX_VOLTAGE_ADC = 1023;
const int MAIN_LOOP_DELAY = 50;
const int BUTTON_DELAY = 20;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

int volt_adc_read = 0;
int volt_adc_prev = -INT16_MAX;
bool lcd_led_state = HIGH;
bool btn_prev_state = HIGH;

void setup()
{
    Serial.begin(9600);

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Miernik A0");
    lcd.setCursor(0, 1);
    lcd.print("V=");
    lcd.setCursor(8, 1);
    lcd.print("ADC=");
}

void loop()
{
    measureVoltage();
    lcdLedUpdate();
    drawChart();

    //END OF MAIN LOOP
    delay(MAIN_LOOP_DELAY);
}

float adcToVoltage(int adc_read)
{
    return (MAX_VOLTAGE * adc_read) / MAX_VOLTAGE_ADC;
}

void printVoltage(float voltage)
{
    lcd.setCursor(2, 1);
    lcd.print(voltage);
}

void printADC(int adc)
{
    lcd.setCursor(12, 1);
    lcd.print(((adc < 10) ? "   " : (adc < 100) ? "  " : (adc < 1000) ? " " : ""));
    lcd.print(adc);
}

void measureVoltage()
{
    volt_adc_read = analogRead(POTENTIOMETER_PIN);

    //print current voltage
    if (abs(volt_adc_read - volt_adc_prev) > 1)
    {
        volt_adc_prev = volt_adc_read;
        printADC(volt_adc_read);
        printVoltage(adcToVoltage(volt_adc_read));
    }
}

bool hasButtonStateChanged()
{
    if (digitalRead(BUTTON_PIN) != btn_prev_state)
    {
        //check for button vibrations
        delay(BUTTON_DELAY);
        if (digitalRead(BUTTON_PIN) != btn_prev_state)
        {
            btn_prev_state = !btn_prev_state;
            return true;
        }
    }

    return false;
}

void lcdLedUpdate()
{
    if (hasButtonStateChanged() && btn_prev_state == HIGH) //if button was released
    {
        lcd_led_state = !lcd_led_state;
        lcd.setBacklight(lcd_led_state);
    }
}

void drawChart()
{
    Serial.print(lcd_led_state);
    Serial.print("\t");

    Serial.print(adcToVoltage(volt_adc_read));
    Serial.print("\t");

    Serial.println("");
}
