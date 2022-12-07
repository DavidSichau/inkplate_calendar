#include "homeplate.h"
#include "utils/network.h"
#include "utils/time.h"

#include <WiFi.h>
#include <HTTPClient.h>
#define FS_NO_GLOBALS
#include "FS.h"
#include <LittleFS.h>

WiFiClientSecure client;
boolean isDataLoaded = false;

#define FORMAT_LITTLEFS_IF_FAILED true

void downloadFileAndSave(String path, String host, int port, String fileName)
{
    unsigned long lostTest = 10000UL;
    unsigned long lost_do = millis();
    waitForWiFi();

    if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED))
    {
        Serial.println("LITTLEFS Mount Failed");
        return;
    }

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

    // fs::File myFile = LittleFS.open(fileName, FILE_READ);

    // // re-open the file for reading:
    // if (!myFile)
    // {
    //     Serial.print("opening test.txt for read failed");
    // }
    // Serial.println("DATA:");

    // // read from the file until there's nothing else in it:`
    // while (myFile.available())
    // {
    //     Serial.write(myFile.read());
    // }
    // // close the file:
    // myFile.close();
}

void loadData(void *params)
{
    spiStart();
    auto weatherPath = "/data/2.5/onecall?appid=" + (String)OPEN_WEATHER_MAP_APP_ID + "&lat=" + OPEN_WEATHER_MAP_LOCATTION_LAT + "&lon=" + OPEN_WEATHER_MAP_LOCATTION_LON + "&units=metric&lang=" + OPEN_WEATHER_MAP_LANGUAGE;
    downloadFileAndSave(weatherPath, "api.openweathermap.org", 443, "/weather.json");

    auto time = getNowL();

    auto calendarPath = "/?token=" + (String)CalendarToken + "&time=" + time;
    downloadFileAndSave(calendarPath, CalendarHost, 443, "/calendar.json");
    isDataLoaded = true;
    spiEnd();
}

void loadData()
{
    spiStart();
    auto weatherPath = "/data/2.5/onecall?appid=" + (String)OPEN_WEATHER_MAP_APP_ID + "&lat=" + OPEN_WEATHER_MAP_LOCATTION_LAT + "&lon=" + OPEN_WEATHER_MAP_LOCATTION_LON + "&units=metric&lang=" + OPEN_WEATHER_MAP_LANGUAGE;
    downloadFileAndSave(weatherPath, "api.openweathermap.org", 443, "/weather.json");

    auto time = getNowL();

    auto calendarPath = "/?token=" + (String)CalendarToken + "&time=" + time;
    downloadFileAndSave(calendarPath, CalendarHost, 443, "/calendar.json");
    isDataLoaded = true;
    spiEnd();
}

void startLoadDataTask()
{
    // inkplate code needs to be on arduino core or may get i2c errors
    // use mutex for all inkplate code
    xTaskCreatePinnedToCore(
        loadData,         /* Task function. */
        "LOAD_DATA_TASK", /* String with name of task. */
        8192,             /* Stack size */
        NULL,             /* Parameter passed as input of the task */
        20,               /* Priority of the task. */
        NULL,
        CONFIG_ARDUINO_RUNNING_CORE);
}

void waitForDataLoaded()
{
    while (!isDataLoaded)
    {
        Serial.println("WAITING: Data not loaded");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}