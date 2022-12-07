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
#include "Calendar/CalendarData.h"
#include "homeplate.h"
#include "utils/network.h"
#define FS_NO_GLOBALS
#include "FS.h"
#include <LittleFS.h>

String PATH_SEPERATOR_CALENDAR = "/";

CalendarData::CalendarData()
{
}

void CalendarData::update(CalendarDataType *data, time_t time)
{
  doUpdate(data, buildPath(time));
}

String CalendarData::buildPath(time_t time)
{
  return "/?token=" + this->appId + "&time=" + time;
}

void CalendarData::doUpdate(CalendarDataType *data, String path)
{
  unsigned long lostTest = 10000UL;
  unsigned long lost_do = millis();
  this->eventItemCounter = 0;
  this->dailyItemCounter = 0;
  this->data = data;
  JsonStreamingParser parser;
  parser.setListener(this);

  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  fs::File file = LittleFS.open("/calendar.json", "r");
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }
  Serial.println("File Content:");
  while (file.available())
  {
    parser.parse(file.read());
  }
  file.close();

  this->data = nullptr;
  Serial.println("[Calendar] finished loading data");
}

void CalendarData::whitespace(char c)
{
}

void CalendarData::startDocument()
{
  this->eventItemCounter = 0;
  this->dailyItemCounter = 0;
}

void CalendarData::key(String key)
{
  currentKey = key;
}

void CalendarData::value(String value)
{
  if (currentParent.startsWith("/ROOT/daily[]"))
  {
    if (currentKey == "summary")
    {
      // this->data->daily[dailyItemCounter].summary = value;
      this->currentDaily.summary = value;
    }
    if (currentKey == "date")
    {
      this->currentDaily.date = value.toInt();
    }
  }
  if (currentParent.startsWith("/ROOT/events[]"))
  {
    if (currentKey == "summary")
    {
      // this->data->events[eventItemCounter].summary = value;
      this->currentEvent.summary = value;
    }
    if (currentKey == "start")
    {
      // this->data->events[eventItemCounter].start = value.toInt();
      this->currentEvent.start = value.toInt();
    }
    if (currentKey == "end")
    {
      // this->data->events[eventItemCounter].end = value.toInt();
      this->currentEvent.end = value.toInt();
    }
  }
}

void CalendarData::endArray()
{
  currentKey = "";
  currentParent = currentParent.substring(0, currentParent.lastIndexOf(PATH_SEPERATOR_CALENDAR));
}

void CalendarData::startObject()
{
  if (currentKey == "")
  {
    currentKey = "_obj";
  }
  currentParent += PATH_SEPERATOR_CALENDAR + currentKey;
  this->currentDaily = {};
  this->currentEvent = {};
}

void CalendarData::endObject()
{

  if (currentParent == "/ROOT/daily[]/_obj")
  {
    dailyItemCounter++;
    this->data->daily.push_back(this->currentDaily);
  }
  if (currentParent == "/ROOT/events[]/_obj")
  {
    eventItemCounter++;
    this->data->events.push_back(this->currentEvent);
  }
  currentKey = "";
  currentParent = currentParent.substring(0, currentParent.lastIndexOf(PATH_SEPERATOR_CALENDAR));
}

bool compareEvents(CalendarEvent i1, CalendarEvent i2)
{
  return (i1.start < i2.start);
}

void CalendarData::endDocument()
{
  std::sort(this->data->events.begin(), this->data->events.end(), compareEvents);
}

void CalendarData::startArray()
{
  currentParent += PATH_SEPERATOR_CALENDAR + currentKey + "[]";
  currentKey = "";
}
