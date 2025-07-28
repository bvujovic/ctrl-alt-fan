//* Fan is working for {itvWorking} minutes, then it's off for some time (2, 4...).

#include <Arduino.h>
typedef unsigned long ulong;

#include "ClickButton.h"
const byte pinButton = 0;
ClickButton btn(pinButton, LOW, CLICKBTN_PULLUP);
int idx = 2;                          // Index for intervals array. -1 -> fan works non-stop
byte intervals[] = {2, 4, 8, 16, 32}; // (minutes) Different options for pause intervals.
byte cntIntervals;                    // Number of elements in intervals array.
byte itvWorking = 1;                  // (minutes) How much time will fan work.
ulong msStarted;                      // (milliseconds) When was the fan last started.
ulong msClick = UINT32_MAX;           // (milliseconds) Moment when button is clicked.
bool isWorking;

const byte pinFan = 4;
const byte pinLed = 3;
const ulong SEC = 1000;
const ulong MIN = 60 * SEC;

void blink(int n)
{
    if (n <= 0)
    {
        digitalWrite(pinLed, 1);
        delay(1000);
    }
    for (size_t i = 1; i < 2 * n; i++)
    {
        digitalWrite(pinLed, i % 2);
        delay(200);
    }
    digitalWrite(pinLed, 0);
}

// start or stop the fan
void fanWorks(bool on)
{
    digitalWrite(pinFan, on);
    isWorking = on;
    if (on)
        msStarted = millis();
}

void setup()
{
    pinMode(pinFan, OUTPUT);
    pinMode(pinLed, OUTPUT);
    fanWorks(true);
    // blink(sizeof(ulong));
    blink(idx + 1);
    cntIntervals = sizeof(intervals) / sizeof(byte);
}

void loop()
{
    delay(10);

    if (idx != -1)
    {
        if (millis() > msStarted + (itvWorking + intervals[idx]) * MIN && !isWorking)
            fanWorks(true);
        if (millis() > msStarted + itvWorking * MIN && isWorking)
            fanWorks(false);
    }
    if (msClick != UINT32_MAX && millis() > msClick + SEC)
    {
        blink(idx + 1);
        msClick = UINT32_MAX;
    }

    btn.Update();
    // if (btn.clicks == 1) // click -> use next interval in intervals array
    // {
    //     if (idx < cntIntervals - 1)
    //         idx++;
    //     fanWorks(true);
    // }
    // if (btn.clicks == 2) // double click -> start the fan immediately - interval remains the same
    // {
    //     fanWorks(true);
    // }
    // if (btn.clicks == -1) // long click -> reset index to -1 - fan works without breaks
    // {
    //     idx = -1;
    //     fanWorks(true);
    // }

    if (btn.clicks == 1) // click -> use next interval in intervals array
        if (idx < cntIntervals - 1)
            idx++;
    // double click -> start the fan immediately - interval remains the same
    if (btn.clicks == -1) // long click -> reset index to -1 - fan works without breaks
        idx = -1;
    if (btn.clicks == 1 || btn.clicks == 2 || btn.clicks == -1)
    {
        msClick = millis();
        fanWorks(true);
    }
}
