#pragma once
#include "Arduino.h"
#include "Calendar/CalendarData.h"

class DrawCalendar
{
private:
    CalendarDataType data;

    time_t currentTime;

    void drawGrid();

    void drawDaily();

    void drawEvents();

    uint16_t getYPosForTime(uint8_t hour, uint8_t min);

    uint16_t getXPosForWeekday(uint8_t day);

public:
    DrawCalendar();

    void drawCalendar();
};

void displayCalendar();
