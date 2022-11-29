#include "Calendar/DrawCalendar.h"

#include "ui/drawHeader.h"
#include "ui/drawDebug.h"

#include "ui/ui.h"
#include "homeplate.h"
#include "Calendar/CalendarData.h"
#include "utils/network.h"
#include <TimeLib.h>
#include "SdFat.h"

HTTPClient http;
SdFile file;

DrawCalendar::DrawCalendar()
{
    CalendarData *oneCallClient = new CalendarData();

    oneCallClient->update(&this->data, CalendarToken);
    delete oneCallClient;
    oneCallClient = nullptr;
    Serial.println("[Calendar]: loaded data");
}

uint16_t DrawCalendar::getYPosForTime(uint8_t hour, uint8_t min)
{
    // 15min = 10px
    auto start = 205; // start for 7:00
    if (hour < 7)
    {
        return start - 20;
    }

    if (hour > 21)
    {
        return 825 - 20;
    }

    return start + 40 * (hour - 6);
}

void DrawCalendar::drawGrid()
{

    for (auto i = 1; i <= 7; i++)
    {
        display.drawFastVLine(15 + 50 + 160 * i, 0, 815, BLACK);
    }

    for (auto i = 1; i <= 15; i++)
    {
        display.drawFastHLine(60, 35 + 170 + 40 * i, 1125, BLACK);
        display.setFont(&Roboto_16);
        char timeStringBuf[6]; // 13:37
        snprintf(timeStringBuf, 6, "%02d:00", 6 + i);
        auto y = this->getYPosForTime(i + 6, 0);
        centerTextY(timeStringBuf, 10, y, y);
    }
}

void DrawCalendar::drawCalendar()
{
    displayStart();

    display.selectDisplayMode(INKPLATE_3BIT);
    display.setTextColor(BLACK, WHITE); // Set text color to black on white

    this->drawGrid();

    Serial.println(this->data.daily[0].summary);

    displayEnd();

    displayStart();

    drawHeader();
    drawDebugCal();
    display.display();

    displayEnd();
}

void displayCalendar()
{
    auto drawCalendar = new DrawCalendar();

    drawCalendar->drawCalendar();
}