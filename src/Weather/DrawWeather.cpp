#include "WeatherStationFonts.h"
#include "Weather/OpenWeather.h"
#include "ui/drawHeader.h"
#include "Weather/DrawWeather.h"
#include "ui/ui.h"

#include "homeplate.h"

DrawWeather::DrawWeather()
{
  OpenWeatherMapOneCall *oneCallClient = new OpenWeatherMapOneCall();
  oneCallClient->setLanguage(OPEN_WEATHER_MAP_LANGUAGE);

  oneCallClient->update(&this->data, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATTION_LAT, OPEN_WEATHER_MAP_LOCATTION_LON);
  delete oneCallClient;
  oneCallClient = nullptr;
  Serial.println("[Weather]: loaded data");
}

// draws current weather information
void DrawWeather::drawCurrentWeather()
{

  displayStart();

  //   display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setCursor(50, 50);

  display.setFont(&Meteocons_Regular_42);
  display.println(this->data.current.weatherIconMeteoCon);

  display.setFont(&Roboto_64);
  display.println("Opfikon");

  display.setFont(&Roboto_64);
  //   display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.println(String(this->data.current.temp, 0) + "°C");

  display.setFont(&Roboto_64);
  //   display.setTextAlignment(TEXT_ALIGN_LEFT);
  Serial.println(this->data.current.weatherDescription);
  display.print(this->data.current.weatherDescription);
  display.partialUpdate();

  displayEnd();
}

void DrawWeather::drawCurrentTemp()
{

  displayStart();

  char temp[10];
  sprintf(temp, "%.0f °C", this->data.current.temp);
  char feelTemp[40];
  sprintf(feelTemp, "feelslike: %.0f °C", this->data.current.feels_like);

  display.setFont(&Roboto_200);
  auto h = getTextHeight(temp);
  display.setCursor(405, 35 + h);
  display.print(temp);

  auto h2 = getTextHeight(feelTemp);

  display.setFont(&Roboto_64);
  display.setCursor(405, 35 + h + h2 + 20);
  display.print(feelTemp);
  display.display();

  displayEnd();
}

void DrawWeather::drawWeather()
{
  displayStart();

  display.clearDisplay();
  display.selectDisplayMode(INKPLATE_1BIT);
  display.setTextColor(BLACK, WHITE); // Set text color to black on white
  drawHeader();

  displayEnd();

  this->drawCurrentTemp();
  this->drawCurrentWeather();
}