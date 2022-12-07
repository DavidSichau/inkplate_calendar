#include "Weather/OpenWeather.h"
#include "Weather/SunMoonCalc.h"

#include "ui/drawHeader.h"
#include "ui/drawDebug.h"

#include "Weather/DrawWeather.h"
#include "ui/ui.h"
#include "homeplate.h"
#include <Timezone.h>
#include <utils/time.h>

DrawWeather::DrawWeather()
{
  OpenWeatherMapOneCall *oneCallClient = new OpenWeatherMapOneCall();
  oneCallClient->setLanguage(OPEN_WEATHER_MAP_LANGUAGE);

  oneCallClient->update(&this->data, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATTION_LAT, OPEN_WEATHER_MAP_LOCATTION_LON);
  delete oneCallClient;
  oneCallClient = nullptr;
  Serial.println("[Weather]: loaded data");
}

bool DrawWeather::isNight(u_int32_t now)
{
  auto smCalc = SunMoonCalc(now, OPEN_WEATHER_MAP_LOCATTION_LAT, OPEN_WEATHER_MAP_LOCATTION_LON);
  auto sunMoon = smCalc.calculateSunAndMoonData();

  auto sunrise = sunMoon.sun.rise;
  auto sunset = sunMoon.sun.set;

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

  auto png = getPngForWeatherId(this->data.current.weatherId, this->isNight(this->data.current.dt));

  display.drawImage("png/256/" + png + ".png", x + 70, y + 30, false);

  auto smCalc = SunMoonCalc(now(), OPEN_WEATHER_MAP_LOCATTION_LAT, OPEN_WEATHER_MAP_LOCATTION_LON);
  auto sunMoon = smCalc.calculateSunAndMoonData();

  display.drawImage("png/64/" + sunMoon.moon.phase.icon + ".png", x + 10, y + 10, false);

  displayEnd();
}

void DrawWeather::drawCurrentTemp(int x = 405, int y = 35)
{

  displayStart();

  char temp[3];
  sprintf(temp, "%.0f", abs(this->data.current.temp));
  char feelTemp[3];
  sprintf(feelTemp, "%.0f", abs(this->data.current.feels_like));

  display.setFont(&Roboto_128);
  auto h = getTextHeight(temp);
  auto w = getTextWidth(temp);
  display.drawImage("png/256/wi-celsius.png", x + w - 30, 165 - 157, false);

  display.setCursor(x + 15, 165);
  display.print(temp);

  display.setFont(&Roboto_64);
  auto h2 = getTextHeight(feelTemp);
  auto w2 = getTextWidth(feelTemp);

  char intTemp[3];
  sprintf(intTemp, "%.0d", display.readTemperature());
  auto w3 = getTextWidth(intTemp);

  display.drawImage("png/128/wi-celsius.png", x + w2 + 40, 207, false);
  display.drawImage("png/128/wi-celsius.png", x + 230 + w3, 207, false);

  display.drawImage("png/64/wi-thermometer-exterior.png", x + 5, 233, false);
  display.drawImage("png/64/wi-thermometer-internal.png", x + 185, 233, false);

  display.setCursor(x + 20 + 45, 285);
  display.print(feelTemp);

  // move to middle
  display.setCursor(x + 245, 285);

  // TODO get correct temperature and display it with Grad C
  display.print(intTemp);

  // display.drawFastHLine(0, 165, 1200, BLACK);
  // display.drawFastVLine(x + 20, 0, 825, BLACK);
  // display.drawFastHLine(0, 285, 1200, BLACK);

  displayEnd();
}

void DrawWeather::drawCurrentStats(int x = 795, int y = 35)
{

  displayStart();

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
  display.printf("%.1f", this->data.daily[0].uvi);

  display.setCursor(x + firstColumn, startY + 65 * 1);
  display.printf("%.0f km/h", abs(this->data.current.windSpeed * 3.6));

  display.setCursor(x + secondColumn, startY + 65 * 1);
  display.printf("%.0i %%", abs(this->data.current.humidity));

  display.setCursor(x + firstColumn, startY + 65 * 2);
  display.print(timeFromUnixString(this->data.current.sunrise));

  display.setCursor(x + secondColumn, startY + 65 * 2);
  display.print(timeFromUnixString(this->data.current.sunset));

  display.setCursor(x + firstColumn, startY + 65 * 3);
  display.printf("%.1f mm", abs(this->data.daily[0].rain));

  display.setCursor(x + secondColumn, startY + 65 * 3);
  display.printf("%.0f %%", abs(this->data.daily[0].pop * 100));

  // display.drawFastVLine(x + 20, 0, 825, BLACK);
  // display.drawFastVLine(x + 195 + 20, 0, 825, BLACK);

  // display.drawFastHLine(0, 100, 1200, BLACK);

  // display.drawFastHLine(0, 165, 1200, BLACK);
  // display.drawFastHLine(0, 230, 1200, BLACK);
  // display.drawFastHLine(0, 285, 1200, BLACK);

  displayEnd();
}

void DrawWeather::drawHourForcast(int x = 15, int y = 295)
{

  displayStart();

  display.setFont(&Roboto_40);

  auto firstColumn = 65;
  auto incrementColumn = 195;

  auto y_internal = y + 40;

  auto skipIndex = 0;

  for (auto i = 0; i < 6; i++)
  {
    auto index = i * 2 + 1 + skipIndex;
    while (getHour(this->data.hourly[index].dt) > 20 || getHour(this->data.hourly[index].dt) < 7)
    {
      skipIndex++;
      index++;
    }

    auto png = getPngForWeatherId(this->data.hourly[index].weatherId, this->isNight(this->data.hourly[index].dt));

    display.drawImage("png/128/" + png + ".png", firstColumn - 10 + incrementColumn * i, y_internal + 55, false);

    display.setCursor(firstColumn + incrementColumn * i, y_internal + 50);
    display.print(timeFromUnixString(this->data.hourly[index].dt));

    char temp[3];
    sprintf(temp, "%.0f", this->data.hourly[index].temp);

    auto w2 = getTextWidth(temp);

    display.drawImage("png/64/wi-celsius.png", firstColumn + 20 + incrementColumn * i + w2 - 12, y_internal + 171, false);
    display.setCursor(firstColumn + 20 + incrementColumn * i, y_internal + 210);
    display.print(temp);
  }

  // display.drawFastHLine(0, y_internal + 210, 1200, BLACK);

  displayEnd();
}

void DrawWeather::drawDayForcast(int x = 15, int y = 555)
{

  displayStart();

  display.setFont(&Roboto_40);

  auto firstColumn = 65;
  auto incrementColumn = 195;

  for (auto i = 0; i < 6; i++)
  {
    auto index = i;
    auto png = getPngForWeatherId(this->data.daily[index].weatherId, false);
    display.drawImage("png/128/" + png + ".png", firstColumn - 10 + incrementColumn * i, y + 55, false);

    display.setCursor(firstColumn + incrementColumn * i + 30, y + 50);
    display.print(getWeekday(this->data.daily[index].dt));

    char minT[5];
    char maxT[5];

    sprintf(minT, "%.0f", this->data.daily[index].tempMin);
    sprintf(maxT, "%.0f", this->data.daily[index].tempMax);

    auto w1 = getTextWidth(minT);
    auto w2 = getTextWidth(maxT);

    display.drawImage("png/64/wi-celsius.png", firstColumn + incrementColumn * i + w2 + w1 + 25, y + 171, false);

    display.setCursor(firstColumn + incrementColumn * i, y + 210);
    display.printf("%s | %s", minT, maxT);
  }

  // display.drawFastHLine(0, y + 210, 1200, BLACK);

  displayEnd();
}

void DrawWeather::drawWeather()
{
  displayStart();

  display.clearDisplay();
  display.selectDisplayMode(INKPLATE_3BIT);
  display.setTextColor(BLACK, WHITE); // Set text color to black on white

  displayEnd();

  this->drawCurrentWeather();
  this->drawCurrentTemp();
  this->drawCurrentStats();
  this->drawHourForcast();
  this->drawDayForcast(15, 565);

  displayStart();

  drawHeader();
  // drawDebug();
  display.display();

  displayEnd();
}

void displayWeather()
{
  auto drawWeather = new DrawWeather();

  drawWeather->drawWeather();
}