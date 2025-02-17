#include <NTPClient.h>
#include <TimeLib.h>
#include <Timezone.h>
#include <WiFiUdp.h>

#include "utils/time.h"
#include "utils/timezone_config.h"
#include "utils/network.h"

#include "homeplate.h"

#define NTP_TASK_PRIORITY 3

bool ntpSynced = false;

bool getNTPSynced()
{
    return ntpSynced;
}

void ntpSync(void *parameter)
{
    WiFiUDP ntpUDP;
    NTPClient timeClient(ntpUDP, NTP_SERVER);
    while (true)
    {
        printDebug("[TIME] loop...");
        waitForWiFi();
        Serial.println("[TIME] Syncing RTC to NTP");
        timeClient.begin();
        if (!timeClient.forceUpdate())
        {
            Serial.printf("[TIME] NTP Sync failed\n");
            vTaskDelay((30 * SECOND) / portTICK_PERIOD_MS);
            continue;
        }
        timeClient.end();

        /* RTC */
        i2cStart();
        time_t et = timeClient.getEpochTime();
        display.rtcSetTime(hour(et), minute(et), second(et));
        display.rtcSetDate(weekday(et), day(et), month(et), year(et));
        setTime(display.rtcGetEpoch());
        i2cEnd();
        ntpSynced = true;
        Serial.printf("[TIME] RTC local time (%ld) %s\n", et, fullDateString());
        break;
    }
    printDebugStackSpace();
    vTaskDelete(NULL); // end self task
}

void setupTimeAndSyncTask()
{
    i2cStart();
    uint32_t t = display.rtcGetEpoch();
    setTime(t);
    bool rtcSet = display.rtcIsSet();
    i2cEnd();
    Serial.printf("[TIME] RTC local time (%u) %s\n", t, fullDateString());
    // Sync RTC if unset or fresh boot
    if (!rtcSet || !sleepBoot)
    {
        xTaskCreate(
            ntpSync,           /* Task function. */
            "NTP_TASK",        /* String with name of task. */
            2048,              /* Stack size */
            NULL,              /* Parameter passed as input of the task */
            NTP_TASK_PRIORITY, /* Priority of the task. */
            NULL);             /* Task handle. */
    }
}

time_t changeTimeByDay(int dayChange, time_t base)
{
    struct tm *tm = localtime(&base);
    tm->tm_mday += dayChange;
    return mktime(tm);
}

char dateStringBuf[17]; // 1990-12-27 13:37
char *fullDateString()
{
    TimeChangeRule *tcr; // pointer to the time change rule, use to get TZ abbrev
    time_t utc = now();
    time_t local = tz.toLocal(utc, &tcr);

    snprintf(dateStringBuf, 17, "%d-%02d-%02d %02d:%02d", year(local), month(local), day(local), hour(local), minute(local));
    return dateStringBuf;
}

char timeStringBuf[6]; // 13:37
char *timeString()
{
    TimeChangeRule *tcr; // pointer to the time change rule, use to get TZ abbrev
    time_t utc = now();
    time_t local = tz.toLocal(utc, &tcr);

    snprintf(timeStringBuf, 17, "%02d:%02d", hour(local), minute(local));
    return timeStringBuf;
}

char timeStringBuf2[6]; // 13:37
char *timeFromUnixString(uint32_t utc)
{
    TimeChangeRule *tcr; // pointer to the time change rule, use to get TZ abbrev
    time_t local = tz.toLocal(utc, &tcr);

    snprintf(timeStringBuf2, 17, "%02d:%02d", hour(local), minute(local));
    return timeStringBuf2;
}

static const char *lang_months[][12] = {

    // English
    {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"},

    // German
    {"Jan", "Feb", "Mrz", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dez"},

};
char dateBuf[7];

char *composeDateChar2()
{
    TimeChangeRule *tcr; // pointer to the time change rule, use to get TZ abbrev
    time_t utc = now();
    time_t local = tz.toLocal(utc, &tcr);

    byte dateDay = day(local);
    byte dateMonth = month(local);

    sprintf(dateBuf, "%2d. %3s", dateDay, lang_months[1][dateMonth - 1]);

    return dateBuf;
}

char dayD[2];

char *getWeekday(uint32_t utc)
{
    TimeChangeRule *tcr; // pointer to the time change rule, use to get TZ abbrev
    time_t local = tz.toLocal(utc, &tcr);

    byte dateDay = weekday(local);

    sprintf(dayD, "%2s", lang_day[1][dateDay - 1]);

    return dayD;
}

int getHour(uint32_t utc)
{
    TimeChangeRule *tcr; // pointer to the time change rule, use to get TZ abbrev
    time_t local = tz.toLocal(utc, &tcr);

    return hour(local);
}

int getWeekdayL(uint32_t utc)
{
    TimeChangeRule *tcr; // pointer to the time change rule, use to get TZ abbrev
    time_t local = tz.toLocal(utc, &tcr);
    auto w = weekday(local);
    if (w == 1)
    {
        return 7;
    }
    return w - 1;
}

int getNowL()
{
    TimeChangeRule *tcr; // pointer to the time change rule, use to get TZ abbrev
    time_t utc = now();
    return tz.toLocal(utc, &tcr);
}