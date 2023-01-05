#include <qrcode.h>
#include "homeplate.h"
#include "qr.h"

void renderQR(QRCode qrcode, uint32_t x, uint32_t y, uint32_t size);

void displayWiFiQR(uint32_t x, uint32_t y)
{
    Serial.printf("Rendering wifi QR Code\n");
    char buf[1024];
    snprintf(buf, 1024, "WIFI:S:%s;T:WPA;P:%s;;", QR_WIFI_NAME, QR_WIFI_PASSWORD);
    // Create the QR code
    static uint8_t version = 5;
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(version)];
    qrcode_initText(&qrcode, qrcodeData, version, ECC_MEDIUM, buf);
    uint32_t size = 3;

    uint32_t y_1 = y - ((qrcode.size * size) / 2); 
    uint32_t x_1 = x - ((qrcode.size * size) /2); 
    renderQR(qrcode, x_1, y_1, size);

    display.drawRect(x_1 - 10, y_1 - 10, (qrcode.size * size) + 20, (qrcode.size * size) + 20, BLACK);
}

void renderQR(QRCode qrcode, uint32_t x, uint32_t y, uint32_t size)
{
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
}
