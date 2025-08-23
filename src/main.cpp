#include <Arduino.h>
// if DISPLAY_SCREEN is true - current state (speed:pause) will be shown on display
#define DISPLAY_SCREEN true

#if DISPLAY_SCREEN
#include <U8g2lib.h> // lib_deps = olikraus/U8g2@^2.36.12
#endif

const bool isMini = true;               // Set to true for Super Mini ESP32-C3, false for other boards
const byte onBoardLed = isMini ? 8 : 2; // On-board LED pin

const byte pinBtnPwm = isMini ? 4 : 17;               // Button for changing PWM duty cycle
const byte pinBtnInterval = isMini ? 3 : 16;          // Button for changing intervals
const byte pinLedPwm = isMini ? 1 : 23;               // LED for PWM duty cycle indication
const byte pinLedInterval = isMini ? onBoardLed : 22; // LED for interval indication
const byte pinFan = isMini ? 2 : 18;                  // Fan control pin (PWM output: GPIO 2, 4, or 5)

const byte pwmChannel = 0; // PWM channel for fan control
const int freq = 25000;    // Frequency for PWM (25 kHz)
const byte resolution = 8; // Resolution for PWM (8 bits, 0-255)

#include "OneButton.h"
OneButton btnPwm(pinBtnPwm, true);
OneButton btnInterval(pinBtnInterval, true);

int idxPwm;                                       // Index for pwms array.
const byte pwms[] = {51, 102, 153, 204, 255};     // (PWM duty cycle) Different options for fan speed.
const byte cntPwms = sizeof(pwms) / sizeof(byte); // Number of elements in pwms array.

int idxInterval;                                            // Index for intervals array. -1 -> fan works non-stop
byte intervals[] = {2, 4, 8, 16, 32};                       // (minutes) Different options for pause intervals.
const byte cntIntervals = sizeof(intervals) / sizeof(byte); // Maximum number of intervals.

byte itvWorking = 1; // (minutes) How much time will fan work.
ulong msStarted;     // (milliseconds) When was the fan last started.
bool isWorking;      // Is the fan currently working?

const ulong SEC = 1000;
const ulong MIN = 60 * SEC;

#if DISPLAY_SCREEN
U8G2_SSD1306_72X40_ER_F_HW_I2C disp(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/6, /* data=*/5);
char dispBuffer[6];
int dx, dy;
ulong msDisplayed = 0;
bool isDisplayOn = true;

void display()
{
    disp.setPowerSave(0);
    int pause = idxInterval == -1 ? 0 : intervals[idxInterval];
    sprintf(dispBuffer, "%d:%d", idxPwm + 1, pause);
    dx = (disp.getDisplayWidth() - disp.getStrWidth(dispBuffer)) / 2;
    disp.clearBuffer();
    disp.drawStr(dx, dy, dispBuffer);
    disp.sendBuffer();
    msDisplayed = millis();
    isDisplayOn = true;
}
#else
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
#endif

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

void anyPwmClick()
{
    // Serial.println("PWM duty cycle: " + String(pwms[idxPwm]) + "%");
    fanWorks(true);
#if DISPLAY_SCREEN
    display();
#else
    blink(pinLedPwm, idxPwm + 1);
#endif
}

void anyIntervalClick()
{
    fanWorks(true);
#if DISPLAY_SCREEN
    display();
#else
    blink(pinLedInterval, idxInterval + 1);
#endif
}

void setup()
{
    // Serial.begin(115200);
    ledcSetup(pwmChannel, freq, resolution); // Configure channel
    ledcAttachPin(pinFan, pwmChannel);       // Attach pin to channel
    idxInterval = 1;
    idxPwm = 1;
    fanWorks(true);
    // Serial.println("Initial PWM duty cycle: " + String(pwms[idxPwm]) + "%");
    // Serial.println("Initial interval: " + String(intervals[idxInterval]) + " minutes");
#if DISPLAY_SCREEN
    disp.begin();
    disp.clearBuffer(); // clear the internal memory
    disp.setFont(u8g2_font_logisoso30_tf);
    dy = disp.getDisplayHeight() - (disp.getDisplayHeight() - 30) / 2; // 30 is the size of the font - change IN
    disp.setDisplayRotation(U8G2_R2);                                  // Rotate display 180 degrees (upside down)
    display();
#else
    pinMode(pinLedPwm, OUTPUT);
    pinMode(pinLedInterval, OUTPUT);
    blink(pinLedPwm, idxPwm + 1);
    blink(pinLedInterval, idxInterval + 1);
#endif

    btnPwm.attachClick(
        []()
        {
            if (isDisplayOn)
            {
                if (idxPwm < cntPwms - 1)
                    idxPwm++;
                else
                    idxPwm = 0; // If already at max, reset to minimum speed
                anyPwmClick();
            }
            else
                display();
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
            anyPwmClick();
            idxInterval = -1; // Reset index to -1 for non-stop fan operation
            anyIntervalClick();
        });
    btnInterval.attachClick(
        []()
        {
            if (isDisplayOn)
            {
                if (idxInterval < cntIntervals - 1)
                    idxInterval++;
                anyIntervalClick();
            }
            else
                display();
        });
    btnInterval.attachDoubleClick(
        []()
        {
            if (idxInterval > 0)
                idxInterval--;
            anyIntervalClick();
        });
    btnInterval.attachLongPressStart(
        []()
        {
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
    if (isDisplayOn && millis() > msDisplayed + 5 * SEC)
    {
        isDisplayOn = false;
        disp.clearDisplay();
        disp.setPowerSave(1);
    }

    btnInterval.tick();
    btnPwm.tick();
}
