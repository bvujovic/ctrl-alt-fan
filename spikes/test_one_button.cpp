#include <OneButton.h>

#define BUTTON_PIN 15 // adjust for your setup

OneButton button(BUTTON_PIN, true); // true = active LOW (pull-up)

void setup()
{
    Serial.begin(115200);
    button.attachClick([]()
                       { Serial.println("Single click"); });
    button.attachDoubleClick([]()
                             { Serial.println("Double click"); });
    button.attachLongPressStart([]()
                                { Serial.println("Long press start"); });
}

void loop()
{
    button.tick(); // Must be called frequently (ideally every loop)
    delay(10); // Adjust delay as needed
}
