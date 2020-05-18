#include <Arduino.h>
#include "WiFi.h"
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <time.h>

#include <config.h>

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;
const int ota_pin = 15;

struct tm timeinfo;
bool ota_disabled = false;

void setup()
{
  pinMode(ota_pin, INPUT);

  Serial.begin(9600);

  WiFi.begin(SSID, PWD);
  Serial.println("Wifi connecting");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(5000);
  }

  Serial.println("Wifi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());

  // https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Time failed");
    return;
  }
  else
  {
    Serial.print("NTP time: ");
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }

  // https://docs.platformio.org/en/latest/platforms/espressif32.html#using-built-in-local-solution
  ArduinoOTA
      .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else // U_SPIFFS
          type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
      })
      .onEnd([]() {
        Serial.println("\nEnd");
      })
      .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      })
      .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
          Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
          Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
          Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
          Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
          Serial.println("End Failed");
      });

  ArduinoOTA.begin();

  if (digitalRead(ota_pin) == HIGH)
  {
    ota_disabled = true;
    Serial.println("OTA disabled");
  }
  else
  {
    Serial.println("OTA enabled");
  }

  Serial.println("== Ready to serve! ==");
}

void loop()
{
  if (!ota_disabled)
  {
    ArduinoOTA.handle();
  }
  else
  {
    struct tm next_time = timeinfo;
    next_time.tm_mday = timeinfo.tm_mday == 31 ? 1 : next_time.tm_mday + 1;
    next_time.tm_hour = 15; // too lazy to set timezone
    next_time.tm_min = 0;
    next_time.tm_sec = 0;

    int sleep_duration_seconds = (int)difftime(mktime(&next_time), mktime(&timeinfo));
    Serial.println(&next_time, "%A, %B %d %Y %H:%M:%S");
    Serial.println(sleep_duration_seconds);

    esp_sleep_enable_timer_wakeup(sleep_duration_seconds * 1000000);
    esp_deep_sleep_start();
  }
}
