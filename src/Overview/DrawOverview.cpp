#include "Weather/OpenWeather.h"
#include "Weather/SunMoonCalc.h"

#include "ui/drawHeader.h"
#include "ui/drawDebug.h"

#include "Overview/DrawOverview.h"
#include "ui/ui.h"
#include "homeplate.h"
#include <Timezone.h>
#include <utils/time.h>

#include "SHTC3-SOLDERED.h"

SHTC3 shtcSensorO;

DrawOverview::DrawOverview()
{
  OpenWeatherMapOneCall *oneCallClient = new OpenWeatherMapOneCall();
  oneCallClient->setLanguage(OPEN_WEATHER_MAP_LANGUAGE);

  oneCallClient->update(&this->weatherData);
  delete oneCallClient;
  oneCallClient = nullptr;

  CalendarData *calenderDataCall = new CalendarData();

  this->currentTime = getNowL();

  calenderDataCall->update(&this->calendarData, this->currentTime, true);
  delete calenderDataCall;

  calenderDataCall = nullptr;

  Serial.println("[Overview]: loaded data");

  shtcSensorO.begin();
  shtcSensorO.sample();
  this->sensorTemperature = shtcSensorO.readTempC();
  this->sensorHumidity = shtcSensorO.readHumidity();
  shtcSensorO.sleep();
}

uint16_t DrawOverview::getYPosForTime(uint8_t hour, uint8_t min)
{
  auto start = 205;
  if (hour < 7)
  {
    return start - 20;
  }

  if (hour > 22)
  {
    return 825 - 20;
  }

  return start + 40 * (hour - 6) + 10 * (min / 15);
}

uint16_t DrawOverview::getXPosForWeekday(uint8_t day)
{
  auto start = 65;

  return start + 800 + 160 * day;
}

void DrawOverview::drawGrid()
{
  for (auto i = 0; i < 2; i++)
  {
    auto current = changeTimeByDay(i, now());
    auto currentDay = day(current);

    auto today = day();
    display.drawFastVLine(15 + 850 + 160 * i, 105, 720, BLACK);
    if (today != currentDay)
    {
      display.setFont(&FreeSans12pt7b);
    }
    else
    {
      display.setFont(&FreeSansBold12pt7b);
    }

    auto weekDay = (weekday(current) - 2) % 7;
    centerTextX(lang_day[1][weekDay], getXPosForWeekday(i), getXPosForWeekday(i) + 160, 45);

    char dayChar[3];
    snprintf(dayChar, 3, "%02d", currentDay);

    if (today != currentDay)
    {
      display.setFont(&FreeSans18pt7b);
    }
    else
    {
      display.setFont(&FreeSansBold18pt7b);
    }
    centerTextX(dayChar, getXPosForWeekday(i), getXPosForWeekday(i) + 160, 80);
  }

  for (auto i = 1; i <= 15; i++)
  {
    display.drawFastHLine(860, 35 + 170 + 40 * i, 1125, 6);
    display.setFont(&FreeSans9pt7b);
    char timeStringBuf[6];
    snprintf(timeStringBuf, 6, "%02d:00", 6 + i);
    auto y = this->getYPosForTime(i + 6, 0);
    centerTextY(timeStringBuf, 810, y, y);
  }
}

void DrawOverview::drawDaily()
{

  std::array<int, 2> dailyEvents = {};

  for (auto &daily : this->calendarData.daily)
  {

    auto date = daily.date;
    auto summary = daily.summary;
    auto currentDay = getWeekdayL(date);
    GFXcanvas1 canvas(159, 19); // Create canvas object
    canvas.drawRoundRect(0, 0, 159, 19, 4, BLACK);
    canvas.setCursor(5, 9);
    canvas.setFont(&FreeSans9pt7b);
    canvas.print(daily.summary);
    if (day(date) == day())
    {
      dailyEvents[0]++;
      display.drawBitmap(getXPosForWeekday(0) + 1, 120 + dailyEvents[0] * 20, canvas.getBuffer(), 159, 19, BLACK);
    }
    if (day(date) == (day() + 1) || day(date) == 1)
    {
      dailyEvents[1]++;
      display.drawBitmap(getXPosForWeekday(1) + 1, 120 + dailyEvents[1] * 20, canvas.getBuffer(), 159, 19, BLACK);
    }
  }
}

void DrawOverview::drawEvents()
{

  auto prevEnd = 0;
  auto currentIntend = 0;
  for (auto &events : this->calendarData.events)
  {
    auto start = events.start;
    auto end = events.end;

    auto summary = events.summary;
    auto currentDay = getWeekdayL(start);

    auto eventStartY = this->getYPosForTime(getHour(start), minute(start));
    auto eventEndY = this->getYPosForTime(getHour(end), minute(end));
    auto eventLength = eventEndY - eventStartY - 1;
    auto intend = 0;
    if (start < prevEnd)
    {
      currentIntend += 20;
    }
    else if (currentIntend != 0)
    {
      currentIntend = 0;
    }

    GFXcanvas1 canvas(159 - currentIntend, eventLength); // Create canvas object
    canvas.drawRoundRect(0, 0, 159 - currentIntend, eventLength, 4, BLACK);

    canvas.setTextColor(BLACK);
    canvas.setCursor(5, 9);
    canvas.setFont(&FreeSans9pt7b);
    canvas.setTextWrap(false);
    canvas.print(events.summary);
    if (eventLength > 35)
    {
      canvas.setCursor(5, 33);
      canvas.printf("%02d:%02d - %02d:%02d", getHour(start), minute(start), getHour(end), minute(end));
    }
    if (day(start) == day())
    {
      display.fillRoundRect(getXPosForWeekday(0) + 1 + currentIntend, eventStartY, 159 - currentIntend, eventLength, 4, C_GREY_6); // 7 white
      display.drawBitmap(getXPosForWeekday(0) + 1 + currentIntend, eventStartY, canvas.getBuffer(), 159 - currentIntend, eventLength, BLACK);
    }
    if (day(start) == (day() + 1) || day(start) == 1)
    {
      display.fillRoundRect(getXPosForWeekday(1) + 1 + currentIntend, eventStartY, 159 - currentIntend, eventLength, 4, C_GREY_6); // 7 white
      display.drawBitmap(getXPosForWeekday(1) + 1 + currentIntend, eventStartY, canvas.getBuffer(), 159 - currentIntend, eventLength, BLACK);
    }

    prevEnd = end;
  }
}

bool DrawOverview::isNight(u_int32_t now)
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
void DrawOverview::drawCurrentWeather(int x = 15, int y = 35)
{

  displayStart();

  auto png = getPngForWeatherId(this->weatherData.current.weatherId, this->isNight(this->weatherData.current.dt));

  display.drawImage("png/256/" + png + ".png", x + 70, y + 30, false);

  auto smCalc = SunMoonCalc(now(), OPEN_WEATHER_MAP_LOCATTION_LAT, OPEN_WEATHER_MAP_LOCATTION_LON);
  auto sunMoon = smCalc.calculateSunAndMoonData();

  display.drawImage("png/64/" + sunMoon.moon.phase.icon + ".png", x + 10, y + 10, false);

  displayEnd();
}

void DrawOverview::drawCurrentTemp(int x = 395, int y = 35)
{

  displayStart();

  char temp[3];
  sprintf(temp, "%.0f", this->weatherData.current.temp);
  char feelTemp[3];
  sprintf(feelTemp, "%.0f", this->weatherData.current.feels_like);

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
  sprintf(intTemp, "%.0f", this->sensorTemperature);
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

void DrawOverview::drawHourForcast(int x = 15, int y = 295)
{

  displayStart();

  display.setFont(&Roboto_40);

  auto firstColumn = 65;
  auto incrementColumn = 195;

  auto y_internal = y + 40;

  auto skipIndex = 0;

  for (auto i = 0; i < 4; i++)
  {
    auto index = i * 3 + 1 + skipIndex;
    while (getHour(this->weatherData.hourly[index].dt) > 20 || getHour(this->weatherData.hourly[index].dt) < 7)
    {
      skipIndex++;
      index++;
    }

    auto png = getPngForWeatherId(this->weatherData.hourly[index].weatherId, this->isNight(this->weatherData.hourly[index].dt));

    display.drawImage("png/128/" + png + ".png", firstColumn - 10 + incrementColumn * i, y_internal + 55, false);

    display.setCursor(firstColumn + incrementColumn * i, y_internal + 50);
    display.print(timeFromUnixString(this->weatherData.hourly[index].dt));

    char temp[3];
    sprintf(temp, "%.0f", this->weatherData.hourly[index].temp);

    auto w2 = getTextWidth(temp);

    display.drawImage("png/64/wi-celsius.png", firstColumn + 20 + incrementColumn * i + w2 - 12, y_internal + 171, false);
    display.setCursor(firstColumn + 20 + incrementColumn * i, y_internal + 210);
    display.print(temp);
  }

  // display.drawFastHLine(0, y_internal + 210, 1200, BLACK);

  displayEnd();
}

void DrawOverview::drawDayForcast(int x = 15, int y = 555)
{

  displayStart();

  display.setFont(&Roboto_40);

  auto firstColumn = 65;
  auto incrementColumn = 195;

  for (auto i = 0; i < 4; i++)
  {
    auto index = i;
    auto png = getPngForWeatherId(this->weatherData.daily[index].weatherId, false);
    display.drawImage("png/128/" + png + ".png", firstColumn - 10 + incrementColumn * i, y + 55, false);

    display.setCursor(firstColumn + incrementColumn * i + 30, y + 50);
    display.print(getWeekday(this->weatherData.daily[index].dt));

    char minT[5];
    char maxT[5];

    sprintf(minT, "%.0f", this->weatherData.daily[index].tempMin);
    sprintf(maxT, "%.0f", this->weatherData.daily[index].tempMax);

    auto w1 = getTextWidth(minT);
    auto w2 = getTextWidth(maxT);

    display.drawImage("png/64/wi-celsius.png", firstColumn + incrementColumn * i + w2 + w1 + 25, y + 171, false);

    display.setCursor(firstColumn + incrementColumn * i, y + 210);
    display.printf("%s | %s", minT, maxT);
  }

  // display.drawFastHLine(0, y + 210, 1200, BLACK);

  displayEnd();
}

void DrawOverview::drawOverview()
{
  displayStart();

  display.clearDisplay();
  display.selectDisplayMode(INKPLATE_3BIT);
  display.setTextColor(BLACK, WHITE); // Set text color to black on white
  display.clearDisplay();

  displayEnd();

  this->drawCurrentWeather();
  this->drawCurrentTemp();
  this->drawHourForcast();
  this->drawDayForcast(15, 565);

  this->drawGrid();
  this->drawDaily();
  this->drawEvents();

  displayStart();

  drawHeader();
  display.display();

  displayEnd();
}

void displayOverview()
{
  auto drawOverview = new DrawOverview();

  drawOverview->drawOverview();
}