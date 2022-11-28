#include "Calendar/DrawCalendar.h"

#include "ui/drawHeader.h"
#include "ui/drawDebug.h"

#include "ui/ui.h"
#include "homeplate.h"
#include "Calendar/CalendarData.h"
#include "utils/network.h"
#include <TimeLib.h>
#include "SdFat.h"

HTTPClient http;
SdFile file;

DrawCalendar::DrawCalendar(String path)
{
    CalendarData *oneCallClient = new CalendarData();

    oneCallClient->update(&this->data, "1234");
    delete oneCallClient;
    oneCallClient = nullptr;
    Serial.println("[Calendar]: loaded data");
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
    auto drawCalendar = new DrawCalendar(CalendarICal);

    drawCalendar->drawCalendar();
}