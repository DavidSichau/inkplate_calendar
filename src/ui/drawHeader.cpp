
#include "drawHeader.h"
#include "ui.h"
#include <homeplate.h>
#include <utils/time.h>
#include "utils/battery.h"

void drawHeader()
{
    auto [percent, voltage] = readBattery();

    display.setFont(&Roboto_20);
    const char *date = composeDateChar2();
    const char *time = timeString();
    char battery[50];
    sprintf(battery, "%d%% (%.2fv)", percent, voltage);
    centerTextLeftMiddleRight(time, date, battery, 15, 1185, 10);
}