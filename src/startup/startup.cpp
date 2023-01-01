#include <Inkplate.h>
#include "homeplate.h"
#include "startup/startup.h"

void printChipInfo()
{
    /* Print chip information */
    Serial.printf("ESP Chip information:\n");
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    Serial.printf("This is %s chip with %u CPU core(s), WiFi%s%s, ",
                  CONFIG_IDF_TARGET,
                  chip_info.cores,
                  (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
                  (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    Serial.printf("silicon revision %u, ", chip_info.revision);

    Serial.printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
                  (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    Serial.printf("Minimum free heap size: %u bytes\n", esp_get_minimum_free_heap_size());

    heap_caps_print_heap_info(MALLOC_CAP_32BIT | MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM | MALLOC_CAP_INTERNAL);
}

void splashScreen()
{
    static const char *splashName = "HomeView";
    displayStart();
    display.selectDisplayMode(INKPLATE_1BIT); // testing
    display.setTextColor(BLACK, WHITE);       // Set text color to black on white
    display.setFont(&Roboto_128);
    display.setTextSize(1);

    // Roboto_64, size: 1, center (439, 437)
    // Roboto_64, size: 2, center (279, 461)
    // Roboto_128, size: 1, center (285, 461)
    int16_t x = 285;
    int16_t y = 461;
    bool dynamicPlacement = false;
    if (dynamicPlacement)
    {
        // get text size for box
        int16_t x1, y1;
        uint16_t w, h;
        display.getTextBounds(splashName, 100, 100, &x1, &y1, &w, &h);
        x = (E_INK_WIDTH - w) / 2;
        y = (E_INK_HEIGHT - h) / 2 + h;
        Serial.printf("SplashScreen location (%d, %d)\n", x, y);
    }

    display.setCursor(x, y);

    // text to print over box
    display.print(splashName);
    displayEnd();
    i2cStart();
    displayStart();
    display.partialUpdate(sleepBoot);
    displayEnd();
    i2cEnd();
}
