#include "WeatherStationFonts.h"
#include "Weather/OpenWeather.h"
#include "ui/drawHeader.h"
#include "ui/drawDebug.h"

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

void DrawWeather::drawCurrentTemp(int x = 405, int y = 35)
{

  displayStart();

  char temp[3];
  sprintf(temp, "%.0f", this->data.current.temp);
  char feelTemp[3];
  sprintf(feelTemp, "%.0f", this->data.current.feels_like);

  display.sdCardInit();

  display.setFont(&Roboto_128);
  auto h = getTextHeight(temp);
  auto w = getTextWidth(temp);
  display.drawImage("png/256/wi-celsius.png", x + w - 30, 165 - 157, false);

  display.setCursor(x + 15, 165);
  display.print(temp);

  display.setFont(&Roboto_64);
  auto h2 = getTextHeight(feelTemp);
  auto w2 = getTextWidth(feelTemp);

  display.drawImage("png/128/wi-celsius.png", x + w2 + 40, 207, false);
  display.drawImage("png/64/wi-thermometer-exterior.png", x + 5, 233, false);
  display.setCursor(x + 20 + 45, 285);
  display.print(feelTemp);

  // display.drawFastHLine(0, 165, 1200, BLACK);
  // display.drawFastVLine(x + 20, 0, 825, BLACK);
  // display.drawFastHLine(0, 285, 1200, BLACK);

  displayEnd();
}

void DrawWeather::drawWeather()
{
  displayStart();

  display.selectDisplayMode(INKPLATE_3BIT);
  display.setTextColor(BLACK, WHITE); // Set text color to black on white
  drawHeader();
  drawDebug();

  displayEnd();

  this->drawCurrentTemp();
  // this->drawCurrentWeather();

  displayStart();

  drawHeader();
  drawDebug();
  display.display();

  displayEnd();
}