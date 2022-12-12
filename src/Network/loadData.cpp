#include "homeplate.h"
#include "utils/network.h"
#include "utils/time.h"
#include "Network/loadData.h"

#include <WiFi.h>
#include <HTTPClient.h>

WiFiClientSecure client;

void getData(String path, String host, int port, JsonStreamingParser &parser)
{
    unsigned long lostTest = 10000UL;
    unsigned long lost_do = millis();
    waitForWiFi();

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
                    parser.parse(c);
                }
            }
            // give WiFi and TCP/IP libraries a chance to handle pending events
            yield();
        }
        client.stop();
    }
    else
    {
        Serial.println("[HTTPS] failed to connect to host");
    }
    Serial.println("[HTTPS] data loaded");
}
