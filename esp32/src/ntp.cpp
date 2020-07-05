#include <ntp.h>

using namespace ntp;

void ntp::sync_clock_from_ntp()
{
    const char *ntpServer = "pool.ntp.org";
    const long gmtOffset_sec = 0;
    const int daylightOffset_sec = 3600;
    struct tm timeinfo;

    //https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("NTP sync failed");
    }
    else
    {
        Serial.println("NTP sync succesful");
    }
}