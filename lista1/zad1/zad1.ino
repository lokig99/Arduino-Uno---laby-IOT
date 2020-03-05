// Imię i Nazwisko: Mateusz Groblicki
// Grupa: Czwartek 11:15
// Zadanie: 1.1. Licznik na dwa przyciski

#define BUTTON_GREEN_PIN 4
#define BUTTON_RED_PIN 2
#define LED_PIN 13
const int led_delay = 500;
const int button_delay = 20;

int counter = 0;
bool btn_red_prev_state = HIGH;
bool btn_green_prev_state = HIGH;

void setup()
{
    Serial.begin(9600);
    Serial.println("Arduino is ready!");
    pinMode(BUTTON_GREEN_PIN, INPUT_PULLUP);
    pinMode(BUTTON_RED_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
}

void loop()
{
    if(buttonPushed(BUTTON_GREEN_PIN) && buttonPushed(BUTTON_RED_PIN))
    {
        Serial.println("Both buttons pushed, counter = " + String(counter));

        for(int i = 0; i < counter; ++i)
        {
            Serial.println("blink " + String(i));
            digitalWrite(LED_PIN, HIGH);
            delay(led_delay);
            digitalWrite(LED_PIN, LOW);
            delay(led_delay);
        }
    }
    else if(buttonPushed(BUTTON_GREEN_PIN))
        Serial.println("counter increased -> " + String(++counter));
    else if(buttonPushed(BUTTON_RED_PIN))
        Serial.println("counter decreased -> " + String(--counter));
}

bool buttonPushed(int btn_pin)
{
    bool buttonCurrentState = digitalRead(btn_pin);

    if(buttonCurrentState != getButtonPrevState(btn_pin))
    {
        setButtonPrevState(btn_pin, buttonCurrentState);
        if(buttonCurrentState == LOW)
        {
            delay(button_delay);
            if(buttonCurrentState == LOW)
                return true;
        }
    }

    return false;
}

bool getButtonPrevState(int btn_pin)
{
    if(btn_pin == BUTTON_GREEN_PIN)
        return btn_green_prev_state;
    else
        return btn_red_prev_state;
}

void setButtonPrevState(int btn_pin, bool btn_current_state)
{
    if(btn_pin == BUTTON_GREEN_PIN)
        btn_green_prev_state = btn_current_state;
    else
        btn_red_prev_state = btn_current_state;
}