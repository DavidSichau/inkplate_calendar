

#include "Weather/OpenWeather.h"

class DrawWeather {
private:

    OpenWeatherMapOneCallData data;

    void drawCurrentWeather();

    void drawCurrentTemp(int x, int y);

    void drawCurrentStats(int x, int y);

public:
    DrawWeather();

    void drawWeather();


};
