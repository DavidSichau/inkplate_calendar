#pragma once
#include "Arduino.h"
#include "Calendar/CalendarData.h"

class DrawCalendar
{
private:
    CalendarDataType data;

    void drawGrid();

    uint16_t getYPosForTime(uint8_t hour, uint8_t min);

public:
    DrawCalendar();

    void drawCalendar();
};

void displayCalendar();
