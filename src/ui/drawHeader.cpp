
#include "drawHeader.h"
#include "homeplate.h"
#include "ui.h"

void drawHeader()
{

    double voltage = display.readBattery();
    int percent = getBatteryPercent(voltage);

    display.setFont(&Roboto_20);
    const char *dateTime = composeDateChar2();
    char battery[50];
    sprintf(battery, "%d%% (%.2fv)", percent, voltage);
    centerTextLeftMiddleRight("test", dateTime, battery, 15, 1185, 10, false);
}