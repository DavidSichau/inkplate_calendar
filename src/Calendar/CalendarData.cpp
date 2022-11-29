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

String PATH_SEPERATOR_CALENDAR = "/";

CalendarData::CalendarData()
{
}

void CalendarData::update(CalendarDataType *data, String appId)
{
  doUpdate(data, buildPath(appId));
}

String CalendarData::buildPath(String appId)
{
  return "/?token=" + appId;
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
  Serial.printf("[HTTP] Requesting resource at http://%s:%u%s\n", host.c_str(), port, path.c_str());

  waitForWiFi();
  WiFiClient client;
  if (client.connect(host.c_str(), port))
  {
    bool isBody = false;
    char c;
    Serial.println("[HTTP] connected, now GETting data");
    client.print("GET " + path + " HTTP/1.1\r\n"
                                 "Host: " +
                 host + "\r\n"
                        "Connection: close\r\n\r\n");

    while (client.connected() || client.available())
    {
      if (client.available())
      {
        if ((millis() - lost_do) > lostTest)
        {
          Serial.println("[HTTP] lost in client with a timeout");
          client.stop();
          ESP.restart();
        }
        c = client.read();
        if (c == '{' || c == '[')
        {
          isBody = true;
        }
        if (isBody)
        {
          parser.parse(c);
        }
      }
      // give WiFi and TCP/IP libraries a chance to handle pending events
      yield();
    }
    client.stop();
  }
  else
  {
    Serial.println("[HTTP] failed to connect to host");
  }
  this->data = nullptr;
  Serial.println("[Calendar] finished updating weather");
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
      this->data->daily[dailyItemCounter].summary = value;
    }
    if (currentKey == "date")
    {
      this->data->daily[dailyItemCounter].date = value.toInt();
    }
  }
  if (currentParent.startsWith("/ROOT/events[]"))
  {
    if (currentKey == "summary")
    {
      this->data->events[eventItemCounter].summary = value;
    }
    if (currentKey == "start")
    {
      this->data->events[eventItemCounter].start = value.toInt();
    }
    if (currentKey == "end")
    {
      this->data->events[eventItemCounter].end = value.toInt();
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
}

void CalendarData::endObject()
{

  if (currentParent == "/ROOT/daily[]/_obj")
  {
    dailyItemCounter++;
  }
  if (currentParent == "/ROOT/events[]/_obj")
  {
    eventItemCounter++;
  }
  currentKey = "";
  currentParent = currentParent.substring(0, currentParent.lastIndexOf(PATH_SEPERATOR_CALENDAR));
}

void CalendarData::endDocument()
{
}

void CalendarData::startArray()
{
  currentParent += PATH_SEPERATOR_CALENDAR + currentKey + "[]";
  currentKey = "";
}
