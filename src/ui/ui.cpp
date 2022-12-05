#include "ui.h"
#include "homeplate.h"

uint16_t getTextHeight(const char *t)
{
    int16_t x1b, y1b;
    uint16_t w, h;
    // y = n to give plenty of room for text to clear height of screen
    display.getTextBounds(t, 0, 100, &x1b, &y1b, &w, &h);

    return h;
}

uint16_t getTextWidth(const char *t)
{
    int16_t x1b, y1b;
    uint16_t w, h;
    // y = n to give plenty of room for text to clear height of screen
    display.getTextBounds(t, 0, 100, &x1b, &y1b, &w, &h);

    return w;
}

// returns height
// y value should be the top of the text location
uint16_t centerTextX(const char *t, int16_t x1, int16_t x2, int16_t y)
{
    // center text
    int16_t x1b, y1b;
    uint16_t w, h;
    // y = n to give plenty of room for text to clear height of screen
    display.getTextBounds(t, 0, 100, &x1b, &y1b, &w, &h);

    int16_t x = ((x2 - x1) - w) / 2 + x1;

    display.setCursor(x, y + h);
    display.print(t);

    return h;
}

// returns width
// y value should be the top of the text location
uint16_t centerTextY(const char *t, int16_t x, int16_t y1, int16_t y2)
{
    // center text
    int16_t x1b, y1b;
    uint16_t w, h;
    // y = n to give plenty of room for text to clear height of screen
    display.getTextBounds(t, 0, 100, &x1b, &y1b, &w, &h);

    int16_t y = ((y2 - y1) - h) / 2 + y1;

    display.setCursor(x, y + h);
    display.print(t);

    return w;
}

uint16_t centerTextLeftMiddleRight(const char *left, const char *middle, const char *right, int16_t x1, int16_t x2, int16_t y)
{
    // center text
    int16_t x1b, y1b;
    uint16_t wr, wm, h, ignore;

    auto full_text = new char[strlen(left) + strlen(middle) + strlen(right) + 1];
    strcpy(full_text, left);
    strcat(full_text, middle);
    strcat(full_text, right);

    display.getTextBounds(full_text, 0, 100, &x1b, &y1b, &ignore, &h);
    display.getTextBounds(middle, 0, 100, &x1b, &y1b, &wm, &ignore);
    display.getTextBounds(right, 0, 100, &x1b, &y1b, &wr, &ignore);

    int16_t x = ((x2 - x1) - wm) / 2 + x1;

    delete full_text;

    display.setCursor(x1, y + h);
    display.print(left);
    display.setCursor(x, y + h);
    display.print(middle);

    display.setCursor(x2 - wr, y + h);
    display.print(right);

    return h;
}
