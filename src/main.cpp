#include <Arduino.h>

const byte pinBtnInterval = 16; // Button for changing intervals
const byte pinBtnPwm = 17;      // Button for changing PWM duty cycle
// const byte pinBtnPwm = 4;
const byte pinLedInterval = 22;
const byte pinLedPwm = 23;

const byte pinFan = 18;
const byte pwmChannel = 0;
const int freq = 25000;
const byte resolution = 8;

#include "OneButton.h"
OneButton btnInterval(pinBtnInterval, true);
OneButton btnPwm(pinBtnPwm, true);

int idxInterval = 2;                  // Index for intervals array. -1 -> fan works non-stop
byte intervals[] = {2, 4, 8, 16, 32}; // (minutes) Different options for pause intervals.
byte cntIntervals;                    // Number of elements in intervals array.
byte itvWorking = 1;                  // (minutes) How much time will fan work.
ulong msStarted;                      // (milliseconds) When was the fan last started.
ulong msClick = UINT32_MAX;           // (milliseconds) Moment when button is clicked.
bool isWorking;

const ulong SEC = 1000;
const ulong MIN = 60 * SEC;

void blink(byte pin, int n)
{
    if (n <= 0) // If n is zero, blink once for 1 second
    {
        digitalWrite(pin, 1);
        delay(1000);
    }
    for (size_t i = 1; i < 2 * n; i++) // Otherwise, blink n times with 200ms intervals
    {
        digitalWrite(pin, i % 2);
        delay(200);
    }
    digitalWrite(pin, 0);
}

// start or stop the fan
void fanWorks(bool isOn)
{
    isWorking = isOn;
    if (isOn)
    {
        msStarted = millis();
        // TODO ledcWrite(pwmChannel, ???);
    }
    else
        ledcWrite(pwmChannel, 0);
    // msStarted = 0; // Reset start time when stopping the fan
    // digitalWrite(pinFan, isOn);
    // if (isOn)
    //     msStarted = millis();
}

void anyIntervalClick()
{
    msClick = millis();
    fanWorks(true);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("\nFan Control Setup");
    ledcSetup(pwmChannel, freq, resolution); // Configure channel
    ledcAttachPin(pinFan, pwmChannel);       // Attach pin to channel
    ledcWrite(pwmChannel, 0);                // Set initial duty cycle to 0
    pinMode(pinLedInterval, OUTPUT);
    pinMode(pinLedPwm, OUTPUT);

    btnInterval.attachClick(
        []()
        {
            idxInterval++;
            // If index exceeds the number of intervals, reset it to the last one (max value)
            if (idxInterval >= cntIntervals)
                idxInterval = cntIntervals - 1;
            anyIntervalClick();
            Serial.println("Next interval: " + String(intervals[idxInterval]) + " minutes");
        });
    btnInterval.attachLongPressStart(
        []()
        {
            idxInterval = -1; // Reset index to -1 for non-stop fan operation
            anyIntervalClick();
            Serial.println("Fan works without breaks");
        });
    btnInterval.attachDoubleClick(
        []()
        {
            anyIntervalClick();
            // blink(pinLedInterval, idxInterval + 1);
            Serial.println("Fan is working with interval: " + String(intervals[idxInterval]) + " minutes");
        });

    fanWorks(true);
    blink(pinLedInterval, idxInterval + 1);
    Serial.println("Initial interval: " + String(intervals[idxInterval]) + " minutes");
    cntIntervals = sizeof(intervals) / sizeof(byte);
    // Serial.println("Number of intervals: " + String(cntIntervals));
}

void loop()
{
    delay(10);

    if (idxInterval != -1)
    {
        if (millis() > msStarted + (itvWorking + intervals[idxInterval]) * MIN && !isWorking)
            fanWorks(true);
        if (millis() > msStarted + itvWorking * MIN && isWorking)
            fanWorks(false);
    }
    if (msClick != UINT32_MAX && millis() > msClick + SEC)
    {
        blink(pinLedInterval, idxInterval + 1);
        msClick = UINT32_MAX;
    }

    btnInterval.tick();
    btnPwm.tick();

    // click -> use next interval in intervals array
    // if (btnInterval.clicks == 1 && idxInterval < cntIntervals - 1)
    // {
    //     idxInterval++;
    //     Serial.println("Next interval: " + String(intervals[idxInterval]) + " minutes");
    // }
    // double click -> start the fan immediately - interval remains the same
    // if (btnInterval.clicks == -1) // long click -> reset index to -1 - fan works without breaks
    // {
    //     idxInterval = -1;
    //     Serial.println("Fan works without breaks");
    // }
    // if (btnInterval.clicks == 1 || btnInterval.clicks == 2 || btnInterval.clicks == -1)
    // {
    //     msClick = millis();
    //     fanWorks(true);
    // }
}
