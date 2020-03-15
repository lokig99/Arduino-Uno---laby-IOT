#include "Basic_Encoder.h"

Encoder encoder(A3, A2);
int counter = 0;

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    byte rotation = encoder.readRotation();

    if (rotation != ENCODER_IDLE)
    {
        if (rotation == ENCODER_ROTATED_CW)
            ++counter;
        else
            --counter;

        Serial.println(counter);
    }
}
