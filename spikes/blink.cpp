#include <Arduino.h>

const byte pinLed = 8; // LED on pin 8 for Super Mini ESP32-C3
// const byte pinLed2 = 2;

void setup() {
  pinMode(pinLed, OUTPUT);
  // pinMode(pinLed2, OUTPUT);
  // digitalWrite(pinLed2, 1);
  Serial.begin(115200);
}

void loop() {
  Serial.print("X");
  digitalWrite(pinLed, 1);
  delay(1500);
  digitalWrite(pinLed, 0);
  delay(500);
}
