#include <Arduino.h>

const byte pinFan = 18;
const byte pwmChannel = 0;
const int freq = 25000;
const byte resolution = 8;

void setup()
{
    ledcSetup(pwmChannel, freq, resolution); // Configure channel
    ledcAttachPin(pinFan, pwmChannel);       // Attach pin to channel
    ledcWrite(pwmChannel, 0);                // Set initial duty cycle to 0
}

void loop()
{
}
