#include "Calendar/DrawCalendar.h"

#include "ui/drawHeader.h"
#include "ui/drawDebug.h"

#include "ui/ui.h"
#include "homeplate.h"
#include "Calendar/CalendarData.h"
#include "utils/network.h"
#include "utils/time.h"

#include <TimeLib.h>
#include <array>

HTTPClient http;

DrawCalendar::DrawCalendar()
{
    CalendarData *oneCallClient = new CalendarData();

    this->currentTime = getNowL();

    oneCallClient->update(&this->data, this->currentTime);
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

    if (hour > 22)
    {
        return 825 - 20;
    }

    return start + 40 * (hour - 6) + 10 * (min / 15);
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
        display.drawFastHLine(60, 35 + 170 + 40 * i, 1125, 6);
        display.setFont(&FreeSans9pt7b);
        char timeStringBuf[6]; // 13:37
        snprintf(timeStringBuf, 6, "%02d:00", 6 + i);
        auto y = this->getYPosForTime(i + 6, 0);
        centerTextY(timeStringBuf, 10, y, y);
    }
}

void DrawCalendar::drawDaily()
{

    std::array<int, 8> dailyEvents = {};

    for (auto &daily : this->data.daily)
    {

        auto date = daily.date;
        auto summary = daily.summary;
        auto currentDay = getWeekdayL(date);

        dailyEvents[currentDay - 1]++;

        GFXcanvas1 canvas(159, 19); // Create canvas object
        canvas.drawRoundRect(0, 0, 159, 19, 4, BLACK);
        canvas.setCursor(5, 9);
        canvas.setFont(&FreeSans9pt7b);
        canvas.print(daily.summary);
        display.drawBitmap(getXPosForWeekday(currentDay - 1) + 1, 120 + dailyEvents[currentDay - 1] * 20, canvas.getBuffer(), 159, 19, BLACK);
    }
}

void DrawCalendar::drawEvents()
{

    auto prevEnd = this->data.events.begin()->end;
    auto currentIntend = 0;
    for (auto &events : this->data.events)
    {

        auto start = events.start;
        auto end = events.end;

        auto summary = events.summary;
        auto currentDay = getWeekdayL(start);

        auto eventStartY = this->getYPosForTime(getHour(start), minute(start));
        auto eventEndY = this->getYPosForTime(getHour(end), minute(end));
        auto eventLength = eventEndY - eventStartY - 1;
        auto intend = 0;
        if (start < prevEnd)
        {
            currentIntend += 20;
        }
        else if (currentIntend != 0)
        {
            currentIntend -= 20;
        }

        GFXcanvas1 canvas(159 - currentIntend, eventLength); // Create canvas object
        canvas.drawRoundRect(0, 0, 159 - currentIntend, eventLength, 4, BLACK);

        canvas.setTextColor(BLACK);
        canvas.setCursor(5, 9);
        canvas.setFont(&FreeSans9pt7b);
        canvas.setTextWrap(false);
        canvas.print(events.summary);
        if (eventLength > 35)
        {
            canvas.setCursor(5, 33);
            canvas.printf("%02d:%02d - %02d:%02d", getHour(start), minute(start), getHour(end), minute(end));
        }
        display.fillRoundRect(getXPosForWeekday(currentDay - 1) + 1 + currentIntend, eventStartY, 159 - currentIntend, eventLength, 4, 6); // 7 white
        display.drawBitmap(getXPosForWeekday(currentDay - 1) + 1 + currentIntend, eventStartY, canvas.getBuffer(), 159 - currentIntend, eventLength, BLACK);
        prevEnd = end;
    }
}

void DrawCalendar::drawCalendar()
{
    displayStart();
    display.clearDisplay();
    display.selectDisplayMode(INKPLATE_3BIT);
    display.setTextColor(BLACK, WHITE); // Set text color to black on white

    this->drawGrid();
    this->drawDaily();
    this->drawEvents();

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