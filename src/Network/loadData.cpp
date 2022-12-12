#include "homeplate.h"
#include "utils/network.h"
#include "utils/time.h"

#include <WiFi.h>
#include <HTTPClient.h>
#define FS_NO_GLOBALS
#include "FS.h"
#include <LittleFS.h>

WiFiClientSecure client;
xTaskHandle loadDataTaskHandle;

#define FORMAT_LITTLEFS_IF_FAILED true

void downloadFileAndSave(String path, String host, int port, String fileName)
{
    unsigned long lostTest = 10000UL;
    unsigned long lost_do = millis();
    waitForWiFi();

    fs::File file = LittleFS.open(fileName, FILE_WRITE);

    client.setInsecure();
    if (client.connect(host.c_str(), port))
    {
        bool isBody = false;
        char c;
        Serial.println("[HTTPS] connected, now GETting data");
        client.println("GET https://" + host + path + " HTTP/1.0");
        client.println("Host: " + host);
        client.println("Connection: close");
        client.println();

        while (client.connected() || client.available())
        {
            if (client.available())
            {
                if ((millis() - lost_do) > lostTest)
                {
                    Serial.println("[HTTPS] lost in client with a timeout");
                    client.stop();
                    ESP.restart();
                }
                c = client.read();
                if (c == '{' || c == '[')
                {
                    isBody = true;
                }
                if (isBody)
                {
                    file.write(c);
                }
            }
            // give WiFi and TCP/IP libraries a chance to handle pending events
            yield();
        }
        file.flush();
        file.close();
        client.stop();
    }
    else
    {
        Serial.println("[HTTPS] failed to connect to host");
    }
    Serial.println("[HTTPS] data loaded");
}

void loadData(void *params)
{
    Serial.println("[Load Data Task] started");
    fsStart();

    if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED))
    {
        Serial.println("LITTLEFS Mount Failed");
        return;
    }

    auto weatherPath = "/data/2.5/onecall?appid=" + (String)OPEN_WEATHER_MAP_APP_ID + "&lat=" + OPEN_WEATHER_MAP_LOCATTION_LAT + "&lon=" + OPEN_WEATHER_MAP_LOCATTION_LON + "&units=metric&lang=" + OPEN_WEATHER_MAP_LANGUAGE;
    downloadFileAndSave(weatherPath, "api.openweathermap.org", 443, "/weather.json");

    auto time = getNowL();

    auto calendarPath = "/?token=" + (String)CalendarToken + "&time=" + time;
    downloadFileAndSave(calendarPath, CalendarHost, 443, "/calendar.json");
    Serial.println("[Load Data Task] ended");

    fsEnd();

    vTaskDelete(loadDataTaskHandle);
}

void startLoadDataTask()
{
    if (loadDataTaskHandle != NULL)
    {
        Serial.printf("[Load Data Task] Task Already running\n");
        return;
    }
    // inkplate code needs to be on arduino core or may get i2c errors
    // use mutex for all inkplate code
    xTaskCreate(
        loadData,         /* Task function. */
        "LOAD_DATA_TASK", /* String with name of task. */
        8192,             /* Stack size */
        NULL,             /* Parameter passed as input of the task */
        20,               /* Priority of the task. */
        &loadDataTaskHandle);
}
