#include <LiquidCrystal.h>
#include <Wire.h>

#define BUTTON_GREEN_PIN 10
#define BUTTON_RED_PIN 11
#define LCD_LED_PIN 9 //tinkercad
const int MAIN_LOOP_DELAY = 50;
const int BUTTON_DELAY = 20;
const byte BUFFER_SIZE = 11;

/************************ STRUCTURES **************************/
template <typename T>
class CyclicBuffer
{
private:
    byte begin, end, pointer, size;
    byte SIZE;
    T *p_array;

public:
    CyclicBuffer(byte size)
    {
        begin, end, pointer, size = 0;
        SIZE = size;
        p_array = new T(size);
    }

    ~CyclicBuffer() { delete[] p_array; }

    void push(T data)
    {
        if (isFull())
            begin = (begin + 1) % SIZE;

        p_array[end] = data;
        end = (end + 1) % SIZE;
        ++size;
    }

    void pull(T &target)
    {
        if (isEmpty())
            return;

        target = p_array[begin];
        begin = (begin + 1) % SIZE;
        --size;
    }

    void next(T &target)
    {
        if (isEmpty())
            return;

        pointer = (pointer + 1) % SIZE;
        if (pointer == end)
            pointer = begin;

        target = p_array[pointer];
    }

    void prev(T &target)
    {
        if (isEmpty())
            return;

        if (pointer == begin)
            pointer = (end == 0) ? SIZE - 1 : end - 1;
        else
            pointer = (pointer == 0) ? SIZE - 1 : pointer - 1;

        target = p_array[pointer];
    }

    void head(T &target)
    {
        if (isEmpty())
            return;

        target = p_array[begin];
    }

    void tail(T &target)
    {
        if (isEmpty())
            return;

        target = p_array[(end == 0) ? SIZE - 1 : end - 1];
    }

    void clear() { begin = end = pointer = this->size = 0; }
    byte getSize() { return size; }
    bool isEmpty() { return end == begin; }
    bool isFull() { return (end + 1) % SIZE == begin; }
};
/****************************************************************/

LiquidCrystal lcd(8, 2, 3, 4, 5, 6, 7);
CyclicBuffer<String> buffer(BUFFER_SIZE);

String message = "";
bool btn_green_prev_state = HIGH;
bool btn_red_prev_state = HIGH;

void setup()
{
    Serial.begin(9600);
    pinMode(BUTTON_GREEN_PIN, INPUT_PULLUP);
    pinMode(BUTTON_RED_PIN, INPUT_PULLUP);
    digitalWrite(LCD_LED_PIN, HIGH); //tinkercad

    lcd.begin(16, 2);
    lcd.clear();
}

void loop()
{
    updateDisplay();

    //END OF MAIN LOOP
    delay(MAIN_LOOP_DELAY);
}

bool hasButtonStateChanged(byte btn_pin)
{
    bool prev_state = getButtonPrevState(btn_pin);
    if (digitalRead(btn_pin) != prev_state)
    {
        //check for button vibrations
        delay(BUTTON_DELAY);
        if (digitalRead(btn_pin) != prev_state)
        {
            setButtonPrevState(btn_pin, !prev_state);
            return true;
        }
    }

    return false;
}

bool getButtonPrevState(byte btn_pin)
{
    switch (btn_pin)
    {
    case BUTTON_GREEN_PIN:
        return btn_green_prev_state;
    case BUTTON_RED_PIN:
        return btn_red_prev_state;
    default:
        return false;
    }
}

void setButtonPrevState(byte btn_pin, bool state)
{
    switch (btn_pin)
    {
    case BUTTON_GREEN_PIN:
        btn_green_prev_state = state;
        break;
    case BUTTON_RED_PIN:
        btn_red_prev_state = state;
        break;
    }
}

void updateDisplay()
{
    if (updateBuffer())
    {
        buffer.tail(message);
        char output[255];
        message.toCharArray(output, message.length() + 1);

        lcd.setCursor(0, 0);
        lcd.clear();
        lcd.print(output);
    }
}

bool updateBuffer()
{
    if (Serial.available() > 0)
    {
        message = Serial.readStringUntil('\n');
        buffer.push(message);
        return true;
    }

    return false;
}
