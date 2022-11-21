#include "WeatherStationFonts.h"
#include "Weather/OpenWeather.h"
#include "Weather/SunMoonCalc.h"

#include "ui/drawHeader.h"
#include "ui/drawDebug.h"

#include "Weather/DrawWeather.h"
#include "ui/ui.h"
#include "homeplate.h"
#include <Timezone.h>

DrawWeather::DrawWeather()
{
  OpenWeatherMapOneCall *oneCallClient = new OpenWeatherMapOneCall();
  oneCallClient->setLanguage(OPEN_WEATHER_MAP_LANGUAGE);

  oneCallClient->update(&this->data, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATTION_LAT, OPEN_WEATHER_MAP_LOCATTION_LON);
  delete oneCallClient;
  oneCallClient = nullptr;
  Serial.println("[Weather]: loaded data");
}

bool DrawWeather::isNight()
{
  auto now = this->data.current.dt;
  auto sunrise = this->data.current.sunrise;
  auto sunset = this->data.current.sunset;

  if (now > sunrise && now < sunset)
  {
    return false;
  }
  return true;
}

// draws current weather information
void DrawWeather::drawCurrentWeather(int x = 15, int y = 35)
{

  displayStart();

  auto png = getPngForWeatherId(this->data.current.weatherId, this->isNight());

  display.sdCardInit();

  Serial.println("png/256/" + png);

  display.drawImage("png/256/" + png + ".png", x + 40, y - 20, false);

  auto smCalc = SunMoonCalc(now(), OPEN_WEATHER_MAP_LOCATTION_LAT, OPEN_WEATHER_MAP_LOCATTION_LON);
  auto sunMoon = smCalc.calculateSunAndMoonData();

  display.drawImage("png/128/" + sunMoon.moon.phase.icon + ".png", x + 270, y + 140, false);

  displayEnd();
}

void DrawWeather::drawCurrentTemp(int x = 405, int y = 35)
{

  displayStart();

  char temp[3];
  sprintf(temp, "%.0f", abs(this->data.current.temp));
  char feelTemp[3];
  sprintf(feelTemp, "%.0f", abs(this->data.current.feels_like));

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

void DrawWeather::drawCurrentStats(int x = 795, int y = 35)
{

  displayStart();

  display.sdCardInit();

  auto startYImg = 35;

  display.drawImage("png/64/wi-barometer.png", x, startYImg + 65 * 0, false);

  display.drawImage("png/64/wi-day-sunny.png", x + 205, startYImg + 65 * 0, false);

  display.drawImage("png/64/wi-strong-wind.png", x, startYImg + 65 * 1, false);

  display.drawImage("png/64/wi-humidity.png", x + 205, startYImg + 65 * 1, false);

  display.drawImage("png/64/wi-sunrise.png", x, startYImg + 65 * 2, false);

  display.drawImage("png/64/wi-sunset.png", x + 205, startYImg + 65 * 2, false);

  display.drawImage("png/64/wi-raindrops.png", x, startYImg + 65 * 3, false);

  display.drawImage("png/64/wi-umbrella.png", x + 205, startYImg + 65 * 3, false);

  display.setFont(&Roboto_32);

  auto firstColumn = 65;
  auto secondColumn = 275;
  auto startY = 85;

  display.setCursor(x + firstColumn, startY + 65 * 0);
  display.printf("%.0i hPa", this->data.current.pressure);

  display.setCursor(x + secondColumn, startY + 65 * 0);
  display.printf("%.1f", this->data.current.uvi);

  // move to middle
  //  int temp = display.readTemperature();
  //  // TODO get correct temperature and display it with Grad C
  //  display.printf("%dC", temp);

  display.setCursor(x + firstColumn, startY + 65 * 1);
  display.printf("%.0f km/h", abs(this->data.current.windSpeed * 3.6));

  display.setCursor(x + secondColumn, startY + 65 * 1);
  display.printf("%.0i %", abs(this->data.current.humidity));

  display.setCursor(x + firstColumn, startY + 65 * 2);
  display.print(timeFromUnixString(this->data.current.sunrise));

  display.setCursor(x + secondColumn, startY + 65 * 2);
  display.print(timeFromUnixString(this->data.current.sunset));

  display.setCursor(x + firstColumn, startY + 65 * 3);
  display.printf("%.1f mm", abs(this->data.hourly[0].rain));

  display.setCursor(x + secondColumn, startY + 65 * 3);
  display.printf("%.0f %", abs(this->data.hourly[0].pop * 100));

  // display.drawFastVLine(x + 20, 0, 825, BLACK);
  // display.drawFastVLine(x + 195 + 20, 0, 825, BLACK);

  // display.drawFastHLine(0, 100, 1200, BLACK);

  // display.drawFastHLine(0, 165, 1200, BLACK);
  // display.drawFastHLine(0, 230, 1200, BLACK);
  // display.drawFastHLine(0, 285, 1200, BLACK);

  displayEnd();
}

void DrawWeather::drawWeather()
{
  displayStart();

  display.selectDisplayMode(INKPLATE_3BIT);
  display.setTextColor(BLACK, WHITE); // Set text color to black on white

  displayEnd();

  this->drawCurrentWeather();
  this->drawCurrentTemp();
  this->drawCurrentStats();

  displayStart();

  drawHeader();
  drawDebug();
  display.display();

  displayEnd();
}