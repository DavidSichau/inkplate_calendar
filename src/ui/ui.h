#include <Arduino.h>

uint16_t centerTextX(const char *t, int16_t x1, int16_t x2, int16_t y, bool lock);

uint16_t centerTextLeftMiddleRight(const char *left, const char *middle, const char *right, int16_t x1, int16_t x2, int16_t y, bool lock);

uint16_t getTextHeight(const char *t);

uint16_t getTextWidth(const char *t);