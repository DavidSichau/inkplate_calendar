#include <qrcode.h>
#include "homeplate.h"
#include "Weather/DrawWeather.h"

void displayWeather()
{

    // displayStart();

    auto drawWeather = new DrawWeather();

    drawWeather->drawWeather();

    // displayEnd();

    // OpenWeatherMapOneCallData openWeatherMapOneCallData;

    // OpenWeatherMapOneCall *oneCallClient = new OpenWeatherMapOneCall();
    // oneCallClient->setLanguage(OPEN_WEATHER_MAP_LANGUAGE);

    // oneCallClient->update(&openWeatherMapOneCallData, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATTION_LAT, OPEN_WEATHER_MAP_LOCATTION_LON);
    // delete oneCallClient;
    // oneCallClient = nullptr;

    // Serial.println( "Current Weather: ");
    // Serial.println( String(openWeatherMapOneCallData.current.temp, 1) + "°C" );
    // Serial.println( openWeatherMapOneCallData.current.weatherDescription );

    // Serial.println( "Forecasts: ");

    // for(int i = 0; i < 5; i++)
    // {
    //     if(openWeatherMapOneCallData.daily[i].dt > 0) {
    //     Serial.println("dt: " + String(openWeatherMapOneCallData.daily[i].dt) );
    //     Serial.println("temp: " + String(openWeatherMapOneCallData.daily[i].tempDay, 1) + "°C" );
    //     Serial.println("desc: " +  openWeatherMapOneCallData.daily[i].weatherDescription);
    //     Serial.println();
    //     }
    // }
}
