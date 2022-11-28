#include "Calendar/DrawCalendar.h"

#include "ui/drawHeader.h"
#include "ui/drawDebug.h"

#include "ui/ui.h"
#include "homeplate.h"
#include "Calendar/uical/uICAL.h"
#include "utils/network.h"
#include <TimeLib.h>
#include "SdFat.h"

bool useEvent(const uICAL::VEvent &event)
{
    // ...
    Serial.println(event.summary);
    return false;
}

DrawCalendar::DrawCalendar()
{
    unsigned long lostTest = 10000UL;
    unsigned long lost_do = millis();
    Serial.printf("[Calendar] Requesting resource at %s\n", CalendarICal);

    waitForWiFi();

    HTTPClient http;

    http.getStream().setTimeout(10);
    http.getStream().flush();

    // Begin http by passing url to it
    http.begin(CalendarICal);

    // Actually do request
    int httpCode = http.GET();

    SdFile file; // Create SdFile object used for accessing files on SD card

    if (httpCode == 200)
    {
        // long n = 0;
        // while (http.getStream().available())
        //     data[n++] = http.getStream().read();
        // data[n++] = 0;
        // Serial.println(n);

        // if (display.sdCardInit())
        // {
        //     file.open("ical.ics", O_WRITE);

        //     char* c = http.getStream().readStringUntil('\n');
        //     Serial.print(c);
        //     file.print(c);
        // }
        // else
        // {
        //     Serial.println(httpCode);
        // }
        // Serial.println(data);

        // end http
        // http.end();

        String length = http.getStream().readStringUntil('\n');

        uICAL::Calendar_ptr cal = nullptr;
        try
        {
            uICAL::istream_Stream istm(http.getStream());
            cal = uICAL::Calendar::load(istm, useEvent);
        }
        catch (uICAL::Error ex)
        {
            Serial.printf("%s: %s\n", ex.message.c_str(), "! Failed loading calendar");
        }

        // auto now2 = now();
        // Serial.println("BLUIBBER");

        // Serial.println(now2);

        // uICAL::DateTime calBegin(now2);
        // uICAL::DateTime calEnd(now2 + 86400);
        // uICAL::CalendarIter_ptr calIt = uICAL::new_ptr<uICAL::CalendarIter>(cal, calBegin, calEnd);

        // while (calIt->next())
        // {
        //     uICAL::CalendarEntry_ptr entry = calIt->current();
        //     Serial.printf("Event @ %s -> %s : %s",
        //                   entry->start().as_str().c_str(),
        //                   entry->end().as_str().c_str(),
        //                   entry->summary().c_str());
        // }

        // give WiFi and TCP/IP libraries a chance to handle pending events
    }
    Serial.println("[Calendar] finished updating weather");
}

void DrawCalendar::drawCalendar()
{
    displayStart();

    display.selectDisplayMode(INKPLATE_3BIT);
    display.setTextColor(BLACK, WHITE); // Set text color to black on white

    displayEnd();

    displayStart();

    drawHeader();
    // drawDebug();
    display.display();

    displayEnd();
}

void displayCalendar()
{
    auto drawCalendar = new DrawCalendar();

    drawCalendar->drawCalendar();
}