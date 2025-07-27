#include <Arduino.h>

// Example: PWM on pin 18 with 25 kHz, 8-bit resolution
const byte pinFan = 18;
const byte pinZero = 19; // Just for testing, not used in this example
const byte pwmChannel = 0;
const int freq = 25000;
const byte resolution = 8;

void setup()
{
    ledcSetup(pwmChannel, freq, resolution); // Configure channel
    ledcAttachPin(pinFan, pwmChannel);       // Attach pin to channel
    ledcWrite(pwmChannel, 0);                // Set initial duty cycle to 0
    // delay(1000); // Wait for a second
    // pinMode(pinFan, OUTPUT);                 
    // digitalWrite(pinFan, LOW);               
    pinMode(pinZero, OUTPUT); // Set pinZero as output
    digitalWrite(pinZero, LOW); // Set pinZero low
}

void loop()
{
    ledcWrite(pwmChannel, 0);
    delay(5000);
    ledcWrite(pwmChannel, 64); // 25% duty cycle
    delay(5000);
    ledcWrite(pwmChannel, 128); // 50% duty cycle
    delay(5000);
    ledcWrite(pwmChannel, 192); // 75% duty cycle
    delay(5000);
    ledcWrite(pwmChannel, 255); // 100% duty cycle
    delay(5000);

    // // Fade up
    // for (int duty = 0; duty <= 255; duty++)
    // {
    //     ledcWrite(pwmChannel, duty);
    //     delay(10);
    // }
    // // Fade down
    // for (int duty = 255; duty >= 0; duty--)
    // {
    //     ledcWrite(pwmChannel, duty);
    //     delay(10);
    // }
}
