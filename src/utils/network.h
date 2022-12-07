#pragma once

#include <WiFi.h>
#include "homeplate.h"

// network settings
#define WIFI_TIMEOUT_MS (20 * SECOND)      // 20 second WiFi connection timeout
#define WIFI_RECOVER_TIME_MS (30 * SECOND) // Wait 30 seconds after a failed connection attempt

#define WIFI_TASK_PRIORITY 2

void wifiConnectTask();

void wifiStopTask();

void waitForWiFi();

bool getWifIFailed();
