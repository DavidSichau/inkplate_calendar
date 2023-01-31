#include <Inkplate.h>
#include <tuple>

// Battery power thresholds
#define BATTERY_VOLTAGE_HIGH 4.55
#define BATTERY_VOLTAGE_LOW 3.7
#define BATTERY_VOLTAGE_WARNING_SLEEP 3.55
#define BATTERY_PERCENT_WARNING 20

std::tuple<uint, double> readBattery();

void lowBatteryCheck();

void displayBatteryWarning();
