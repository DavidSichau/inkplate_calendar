#pragma once

void setupTimeAndSyncTask();

bool getNTPSynced();

char *timeString();

char *fullDateString();

char *composeDateChar2();

char *getWeekday(uint32_t utc);

char *timeFromUnixString(uint32_t utc);

int getHour(uint32_t utc);
