#include "utils/sleep.h"
#include "utils/network.h"
#include "utils/ota.h"
#include "utils/time.h"
#include <TimeLib.h>

#define uS_TO_S_FACTOR 1000000 // Conversion factor for micro seconds to seconds
#define SLEEP_TASK_PRIORITY 1
#define TOUCHPAD_WAKE_MASK (int64_t(1) << GPIO_NUM_34)

static unsigned long sleepTime;
uint32_t defaultSleepDuration = TIME_TO_SLEEP_SEC;

void setSleepDuration(uint32_t sec)
{
    defaultSleepDuration = sec;
}

void gotoSleepNow()
{
    Serial.println("[SLEEP] prepping for sleep");

    auto sleepDuration = defaultSleepDuration;

    auto now = getNowL();
    // Sleep from 22 until 6 in the morning
    if (hour(now) >= 22 || hour(now) < 6)
    {
        // 6 in the morning
        auto secondsSkip = SECS_PER_HOUR * 6;
        if (hour(now) >= 22)
        {
            secondsSkip = secondsSkip + SECS_PER_DAY - elapsedSecsToday(now);
        }
        else
        {
            secondsSkip = secondsSkip - elapsedSecsToday(now);
        }
        sleepDuration = secondsSkip;
    }

    i2cStart();
    // disconnect WiFi as it's no longer needed
    wifiStopTask(); // prevent i2c lock in main thread

    // set MCP interrupts
    display.setIntOutputInternal(MCP23017_INT_ADDR, display.mcpRegsInt, 1, false, false, HIGH);
    i2cEnd();

    // Go to sleep for TIME_TO_SLEEP seconds
    if (esp_sleep_enable_timer_wakeup(sleepDuration * uS_TO_S_FACTOR) != ESP_OK)
    {
        Serial.printf("[SLEEP] ERROR esp_sleep_enable_timer_wakeup(%u) invalid value", sleepDuration * uS_TO_S_FACTOR);
    }

    // Enable wakeup from deep sleep on gpio 36 (WAKE BUTTON)
    esp_sleep_enable_ext0_wakeup(WAKE_BUTTON, LOW);
    // enable wake from MCP port expander
    esp_sleep_enable_ext1_wakeup(TOUCHPAD_WAKE_MASK, ESP_EXT1_WAKEUP_ANY_HIGH);
    Serial.printf("[SLEEP] entering sleep for %u seconds (%u min)\n\n\n", sleepDuration, sleepDuration / 60);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    esp_deep_sleep_start(); // Put ESP32 into deep sleep. Program stops here.
}

void delaySleep(uint seconds)
{
    unsigned long timeLeft = sleepTime - millis();
    // if the bumped time is farther in the future than our current sleep time
    int ms = seconds * SECOND;
    if (ms > timeLeft)
    {
        Serial.printf("[SLEEP] delaying sleep for %u seconds\n", seconds);
        sleepTime = ms + millis();
    }
}

void checkSleep(void *parameter)
{
    while (true)
    {
        printDebug("[SLEEP] sleep loop..");
        // check the sleep time
        while (sleepTime > millis())
        {
            vTaskDelay(SECOND / portTICK_PERIOD_MS);
        }

        startActivity(NONE);
        waitForOTA(); // dont sleep if there is an OTA being performed
        printDebugStackSpace();
        // i2cStart();
        // displayStart();
        // display.einkOff();
        // displayEnd();
        // i2cEnd();
        gotoSleepNow();
    }
}

void sleepTask()
{
    sleepTime = (SLEEP_TIMEOUT_SEC * SECOND) + millis();

    xTaskCreate(
        checkSleep,
        "SLEEP_TASK",        // Task name
        2048,                // Stack size
        NULL,                // Parameter
        SLEEP_TASK_PRIORITY, // Task priority
        NULL                 // Task handle
    );
}
