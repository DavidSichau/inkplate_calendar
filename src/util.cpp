#include "homeplate.h"

const char *bootReason()
{
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause();
    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        return "wake button"; // (EXT0_RTC_IO) is the wake button...
    case ESP_SLEEP_WAKEUP_EXT1:
        return "touchpads"; // (EXT1_RTC_CNTL)
    case ESP_SLEEP_WAKEUP_TIMER:
        return "timer";
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        return "internal touchpad";
    case ESP_SLEEP_WAKEUP_ULP:
        return "ULP program";
    default:
        return "normal";
    }
}


/**
 * \brief Called if stack overflow during execution
 */
extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
                                          signed char *pcTaskName)
{
    Serial.printf("\n\n!!! [DEBUG]: stack overflow %x %s\n\n", (unsigned int)pxTask, (portCHAR *)pcTaskName);
    /* If the parameters have been corrupted then inspect pxCurrentTCB to
     * identify which task has overflowed its stack.
     */
    for (;;)
    {
    }
}

void printDebugStackSpace()
{
    if (DEBUG_STACK)
    {
        UBaseType_t uxHighWaterMark;
        uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        Serial.printf("[DEBUG][HEAP] TASK: %s; stack min remaining(%d) txPortGetFreeHeapSize(%d) xPortGetMinimumEverFreeHeapSize(%d) bytes\n", pcTaskGetTaskName(NULL), uxHighWaterMark, xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
    }
}

// vTaskGetRunTimeStats and vTaskList are disabled in esp-idf :()
// void taskinfo()
// {
//     Serial.printf("Task info:\n");
//     auto num = uxTaskGetNumberOfTasks();
//     Serial.printf("total tasks: %d\n", num);
//     char buff[40 * num + 40]; // 40 bytes per task + 40
//     vTaskGetRunTimeStats(buff);
//     Serial.printf("vTaskGetRunTimeStats:\n%s", buff);
//     vTaskList(buff);
//     Serial.printf("vTaskList:\n%s", buff);
//     //uxTaskGetSystemState();
// }


void printDebug(const char *s)
{
    if (DEBUG_PRINT)
    {
        Serial.printf("[DEBUG]%s\n", s);
    }
}