#include <Inkplate.h>

// Battery power thresholds
#define BATTERY_VOLTAGE_HIGH 4.7
#define BATTERY_VOLTAGE_LOW 3.6
#define BATTERY_VOLTAGE_WARNING_SLEEP 3.55
#define BATTERY_PERCENT_WARNING 20

uint getBatteryPercent(double voltage);

void lowBatteryCheck();

void displayBatteryWarning();
