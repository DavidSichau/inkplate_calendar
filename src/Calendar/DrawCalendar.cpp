#include "Calendar/DrawCalendar.h"

#include "ui/drawHeader.h"
#include "ui/drawDebug.h"

#include "ui/ui.h"
#include "homeplate.h"
#include "Calendar/CalendarData.h"
#include "utils/network.h"
#include "utils/time.h"

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

uint16_t DrawCalendar::getXPosForWeekday(uint8_t day)
{
    // 15min = 10px
    auto start = 65; // start for 7:00

    return start + 160 * day;
}

void DrawCalendar::drawGrid()
{

    auto prevSunday = previousSunday(now());

    for (auto i = 0; i < 7; i++)
    {
        auto current = changeTimeByDay(1 + i, prevSunday);
        auto currentDay = day(current);
        auto today = day();
        display.drawFastVLine(15 + 50 + 160 * i, 105, 720, BLACK);
        if (today != currentDay)
        {
            display.setFont(&FreeSans12pt7b);
        }
        else
        {
            display.setFont(&FreeSansBold12pt7b);
        }

        centerTextX(lang_day[1][i], getXPosForWeekday(i), getXPosForWeekday(i) + 160, 35);

        char dayChar[3];
        snprintf(dayChar, 3, "%02d", currentDay);

        if (today != currentDay)
        {
            display.setFont(&FreeSans18pt7b);
        }
        else
        {
            display.setFont(&FreeSansBold18pt7b);
        }
        centerTextX(dayChar, getXPosForWeekday(i), getXPosForWeekday(i) + 160, 75);
    }

    for (auto i = 1; i <= 15; i++)
    {
        display.drawFastHLine(60, 35 + 170 + 40 * i, 1125, BLACK);
        display.setFont(&FreeSans9pt7b);
        char timeStringBuf[6]; // 13:37
        snprintf(timeStringBuf, 6, "%02d:00", 6 + i);
        auto y = this->getYPosForTime(i + 6, 0);
        centerTextY(timeStringBuf, 10, y, y);
    }
}

void DrawCalendar::drawDaily()
{
    for (auto i = 0; i < 20; i++)
    {
        auto date = this->data.daily[i].date;
        auto summary = this->data.daily[i].summary;
        Serial.println(date);
        }
}

void DrawCalendar::drawCalendar()
{
    displayStart();

    display.selectDisplayMode(INKPLATE_3BIT);
    display.setTextColor(BLACK, WHITE); // Set text color to black on white

    this->drawGrid();
    this->drawDaily();

    Serial.println(this->data.daily[0].summary);

    displayEnd();

    displayStart();

    // drawHeader();
    // drawDebugCal();
    display.display();

    displayEnd();
}

void displayCalendar()
{
    auto drawCalendar = new DrawCalendar();

    drawCalendar->drawCalendar();
}