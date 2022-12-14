#pragma once

#include "Weather/OpenWeather.h"

class DrawWeather
{
private:
    OpenWeatherMapOneCallData data;

    float sensorTemperature;

    float sensorHumidity;

    void drawCurrentWeather(int x, int y);

    void drawCurrentTemp(int x, int y);

    void drawCurrentStats(int x, int y);

    void drawHourForcast(int x, int y);

    void drawDayForcast(int x, int y);

    bool isNight(u_int32_t now);

public:
    DrawWeather();

    void drawWeather();
};

void displayWeather();
