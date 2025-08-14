#include <Arduino.h>

const bool isMini = true;               // Set to true for Super Mini ESP32-C3, false for other boards
const byte onBoardLed = isMini ? 8 : 2; // On-board LED pin

const byte pinBtnInterval = isMini ? 3 : 16;          // Button for changing intervals
const byte pinBtnPwm = isMini ? 4 : 17;               // Button for changing PWM duty cycle
const byte pinLedInterval = isMini ? onBoardLed : 22; // LED for interval indication
const byte pinLedPwm = isMini ? 1 : 23;               // LED for PWM duty cycle indication
const byte pinFan = isMini ? 2 : 18;                  // Fan control pin

const byte pwmChannel = 0; // PWM channel for fan control
const int freq = 25000;    // Frequency for PWM (25 kHz)
const byte resolution = 8; // Resolution for PWM (8 bits, 0-255)

#include "OneButton.h"
OneButton btnInterval(pinBtnInterval, true);
OneButton btnPwm(pinBtnPwm, true);

int idxPwm;                                       // Index for pwms array.
const byte pwms[] = {51, 102, 153, 204, 255};     // (PWM duty cycle) Different options for fan speed.
const byte cntPwms = sizeof(pwms) / sizeof(byte); // Number of elements in pwms array.

// idx   -1  0   1   2   3   4
// sp1   x   51  102 153 204 255     speed/duty cycle for fan
// pa1   0   2   4   8   16  32      interval in minutes for fan working
// pa2   x   16  8   4   2   0       rejected
// sp2   255 204 153 102 51  0       rejected

int idxInterval;                                            // Index for intervals array. -1 -> fan works non-stop
byte intervals[] = {2, 4, 8, 16, 32};                       // (minutes) Different options for pause intervals.
const byte cntIntervals = sizeof(intervals) / sizeof(byte); // Maximum number of intervals.

byte itvWorking = 1; // (minutes) How much time will fan work.
ulong msStarted;     // (milliseconds) When was the fan last started.
bool isWorking;      // Is the fan currently working?
//-ulong msClick = UINT32_MAX; // (milliseconds) Moment when button is clicked.

const ulong SEC = 1000;
const ulong MIN = 60 * SEC;

// digitalWrite for blink() function
// This function is used to control the on-board LED or any other LED connected to a pin
void blinkDigitalWrite(byte pin, int value)
{
    if (isMini && pin == onBoardLed) // If using Super Mini ESP32-C3, invert the LED logic
        digitalWrite(pin, !value);
    else
        digitalWrite(pin, value);
}

// Blinks the LED connected to pin for n times
void blink(byte pin, int n)
{
    if (n <= 0) // If n is zero, blink once for 1 second
    {
        blinkDigitalWrite(pin, 1);
        delay(1000);
    }
    for (size_t i = 1; i < 2 * n; i++) // Otherwise, blink n times with 200ms intervals
    {
        blinkDigitalWrite(pin, i % 2);
        delay(200);
    }
    blinkDigitalWrite(pin, 0); // Turn off the LED after blinking
}

// start or stop the fan
void fanWorks(bool isOn)
{
    isWorking = isOn;
    if (isOn)
    {
        msStarted = millis();
        ledcWrite(pwmChannel, pwms[idxPwm]); // Set PWM duty cycle based on current index
    }
    else
        ledcWrite(pwmChannel, 0);
}

void anyIntervalClick()
{
    fanWorks(true);
    blink(pinLedInterval, idxInterval + 1);
    //- msClick = millis();
}

void anyPwmClick()
{
    Serial.println("PWM duty cycle: " + String(pwms[idxPwm]) + "%");
    fanWorks(true);
    blink(pinLedPwm, idxPwm + 1);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("\nFan Control Setup");
    pinMode(pinLedInterval, OUTPUT);
    pinMode(pinLedPwm, OUTPUT);
    // pinMode(pinFan, OUTPUT);
    // digitalWrite(pinFan, LOW); // Ensure the fan is off initially
    ledcSetup(pwmChannel, freq, resolution); // Configure channel
    ledcAttachPin(pinFan, pwmChannel);       // Attach pin to channel
    // ledcWrite(pwmChannel, 0);                // Set initial duty cycle to 0
    idxInterval = 1;
    idxPwm = 1;
    fanWorks(true);
    blink(pinLedPwm, idxPwm + 1);
    Serial.println("Initial PWM duty cycle: " + String(pwms[idxPwm]) + "%");
    blink(pinLedInterval, idxInterval + 1);
    Serial.println("Initial interval: " + String(intervals[idxInterval]) + " minutes");

    btnInterval.attachClick(
        []()
        {
            if (idxInterval < cntIntervals - 1)
                idxInterval++;
            anyIntervalClick();
            Serial.println("Next interval: " + String(intervals[idxInterval]) + " minutes");
        });
    btnInterval.attachDoubleClick(
        []()
        {
            if (idxInterval > 0)
                idxInterval--;
            anyIntervalClick();
            Serial.println("Fan is working with interval: " + String(intervals[idxInterval]) + " minutes");
        });
    btnInterval.attachLongPressStart(
        []()
        {
            idxInterval = -1; // Reset index to -1 for non-stop fan operation
            anyIntervalClick();
            Serial.println("Fan works without breaks");
        });
    btnPwm.attachClick(
        []()
        {
            if (idxPwm < cntPwms - 1)
                idxPwm++;
            else
                idxPwm = 0; // If already at max, reset to minimum speed
            anyPwmClick();
        });
    btnPwm.attachDoubleClick(
        []()
        {
            if (idxPwm > 0)
                idxPwm--;
            anyPwmClick();
        });
    btnPwm.attachLongPressStart(
        []()
        {
            if (idxPwm < cntPwms - 1)
                idxPwm = cntPwms - 1; // Reset index to the last PWM value (maximum speed)
            // else
            //     idxPwm = 0; // If already at max, reset to minimum speed
            anyPwmClick();
            idxInterval = -1; // Reset index to -1 for non-stop fan operation
            anyIntervalClick();
        });
}

void loop()
{
    delay(10);

    if (idxInterval != -1)
    {
        // checks if the fan should start or stop based on the intervals
        if (millis() > msStarted + (itvWorking + intervals[idxInterval]) * MIN && !isWorking)
            fanWorks(true);
        if (millis() > msStarted + itvWorking * MIN && isWorking)
            fanWorks(false);
    }

    btnInterval.tick();
    btnPwm.tick();
}
