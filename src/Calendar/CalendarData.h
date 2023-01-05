#pragma once
#include <Arduino.h>
#include <JsonListener.h>
#include <JsonStreamingParser.h>
#include "config.h"
#include <vector>

typedef struct CalendarEvent
{
  // "dt":1587216739
  uint32_t start;
  // "sunrise":1587182465
  uint32_t end;

  String summary;

} CalendarEvent;

typedef struct CalendarEventDaily
{
  // "dt":1587216739
  uint32_t date;

  String summary;

} CalendarEventDaily;

typedef struct CalendarDataType
{
  std::vector<CalendarEvent> events;
  std::vector<CalendarEventDaily> daily;
} CalendarDataType;

class CalendarData : public JsonListener
{
private:
  const String host = CalendarHost;
  String appId = CalendarToken;
  const uint8_t port = 80;
  String currentKey = "ROOT";
  String currentParent;
  CalendarDataType *data;
  uint8_t dailyItemCounter = 0;
  uint8_t eventItemCounter = 0;

  CalendarEvent currentEvent;
  CalendarEventDaily currentDaily;

  uint8_t maxDaily = 20;
  uint8_t maxEvents = 50;

  void doUpdate(CalendarDataType *data, String path);
  String buildPath(time_t time, bool includeNextDay);

public:
  CalendarData();
  void update(CalendarDataType *data, time_t time, bool includeNextDay);

  virtual void whitespace(char c);
  virtual void startDocument();
  virtual void key(String key);
  virtual void value(String value);
  virtual void endArray();
  virtual void endObject();
  virtual void endDocument();
  virtual void startArray();
  virtual void startObject();
};
