#include "homeplate.h"
#include "utils/battery.h"
#include "utils/sleep.h"
#include "qr.h"
#include "utils/ota.h"
#include "Weather/DrawWeather.h"
#include "Overview/DrawOverview.h"
#include "Network/loadData.h"
#include "Calendar/DrawCalendar.h"

#define ACTIVITY_TASK_PRIORITY 4

static bool resetActivity = false;
uint activityCount = 0;

QueueHandle_t activityQueue = xQueueCreate(1, sizeof(Activity));

static unsigned long lastActivityTime = 0;
static Activity activityNext, activityCurrent = NONE;

void startActivity(Activity activity)
{
    static SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
    if (xSemaphoreTake(mutex, (SECOND) / portTICK_PERIOD_MS) == pdTRUE)
    {
        // dont re-queue HomeAssistant Activity is run within 60 sec and already running
        // if (activity == DataLoading && activityCurrent == DataLoading && ((millis() - lastActivityTime) / SECOND < 60))
        // {
        //     Serial.printf("[ACTIVITY] startActivity(%d) DataLoading already running within time limit, skipping\n", activity);
        //     xSemaphoreGive(mutex);
        //     return;
        // }
        // insert into queue
        Serial.printf("[ACTIVITY] startActivity(%d) put into queue\n", activity);
        resetActivity = true;
        xQueueOverwrite(activityQueue, &activity);
        xSemaphoreGive(mutex);
    }
    else
    {
        Serial.printf("[ACTIVITY][ERROR] startActivity(%d) unable to take mutex\n", activity);
        return;
    }
}

// returns true if there was an event that should cause the curent activity to stop/break early to start something new
bool stopActivity()
{
    return resetActivity;
}

void waitForWiFiOrActivityChange()
{
    while (!WiFi.isConnected() && !resetActivity)
    {
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
}

void runActivities(void *params)
{
    while (true)
    {
        printDebug("[ACTIVITY] loop...");
        printDebugStackSpace();
        waitForOTA(); // block if an OTA is running
        if (xQueueReceive(activityQueue, &activityNext, portMAX_DELAY) != pdTRUE)
        {
            Serial.printf("[ACTIVITY][ERROR] runActivities() unable to read from activityQueue\n");
            vTaskDelay(500 / portTICK_PERIOD_MS); // wait just to be safe
            continue;
        }
        waitForOTA();
        resetActivity = false;
        printDebug("[ACTIVITY] runActivities ready...");

        if (activityNext != NONE)
            delaySleep(10); // bump the sleep timer a little for any ongoing activity

        // activity debounce
        if ((activityNext == activityCurrent) && ((millis() - lastActivityTime) / SECOND < MIN_ACTIVITY_RESTART_SECS))
        {
            Serial.printf("[ACTIVITY] same activity %d launched withing min window, skipping\n", activityNext);
            continue;
        }
        lastActivityTime = millis();
        activityCount++;

        Serial.printf("[ACTIVITY] starting activity: %d\n", activityNext);
        switch (activityNext)
        {
        case NONE:
            break;
        case Overview:
            delaySleep(25);
            setSleepDuration(TIME_TO_SLEEP_SEC);
            // wait for wifi or reset activity
            if (resetActivity)
            {
                Serial.printf("[ACTIVITY][ERROR] Overview Activity reset while waiting, aborting...\n");
                continue;
            }
            displayOverview();
            break;
        case Weather:
            delaySleep(15);
            setSleepDuration(TIME_TO_SLEEP_SEC);
            // wait for wifi or reset activity
            if (resetActivity)
            {
                Serial.printf("[ACTIVITY][ERROR] Weather Activity reset while waiting, aborting...\n");
                continue;
            }
            displayWeather();
            break;
        case Calendar:
            delaySleep(15);
            setSleepDuration(TIME_TO_SLEEP_SEC);
            // wait for wifi or reset activity
            if (resetActivity)
            {
                Serial.printf("[ACTIVITY][ERROR] Weather Activity reset while waiting, aborting...\n");
                continue;
            }
            displayCalendar();
            break;
        case Info:
            setSleepDuration(TIME_TO_QUICK_SLEEP_SEC);
            displayInfoScreen();
            break;
        default:
            Serial.printf("[ACTIVITY][ERROR] runActivities() unhandled Activity: %d\n", activityNext);
        }
        activityCurrent = activityNext;
        // check and display a low battery warning if needed
        displayBatteryWarning();
    }
}

void startActivitiesTask()
{
    xTaskCreate(
        runActivities,
        "ACTIVITY_TASK",        // Task name
        8192 * 5,               // Stack size
        NULL,                   // Parameter
        ACTIVITY_TASK_PRIORITY, // Task priority
        NULL                    // Task handle
    );
}