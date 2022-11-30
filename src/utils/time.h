#pragma once

void setupTimeAndSyncTask();

bool getNTPSynced();

time_t changeTimeByDay(int dayChange, time_t base);

char *timeString();

char *fullDateString();

char *composeDateChar2();

char *getWeekday(uint32_t utc);

char *timeFromUnixString(uint32_t utc);

int getHour(uint32_t utc);

static const char *lang_day[][7] = {

    // English
    {"Mo", "Tu", "We", "Th", "Fr", "Sa", "So"},

    // German
    {"Mo", "Di", "Mi", "Do", "Fr", "Sa", "So"},

};