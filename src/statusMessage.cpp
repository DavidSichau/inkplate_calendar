#include "homeplate.h"
#include <libs/pngle/pngle.h>
#include <utils/time.h>

#define max(x, y) (((x) >= (y)) ? (x) : (y))
void displayStats()
{
    displayStart();
    display.setTextColor(C_BLACK, C_WHITE); // Set text color to black on white
    display.setFont(&Roboto_12);
    display.setTextSize(1);
    // display status message
    display.setCursor(1155, 820);

    // text to print over box
    display.printf("[%s]", timeString());
    displayEnd();
}


// NOTE I2C & display locks MUST NOT be held by caller.
void displayStatusMessage(const char *format, ...)
{
    static char statusBuffer[100];
    // setup format string
    va_list argptr;
    va_start(argptr, format);
    vsnprintf(statusBuffer, 100, format, argptr);
    va_end(argptr);

    Serial.printf("[STATUS] %s\n", statusBuffer);

    i2cStart();
    displayStart();
    display.selectDisplayMode(INKPLATE_1BIT);
    display.setTextColor(BLACK, WHITE); // Set text color to black on white
    display.setFont(&Roboto_16);
    display.setTextSize(1);

    const int16_t pad = 3;           // padding
    const int16_t mar = 5;           // margin
    const int16_t statusWidth = 400; // extra space to clear for text
    const int16_t x = mar;
    const int16_t y = E_INK_HEIGHT - mar;

    // get text size for box
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(statusBuffer, x, y, &x1, &y1, &w, &h);

    // background box to set internal buffer colors
    display.fillRect(x - pad, y - pad - h, max(w + (pad * 2), statusWidth), h + (pad * 2), WHITE);
    // Serial.printf("fillRect(x:%u, y:%u, w:%u, h:%u)\n", x-pad, y-pad-h, max(w+(pad*2), 400), h+(pad*2));
    display.partialUpdate(sleepBoot);

    // display status message
    display.setCursor(x, y);

    // text to print over box
    display.print(statusBuffer);
    display.partialUpdate(sleepBoot);
    displayEnd();
    i2cEnd();
}
