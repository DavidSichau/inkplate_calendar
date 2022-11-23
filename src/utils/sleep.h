#include <Inkplate.h>

// Sleep
#define SLEEP_TIMEOUT_SEC 15

// Sleep
#define TIME_TO_SLEEP_SEC (TIME_TO_SLEEP_MIN * 60) // How long ESP32 will be in deep sleep (in seconds)
#define TIME_TO_QUICK_SLEEP_SEC 5 * 60             // 5 minutes. How long ESP32 will be in deep sleep (in seconds) for short activities
void startSleep();
void setSleepDuration(uint32_t sec);
void gotoSleepNow();
void sleepTask();
void delaySleep(uint seconds);
