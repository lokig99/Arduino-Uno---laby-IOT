#ifndef BASIC_ENCODER_H
#define BASIC_ENCODER_H

#include <Arduino.h>
#include "Basic_Button.h"

#define ENCODER_IDLE 0
#define ENCODER_ROTATED_CW 1
#define ENCODER_ROTATED_CCW 2 

//encoder class
class Encoder
{
private:
    bool a_state;
    bool a_prev_state;
    byte pinA;
    byte pinB;
public:
    Encoder(byte encoderPinA, byte encoderPinB);
    byte readRotation();
};

#endif