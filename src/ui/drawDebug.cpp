
#include "ui/drawDebug.h"
#include "homeplate.h"
#include "ui.h"

void drawDebug()
{

    Serial.println("Drawing Debug Lines");
    display.drawFastVLine(15, 0, 815, BLACK);

    display.drawFastVLine(405, 0, 815, BLACK);

    display.drawFastVLine(795, 0, 815, BLACK);

    display.drawFastVLine(1185, 0, 815, BLACK);

    display.drawFastHLine(0, 10, 1200, BLACK);

    display.drawFastHLine(0, 35, 1200, BLACK);

    display.drawFastHLine(0, 295, 1200, BLACK);

    display.drawFastHLine(0, 555, 1200, BLACK);

    display.drawFastHLine(0, 815, 1200, BLACK);
}

void drawDebugCal()
{

    Serial.println("Drawing Debug Lines");
    display.drawFastVLine(10, 0, 815, BLACK);

  

    display.drawFastVLine(15 + 50, 0, 815, BLACK);

    display.drawFastVLine(1185, 0, 815, BLACK);

    
    display.drawFastHLine(0, 10, 1200, BLACK);

    display.drawFastHLine(0, 35, 1200, BLACK);

    display.drawFastHLine(0, 815, 1200, BLACK);
}