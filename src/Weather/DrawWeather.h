

#include "Weather/OpenWeather.h"

class DrawWeather {
private:

    OpenWeatherMapOneCallData data;

    void drawCurrentWeather(int x, int y);

    void drawCurrentTemp(int x, int y);

    void drawCurrentStats(int x, int y);

    bool isNight();

public:
    DrawWeather();

    void drawWeather();


};
