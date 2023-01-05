#pragma once

#include "Weather/OpenWeather.h"
#include "Calendar/CalendarData.h"

class DrawOverview
{
private:
    OpenWeatherMapOneCallData weatherData;

    CalendarDataType calendarData;

    time_t currentTime;

    float sensorTemperature;

    float sensorHumidity;

    void drawCurrentWeather(int x, int y);

    void drawCurrentTemp(int x, int y);

    void drawHourForcast(int x, int y);

    void drawDayForcast(int x, int y);

    bool isNight(u_int32_t now);

    void drawGrid();

    void drawDaily();

    void drawEvents();

    uint16_t getYPosForTime(uint8_t hour, uint8_t min);

    uint16_t getXPosForWeekday(uint8_t day);

public:
    DrawOverview();

    void drawOverview();
};

void displayOverview();
