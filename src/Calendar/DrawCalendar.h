#pragma once
#include "Arduino.h"
#include "Calendar/CalendarData.h"

class DrawCalendar
{
private:
    CalendarDataType data;

public:
    DrawCalendar(String path);

    void drawCalendar();
};

void displayCalendar();
