

#include "Weather/OpenWeather.h"

class DrawWeather {
private:

    OpenWeatherMapOneCallData data;

    void drawCurrentWeather();

    void drawCurrentTemp();

public:
    DrawWeather();

    void drawWeather();


};
