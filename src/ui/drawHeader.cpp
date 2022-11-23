
#include "drawHeader.h"
#include "homeplate.h"
#include "ui.h"

void drawHeader()
{

    double voltage = display.readBattery();
    int percent = getBatteryPercent(voltage);

    display.setFont(&Roboto_20);
    const char *date = composeDateChar2();
    const char *time = timeString();
    char battery[50];
    sprintf(battery, "%d%% (%.2fv)", percent, voltage);
    centerTextLeftMiddleRight(time, date, battery, 15, 1185, 10, false);
}