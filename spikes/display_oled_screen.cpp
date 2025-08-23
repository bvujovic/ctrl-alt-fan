#include <Arduino.h>
#include <U8g2lib.h> // lib_deps = olikraus/U8g2@^2.36.12

U8G2_SSD1306_72X40_ER_F_HW_I2C disp(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/6, /* data=*/5);
char str[6];
int dx, dy;

void displayNums(int a, int b)
{
    sprintf(str, "%d:%d", a, b);
    dx = (disp.getDisplayWidth() - disp.getStrWidth(str)) / 2;
    disp.clearBuffer();
    disp.drawStr(dx, dy, str);
    disp.sendBuffer();
}

int ints[] = {0, 2, 4, 8, 16, 32};

void setup()
{
    disp.begin();
    disp.clearBuffer(); // clear the internal memory
    // disp.setDisplayRotation(U8G2_R2);   // Rotate display 180 degrees (upside down)
    // disp.setFont(u8g2_font_ncenB24_tf);
    disp.setFont(u8g2_font_logisoso30_tf);

    // auto x = disp.drawStr(0, 40, "0:5"); // write something to the internal memory
    // str[0] = '8';
    // str[2] = '2';
    dy = disp.getDisplayHeight() - (disp.getDisplayHeight() - 30) / 2; // 30 is the size of the font - change IN
    dx = (disp.getDisplayWidth() - disp.getStrWidth(str)) / 2;

    // itoa(disp.getHeight(), str, 10);
    itoa(disp.getMaxCharHeight(), str, 10);
    disp.drawStr(dx, dy, str);
    disp.sendBuffer(); // transfer internal memory to the display
    delay(1000);

    // itoa(LWIP_ARRAYSIZE(ints), str, 10);
    // disp.drawStr(dx, dy, str);
    // disp.sendBuffer(); // transfer internal memory to the display
    // delay(1000);
}

int i = -1;

void loop()
{
    delay(1000);
    // if (i++ > 10)
    //     disp.clearDisplay();
    // disp.setPowerSave(1);

    if (++i < LWIP_ARRAYSIZE(ints))
    {
        // itoa(i, str, 10);
        displayNums(5, ints[i]);
    }
    else if (i == LWIP_ARRAYSIZE(ints))
    {
        disp.clearDisplay();
        disp.setPowerSave(1);
    }
}
