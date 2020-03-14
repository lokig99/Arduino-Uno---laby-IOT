#include "Basic_Encoder.h"

Encoder::Encoder(byte encoderPinA, byte encoderPinB)
{
    pinA = encoderPinA;
    pinB = encoderPinB;
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    a_prev_state = digitalRead(pinA);
}

byte Encoder::readRotation()
{
    a_state = digitalRead(pinA);
    byte return_val = ENCODER_IDLE;

    if (a_state != a_prev_state && a_state == LOW)
        if (digitalRead(pinB) != a_state)
            return_val = ENCODER_ROTATED_CW;
        else
            return_val = ENCODER_ROTATED_CCW;

    a_prev_state = a_state;
    return return_val;
}