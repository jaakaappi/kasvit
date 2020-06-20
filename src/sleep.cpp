#include <sleep.h>

using namespace device_sleep;

void device_sleep::go_to_sleep()
{
    struct tm current_time;
    double sleep_duration_seconds = 86340; // 23h 59min, fallback if ntp is unavailable

    if (getLocalTime(&current_time))
    {
        struct tm next_time = current_time;
        // next_time.tm_mday = current_time.tm_mday == 31 ? 1 : next_time.tm_mday + 1;
        // next_time.tm_hour = 13 - 2; // too lazy to set timezone
        // next_time.tm_min = 0;
        // next_time.tm_sec = 0;
        next_time.tm_min = current_time.tm_min + 2; // dev shortcut
        Serial.println(&next_time, "Next wake-up %A, %B %d %Y %H:%M:%S");
        sleep_duration_seconds = (double)difftime(mktime(&next_time), mktime(&current_time));
    }
    delay(1000);
    esp_deep_sleep(sleep_duration_seconds * 1000000);
}