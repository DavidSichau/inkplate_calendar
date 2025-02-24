#pragma once

#include <Inkplate.h>

#include "fonts/Roboto_12.h"
#include "fonts/Roboto_16.h"
#include "fonts/Roboto_20.h"
#include "fonts/Roboto_32.h"
#include "fonts/Roboto_40.h"
#include "fonts/Roboto_64.h"
#include "fonts/Roboto_128.h"
#include "fonts/Roboto_200.h"
#include "fonts/FreeSans9pt7b.h"
#include "fonts/FreeSans12pt7b.h"
#include "fonts/FreeSans18pt7b.h"
#include "fonts/FreeSans24pt7b.h"

#include "fonts/FreeSansBold9pt7b.h"
#include "fonts/FreeSansBold12pt7b.h"
#include "fonts/FreeSansBold18pt7b.h"
#include "fonts/FreeSansBold24pt7b.h"
#include "config.h"

// check that config file is correctly set
#if !defined CONFIG_H
#error Missing config.h!
#error HINT: copy config_example.h to config.h and make changes.
#endif

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
                                          signed char *pcTaskName);

extern Inkplate display;
extern SemaphoreHandle_t mutexI2C, mutexDisplay, mutexSPI, mutexFS;
extern bool sleepBoot;
extern uint bootCount, activityCount;

#define i2cStart() xSemaphoreTake(mutexI2C, portMAX_DELAY)
#define i2cEnd() xSemaphoreGive(mutexI2C)
#define spiStart() xSemaphoreTake(mutexSPI, portMAX_DELAY)
#define spiEnd() xSemaphoreGive(mutexSPI)
#define displayStart() xSemaphoreTake(mutexDisplay, portMAX_DELAY)
#define displayEnd() xSemaphoreGive(mutexDisplay)
#define fsStart() xSemaphoreTake(mutexFS, portMAX_DELAY)
#define fsEnd() xSemaphoreGive(mutexFS)

#define WAKE_BUTTON GPIO_NUM_36

#define VERSION __DATE__ ", " __TIME__

// Image "colors" (3bit mode)
#define C_BLACK 0
#define C_GREY_1 1
#define C_GREY_2 2
#define C_GREY_3 3
#define C_GREY_4 4
#define C_GREY_5 5
#define C_GREY_6 6
#define C_WHITE 7

// for second to ms conversions
#define SECOND 1000

// info
void displayInfoScreen();

// Image
void displayStatusMessage(const char *format, ...);

// util
const char *bootReason();
void printDebugStackSpace();
void printDebug(const char *s);

// activity
enum Activity
{
    NONE,
    Overview,
    Calendar,
    Weather,
    Info,
};

#define DEFAULT_ACTIVITY Overview
void startActivity(Activity activity);
void startActivitiesTask();
bool stopActivity();

/*
 * Global Settings
 */

// debounce time limit for static activities
#define MIN_ACTIVITY_RESTART_SECS 1

// input debounce
#define DEBOUNCE_DELAY_MS (SECOND / 2)

// debug settings
#define DEBUG_STACK false
#define DEBUG_PRINT false
