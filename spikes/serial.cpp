//* https://www.reddit.com/r/esp32/comments/16qaf8u/trouble_reading_serial_output_on_my_esp32c3/?rdt=50276

#include <Arduino.h>

void setup()
{
    Serial.begin(115200);
    Serial.println("\nHey now.");
    pinMode(8, OUTPUT);
    digitalWrite(8, 0);
}

void loop()
{
    Serial.print('x');
    delay(1000);
}
