#include <Inkplate.h>
#include "utils/battery.h"
#include "utils/sleep.h"

#include "homeplate.h"

std::tuple<uint, double> readBattery()
{
    std::vector<double> voltages;
    for (int i = 0; i < 3; i++)
    {
        auto vol = display.readBattery();
        vol = roundf(vol * 100) / 100; // rounds to 2 decimal places

        voltages.push_back(vol);
        delay(50);
    }
    std::sort(voltages.begin(), voltages.end());

    auto voltage = voltages[1];

    uint percentage = ((voltage - BATTERY_VOLTAGE_LOW) * 100.0) / (BATTERY_VOLTAGE_HIGH - BATTERY_VOLTAGE_LOW);
    if (percentage > 100)
        percentage = 100;
    if (percentage < 0)
        percentage = 0;
    return {percentage, voltage};
}

void lowBatteryCheck()
{
    // need min uptime for voltage to be stable
    if (millis() < 2 * SECOND)
    {
        return;
    }
    i2cStart();
    double voltage = display.readBattery();
    i2cEnd();

    // if voltage was < 1, it was a bad reading.
    if (voltage > 1 && voltage <= BATTERY_VOLTAGE_WARNING_SLEEP)
    {
        Serial.printf("[MAIN] voltage %.2f <= min %.2f, powering down\n", voltage, BATTERY_VOLTAGE_WARNING_SLEEP);
        displayStatusMessage("Low Battery");

        setSleepDuration(0xFFFFFFFF);
        gotoSleepNow();
    }
}

// NOTE I2C & display locks MUST NOT be held by caller.
void displayBatteryWarning()
{
    static const uint8_t buf_size = 30;
    static char statusBuffer[buf_size];
    i2cStart();

    auto [percent, voltage] = readBattery();

    if (percent > BATTERY_PERCENT_WARNING)
    {
        i2cEnd();
        return;
    }

    snprintf(statusBuffer, buf_size, "WARNING: battery %u%%", percent);

    displayStart();
    display.selectDisplayMode(INKPLATE_1BIT);
    display.setTextColor(BLACK, WHITE);
    display.setFont(&Roboto_16);
    display.setTextSize(1);

    const int16_t pad = 3; // padding
    const int16_t mar = 5; // margin
    int16_t x = E_INK_WIDTH / 2;
    int16_t y = E_INK_HEIGHT - mar;

    // get text size for box
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(statusBuffer, x, y, &x1, &y1, &w, &h);

    x = (E_INK_WIDTH / 2) - (w / 2);

    // background box to set internal buffer colors
    display.fillRect(x - pad, y - pad - h, w + (pad * 2), h + (pad * 2), WHITE);
    // Serial.printf("fillRect(x:%u, y:%u, w:%u, h:%u)\n", x-pad, y-pad-h, max(w+(pad*2), 400), h+(pad*2));
    // display.partialUpdate(sleepBoot);

    // display status message
    display.setCursor(x, y);

    // text to print over box
    display.print(statusBuffer);
    display.partialUpdate(sleepBoot);
    displayEnd();
    i2cEnd();
}