/**The MIT License (MIT)

 Copyright (c) 2020 by Chris Klinger

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include <WiFi.h>
#include <WiFiClient.h>
#include "Weather/OpenWeather.h"
#include "homeplate.h"
#include "utils/network.h"
#include "Network/loadData.h"

String PATH_SEPERATOR = "/";

OpenWeatherMapOneCall::OpenWeatherMapOneCall()
{
}

void OpenWeatherMapOneCall::update(OpenWeatherMapOneCallData *data)
{
  this->weatherItemCounter = 0;
  this->hourlyItemCounter = 0;
  this->dailyItemCounter = 0;
  this->data = data;
  JsonStreamingParser parser;
  parser.setListener(this);

  auto weatherPath = "/data/2.5/onecall?appid=" + (String)OPEN_WEATHER_MAP_APP_ID + "&lat=" + OPEN_WEATHER_MAP_LOCATTION_LAT + "&lon=" + OPEN_WEATHER_MAP_LOCATTION_LON + "&units=metric&lang=" + OPEN_WEATHER_MAP_LANGUAGE;
  getData(weatherPath, "api.openweathermap.org", 443, parser);

  this->data = nullptr;
  Serial.println("[Weather] finished updating weather");
}

void OpenWeatherMapOneCall::whitespace(char c)
{
}

void OpenWeatherMapOneCall::startDocument()
{
  this->hourlyItemCounter = 0;
}

void OpenWeatherMapOneCall::key(String key)
{
  currentKey = key;
}

void OpenWeatherMapOneCall::value(String value)
{
  // "lon": 8.54, float lon;
  if (currentKey == "lon")
  {
    this->data->lon = value.toFloat();
  }
  // "lat": 47.37 float lat;
  if (currentKey == "lat")
  {
    this->data->lat = value.toFloat();
  }
  // "timezone": "Europe/Berlin"
  if (currentKey == "timezone")
  {
    this->data->timezone = value;
  }

  // "current": {..}
  if (currentParent.startsWith("/ROOT/current"))
  {
    // "current": {.. "dt": 1587216739, .. }
    if (currentKey == "dt")
    {
      this->data->current.dt = value.toInt();
    }
    if (currentKey == "sunrise")
    {
      this->data->current.sunrise = value.toInt();
    }
    if (currentKey == "sunset")
    {
      this->data->current.sunset = value.toInt();
    }
    if (currentKey == "temp")
    {
      this->data->current.temp = value.toFloat();
    }
    if (currentKey == "feels_like")
    {
      this->data->current.feels_like = value.toFloat();
    }
    if (currentKey == "pressure")
    {
      this->data->current.pressure = value.toInt();
    }
    if (currentKey == "humidity")
    {
      this->data->current.humidity = value.toInt();
    }
    if (currentKey == "dew_point")
    {
      this->data->current.dew_point = value.toFloat();
    }
    if (currentKey == "uvi")
    {
      this->data->current.uvi = value.toFloat();
    }
    if (currentKey == "clouds")
    {
      this->data->current.clouds = value.toInt();
    }
    if (currentKey == "visibility")
    {
      this->data->current.visibility = value.toInt();
    }
    if (currentKey == "wind_speed")
    {
      this->data->current.windSpeed = value.toFloat();
    }
    if (currentKey == "wind_deg")
    {
      this->data->current.windDeg = value.toFloat();
    }
    // weatherItemCounter: only get the first item if more than one is available
    if (currentParent.startsWith("/ROOT/current/weather[]") && weatherItemCounter == 0)
    {
      if (currentKey == "id")
      {
        this->data->current.weatherId = value.toInt();
      }
      if (currentKey == "main")
      {
        this->data->current.weatherMain = value;
      }
      if (currentKey == "description")
      {
        this->data->current.weatherDescription = value;
      }
      if (currentKey == "icon")
      {
        this->data->current.weatherIcon = value;
      }
    }
  }

  // "hourly": [..]
  if (currentParent.startsWith("/ROOT/hourly[]"))
  {
    if (currentKey == "dt")
    {
      this->data->hourly[hourlyItemCounter].dt = value.toInt();
    }
    if (currentKey == "temp")
    {
      this->data->hourly[hourlyItemCounter].temp = value.toFloat();
    }
    if (currentKey == "feels_like")
    {
      this->data->hourly[hourlyItemCounter].feels_like = value.toFloat();
    }
    if (currentKey == "pressure")
    {
      this->data->hourly[hourlyItemCounter].pressure = value.toInt();
    }
    if (currentKey == "humidity")
    {
      this->data->hourly[hourlyItemCounter].humidity = value.toInt();
    }
    if (currentKey == "dew_point")
    {
      this->data->hourly[hourlyItemCounter].dew_point = value.toFloat();
    }
    if (currentKey == "clouds")
    {
      this->data->hourly[hourlyItemCounter].clouds = value.toInt();
    }
    if (currentKey == "wind_speed")
    {
      this->data->hourly[hourlyItemCounter].windSpeed = value.toFloat();
    }
    if (currentKey == "wind_deg")
    {
      this->data->hourly[hourlyItemCounter].windDeg = value.toFloat();
    }
    if (currentKey == "pop")
    {
      this->data->hourly[hourlyItemCounter].pop = value.toFloat();
    }
    if (currentParent.startsWith("/ROOT/hourly[]/_obj/rain"))
    {
      if (currentKey == "1h")
      {
        this->data->hourly[hourlyItemCounter].rain = value.toFloat();
      }
    }

    // weatherItemCounter: only get the first item if more than one is available
    if (currentParent.startsWith("/ROOT/hourly[]/_obj/weather[]") && weatherItemCounter == 0)
    {
      if (currentKey == "id")
      {
        this->data->hourly[hourlyItemCounter].weatherId = value.toInt();
      }
      if (currentKey == "main")
      {
        this->data->hourly[hourlyItemCounter].weatherMain = value;
      }
      if (currentKey == "description")
      {
        this->data->hourly[hourlyItemCounter].weatherDescription = value;
      }
      if (currentKey == "icon")
      {
        this->data->hourly[hourlyItemCounter].weatherIcon = value;
      }
    }
  }

  // "daily": [..]
  if (currentParent.startsWith("/ROOT/daily[]"))
  {
    if (currentKey == "dt")
    {
      this->data->daily[dailyItemCounter].dt = value.toInt();
    }
    if (currentKey == "sunrise")
    {
      this->data->daily[dailyItemCounter].sunrise = value.toInt();
    }
    if (currentKey == "sunset")
    {
      this->data->daily[dailyItemCounter].sunset = value.toInt();
    }
    if (currentKey == "pressure")
    {
      this->data->daily[dailyItemCounter].pressure = value.toInt();
    }
    if (currentKey == "humidity")
    {
      this->data->daily[dailyItemCounter].humidity = value.toInt();
    }
    if (currentKey == "dew_point")
    {
      this->data->daily[dailyItemCounter].dew_point = value.toFloat();
    }
    if (currentKey == "wind_speed")
    {
      this->data->daily[dailyItemCounter].windSpeed = value.toFloat();
    }
    if (currentKey == "wind_deg")
    {
      this->data->daily[dailyItemCounter].windDeg = value.toFloat();
    }
    if (currentKey == "clouds")
    {
      this->data->daily[dailyItemCounter].clouds = value.toInt();
    }
    if (currentKey == "rain")
    {
      this->data->daily[dailyItemCounter].rain = value.toFloat();
    }
    if (currentKey == "snow")
    {
      this->data->daily[dailyItemCounter].snow = value.toFloat();
    }
    if (currentKey == "uvi")
    {
      this->data->daily[dailyItemCounter].uvi = value.toFloat();
    }
    if (currentKey == "pop")
    {
      this->data->daily[dailyItemCounter].pop = value.toFloat();
    }

    if (currentParent.startsWith("/ROOT/daily[]/_obj/temp"))
    {
      if (currentKey == "day")
      {
        this->data->daily[dailyItemCounter].tempDay = value.toFloat();
      }
      if (currentKey == "min")
      {
        this->data->daily[dailyItemCounter].tempMin = value.toFloat();
      }
      if (currentKey == "max")
      {
        this->data->daily[dailyItemCounter].tempMax = value.toFloat();
      }
      if (currentKey == "night")
      {
        this->data->daily[dailyItemCounter].tempNight = value.toFloat();
      }
      if (currentKey == "eve")
      {
        this->data->daily[dailyItemCounter].tempEve = value.toFloat();
      }
      if (currentKey == "morn")
      {
        this->data->daily[dailyItemCounter].tempMorn = value.toFloat();
      }
    }

    if (currentParent.startsWith("/ROOT/daily[]/_obj/feels_like"))
    {
      if (currentKey == "day")
      {
        this->data->daily[dailyItemCounter].feels_likeDay = value.toFloat();
      }
      if (currentKey == "night")
      {
        this->data->daily[dailyItemCounter].feels_likeNight = value.toFloat();
      }
      if (currentKey == "eve")
      {
        this->data->daily[dailyItemCounter].feels_likeEve = value.toFloat();
      }
      if (currentKey == "morn")
      {
        this->data->daily[dailyItemCounter].feels_likeMorn = value.toFloat();
      }
    }
    // weatherItemCounter: only get the first item if more than one is available
    if (currentParent.startsWith("/ROOT/daily[]/_obj/weather[]") && weatherItemCounter == 0)
    {
      if (currentKey == "id")
      {
        this->data->daily[dailyItemCounter].weatherId = value.toInt();
      }
      if (currentKey == "main")
      {
        this->data->daily[dailyItemCounter].weatherMain = value;
      }
      if (currentKey == "description")
      {
        this->data->daily[dailyItemCounter].weatherDescription = value;
      }
      if (currentKey == "icon")
      {
        this->data->daily[dailyItemCounter].weatherIcon = value;
      }
    }
  }
}

void OpenWeatherMapOneCall::endArray()
{
  currentKey = "";
  currentParent = currentParent.substring(0, currentParent.lastIndexOf(PATH_SEPERATOR));
}

void OpenWeatherMapOneCall::startObject()
{
  if (currentKey == "")
  {
    currentKey = "_obj";
  }
  currentParent += PATH_SEPERATOR + currentKey;
}

void OpenWeatherMapOneCall::endObject()
{
  if (currentParent == "/ROOT/current/weather[]/_obj" || currentParent == "/ROOT/daily[]/_obj/weather[]/_obj" || currentParent == "/ROOT/daily[]/_obj/weather[]/_obj")
  {
    weatherItemCounter++;
  }
  if (currentParent == "/ROOT/hourly[]/_obj")
  {
    // ensure that it is set if no value is provided
    if (abs(this->data->hourly[hourlyItemCounter].rain) > 25000.0)
    {
      this->data->hourly[hourlyItemCounter].rain = 0.0;
    }
    hourlyItemCounter++;
  }
  if (currentParent == "/ROOT/daily[]/_obj")
  {
    if (abs(this->data->daily[dailyItemCounter].rain) > 25000.0)
    {
      this->data->daily[dailyItemCounter].rain = 0.0;
    }
    dailyItemCounter++;
  }
  currentKey = "";
  currentParent = currentParent.substring(0, currentParent.lastIndexOf(PATH_SEPERATOR));
}

void OpenWeatherMapOneCall::endDocument()
{
}

void OpenWeatherMapOneCall::startArray()
{
  weatherItemCounter = 0;

  currentParent += PATH_SEPERATOR + currentKey + "[]";
  currentKey = "";
}

String getPngForWeatherId(uint16_t weatherId, bool night = false)
{
  // see https://openweathermap.org/weather-conditions#Weather-Condition-Codes-2
  // https://erikflowers.github.io/weather-icons/api-list.html

  if (night)
  {
    if (weatherId == 200 || weatherId == 201 || weatherId == 202 || weatherId == 230 || weatherId == 231 || weatherId == 232)
    {
      return "wi-night-alt-thunderstorm";
    }
    if (weatherId == 210 || weatherId == 211 || weatherId == 212 || weatherId == 221)
    {
      return "wi-night-alt-lightning";
    }

    if (weatherId == 300 || weatherId == 301 || weatherId == 321 || weatherId == 500)
    {
      return "wi-night-alt-sprinkle";
    }
    if (weatherId == 302 || weatherId == 310 || weatherId == 311 || weatherId == 312 || weatherId == 313 || weatherId == 314 || weatherId == 501 || weatherId == 502 || weatherId == 503 || weatherId == 504)
    {
      return "wi-night-alt-rain";
    }
    if (weatherId == 511 || weatherId == 615 || weatherId == 616 || weatherId == 620)
    {
      return "wi-night-alt-rain-mix";
    }
    if (weatherId == 313 || weatherId == 520 || weatherId == 521 || weatherId == 522)
    {
      return "wi-night-alt-showers";
    }
    if (weatherId == 531 || weatherId == 901)
    {
      return "wi-night-alt-storm-showers";
    }
    if (weatherId == 600 || weatherId == 601 || weatherId == 602 || weatherId == 621 || weatherId == 622)
    {
      return "wi-night-alt-snow";
    }
    if (weatherId == 611 || weatherId == 612 || weatherId == 613)
    {
      return "wi-night-alt-sleet";
    }
    if (weatherId == 701 || weatherId == 741)
    {
      return "wi-fog";
    }
    if (weatherId == 711)
    {
      return "wi-smog";
    }
    if (weatherId == 721)
    {
      return "wi-night-fog";
    }
    if (weatherId == 731 || weatherId == 751 || weatherId == 761 || weatherId == 762)
    {
      return "wi-dust";
    }
    if (weatherId == 771) // Sturmböen
    {
      return "wi-night-alt-cloudy-gusts";
    }
    if (weatherId == 781)
    {
      return "wi-tornado";
    }

    // 800 Clear
    if (weatherId == 800)
    {
      return "wi-night-clear";
    }
    // 80x Clouds
    if (weatherId == 801 || weatherId == 802)
    {
      return "wi-night-alt-cloudy";
    }
    if (weatherId == 803)
    {
      return "wi-night-alt-cloudy-high";
    }
    if (weatherId == 804)
    {
      return "wi-cloudy";
    }
  }
  else
  {
    if (weatherId == 200 || weatherId == 201 || weatherId == 202 || weatherId == 230 || weatherId == 231 || weatherId == 232)
    {
      return "wi-day-thunderstorm";
    }
    if (weatherId == 210 || weatherId == 211 || weatherId == 212 || weatherId == 221)
    {
      return "wi-day-lightning";
    }

    if (weatherId == 300 || weatherId == 301 || weatherId == 321 || weatherId == 500)
    {
      return "wi-day-sprinkle";
    }
    if (weatherId == 302 || weatherId == 310 || weatherId == 311 || weatherId == 312 || weatherId == 313 || weatherId == 314 || weatherId == 501 || weatherId == 502 || weatherId == 503 || weatherId == 504)
    {
      return "wi-day-rain";
    }
    if (weatherId == 511 || weatherId == 615 || weatherId == 616 || weatherId == 620)
    {
      return "wi-day-rain-mix";
    }
    if (weatherId == 313 || weatherId == 520 || weatherId == 521 || weatherId == 522)
    {
      return "wi-day-showers";
    }
    if (weatherId == 531 || weatherId == 901)
    {
      return "wi-day-storm-showers";
    }
    if (weatherId == 600 || weatherId == 601 || weatherId == 602 || weatherId == 621 || weatherId == 622)
    {
      return "wi-day-snow";
    }
    if (weatherId == 611 || weatherId == 612 || weatherId == 613)
    {
      return "wi-day-sleet";
    }
    if (weatherId == 701 || weatherId == 741)
    {
      return "wi-fog";
    }
    if (weatherId == 711)
    {
      return "wi-smog";
    }
    if (weatherId == 721)
    {
      return "wi-day-haze";
    }
    if (weatherId == 731 || weatherId == 751 || weatherId == 761 || weatherId == 762)
    {
      return "wi-dust";
    }
    if (weatherId == 771) // Sturmböen
    {
      return "wi-day-cloudy-gusts";
    }
    if (weatherId == 781)
    {
      return "wi-tornado";
    }

    // 800 Clear
    if (weatherId == 800)
    {
      return "wi-day-sunny";
    }
    // 80x Clouds
    if (weatherId == 801 || weatherId == 802)
    {
      return "wi-day-cloudy";
    }
    if (weatherId == 803)
    {
      return "wi-day-cloudy-high";
    }
    if (weatherId == 804)
    {
      return "wi-cloudy";
    }
  }
  return "wi-na";
}