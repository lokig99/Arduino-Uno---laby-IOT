#define LED_PIN 13
#define LED_OFF 0
#define LED_ON 1
#define LED_BLINK 2
const String COMMAND_LED_ON = "LED ON";
const String COMMAND_LED_OFF = "LED OFF";
const String COMMAND_LED_BLINK = "LED BLINK";

short ledStatus = LED_OFF;
long ledBlinkRate = 1000;
String command = "";

void setup()
{
    Serial.begin(9600);
    Serial.println("Arduino is ready!");
    pinMode(LED_PIN, OUTPUT);
}

void loop()
{
    if(ledStatus == LED_BLINK)
    {
        digitalWrite(LED_PIN, LOW);
        delay(ledBlinkRate);
        digitalWrite(LED_PIN, HIGH);
        delay(ledBlinkRate);
    }

    checkForCommands();
}

void checkForCommands()
{
    if(Serial.available() > 0)
    {
        command = Serial.readStringUntil('\n');

        if(command.equals(COMMAND_LED_OFF))
        {
            digitalWrite(LED_PIN, LOW);
            Serial.println("LED status - off");
            ledStatus = LED_OFF;
        }
        else if(command.equals(COMMAND_LED_ON))
        {
            digitalWrite(LED_PIN, HIGH);
            Serial.println("LED status - on");
            ledStatus = LED_ON;
        }
        //check for "LED BLINK"
        else if(command.indexOf(COMMAND_LED_BLINK) == 0)
        {
            if(command.equals(COMMAND_LED_BLINK))
            {
                Serial.print("LED status - blinking at previous rate of: ");
                Serial.println(ledBlinkRate);
                ledStatus = LED_BLINK;
            }
            else
            {
                long blinkRate = command.substring(COMMAND_LED_BLINK.length()).toInt();
                if(blinkRate <= 0)
                {
                    Serial.print("Error - invalid blink rate: ");
                    Serial.println(blinkRate);
                }
                else
                {
                    ledBlinkRate = blinkRate; 
                    Serial.print("LED status - blinking at rate of: ");
                    Serial.println(ledBlinkRate);
                    ledStatus = LED_BLINK;
                }
            }       
        }
        else
        {
            Serial.print("Error - Unknown command: ");
            Serial.println(command);
        }
    }
}