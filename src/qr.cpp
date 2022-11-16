#include <qrcode.h>
#include "homeplate.h"

void renderQR(QRCode qrcode, uint32_t x, uint32_t y, uint32_t size);

void displayWiFiQR()
{
    Serial.printf("Rendering wifi QR Code\n");
    char buf[1024];
    snprintf(buf, 1024, "WIFI:S:%s;T:WPA;P:%s;;", QR_WIFI_NAME, QR_WIFI_PASSWORD);
    // Create the QR code
    static uint8_t version = 5;
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(version)];
    qrcode_initText(&qrcode, qrcodeData, version, ECC_MEDIUM, buf);
    uint32_t size = 5;

    uint32_t y = (E_INK_HEIGHT - (qrcode.size * size)) / 2;  // center QR code vertically
    uint32_t x = (E_INK_WIDTH - (qrcode.size * size) - 100); // 100 px padding on right side

    displayStart();
    display.selectDisplayMode(INKPLATE_1BIT);
    display.setTextColor(BLACK, WHITE); // Set text color to black on white
    display.setFont(&Roboto_64);
    display.setTextSize(1);
    display.clearDisplay();
    displayEnd();

    renderQR(qrcode, x, y, size);

    // y = y + 100; // lower text a little
    // // 100px padding on each size, 20px padding between text
    // uint16_t h = centerTextX("WiFi", 100, x - 100, y);
    // y = y + 60;
    // h = centerTextX(QR_WIFI_NAME, 100, x - 100, y + h + 30);
    // h = centerTextX(QR_WIFI_PASSWORD, 100, x - 100, y + (h + 30) * 2);

    i2cStart();
    displayStart();
    // centerTextX("WiFi", 100, x - 100, y);
    display.drawRect(x - 10, y - 10, (qrcode.size * size) + 20, (qrcode.size * size) + 20, BLACK);
    display.display();
    displayEnd();
    i2cEnd();
}

void renderQR(QRCode qrcode, uint32_t x, uint32_t y, uint32_t size)
{
    displayStart();
    // set correct color based on display mode
    uint16_t foreground = BLACK;
    uint16_t background = WHITE;
    if (display.getDisplayMode() == INKPLATE_3BIT)
    {
        foreground = C_BLACK;
        background = C_WHITE;
    }

    // set the correct pixels
    for (uint8_t j = 0; j < qrcode.size; j++)
    {
        // Each horizontal module
        for (uint8_t i = 0; i < qrcode.size; i++)
        {
            // use filled rect to scale the image
            display.fillRect(x + (i * size), y + (j * size), size, size, qrcode_getModule(&qrcode, i, j) ? foreground : background);
        }
    }
    displayEnd();
}
