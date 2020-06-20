#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include "WiFi.h"
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <time.h>
#include <HTTPClient.h>
#include "driver/rtc_io.h"

#include "config.h"
#include "plant_camera.h"

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;
const int ota_pin = 15;

struct tm timeinfo;
bool ota_disabled = false;

void setup()
{
  delay(1000);

  //pinMode(ota_pin, INPUT);

  Serial.begin(9600);
  //Serial.println(esp_sleep_get_wakeup_cause());

  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // === CONNECT WIFI ===

  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("Wifi connecting");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  Serial.println("Wifi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());

  // === GET NTP TIME ===

  // https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/
  // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  // if (!getLocalTime(&timeinfo))
  // {
  //   Serial.println("Time failed");
  //   return;
  // }
  // else
  // {
  //   Serial.print("NTP time: ");
  //   Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  // }

  // === CONFIG OTA ===

  // https://docs.platformio.org/en/latest/platforms/espressif32.html#using-built-in-local-solution
  // ArduinoOTA
  //     .onStart([]() {
  //       String type;
  //       if (ArduinoOTA.getCommand() == U_FLASH)
  //         type = "sketch";
  //       else // U_SPIFFS
  //         type = "filesystem";

  //       // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
  //       Serial.println("Start updating " + type);
  //     })
  //     .onEnd([]() {
  //       Serial.println("\nEnd");
  //     })
  //     .onProgress([](unsigned int progress, unsigned int total) {
  //       Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  //     })
  //     .onError([](ota_error_t error) {
  //       Serial.printf("Error[%u]: ", error);
  //       if (error == OTA_AUTH_ERROR)
  //         Serial.println("Auth Failed");
  //       else if (error == OTA_BEGIN_ERROR)
  //         Serial.println("Begin Failed");
  //       else if (error == OTA_CONNECT_ERROR)
  //         Serial.println("Connect Failed");
  //       else if (error == OTA_RECEIVE_ERROR)
  //         Serial.println("Receive Failed");
  //       else if (error == OTA_END_ERROR)
  //         Serial.println("End Failed");
  //     });

  // ArduinoOTA.begin();

  // OTA is mainly used for dev purposes
  // if (digitalRead(ota_pin) != HIGH)
  // {
  //   ota_disabled = true;
  //   Serial.println("OTA disabled");
  // }
  // else
  // {
  //   Serial.println("OTA enabled");
  // }

  // if (ota_disabled)
  // {
  // File picture = take_image(FILE_PHOTO);
  // if (picture)
  // {
  // === SEND PICTURE OVER HTTP ===

  //randomnerdtutorials.com/esp32-http-get-post-arduino/#http-post

  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else
  {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }

  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // OV2640 camera module
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound())
  {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  }
  else
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }

  camera_fb_t *fb = NULL; // pointer

  // Take a photo with the camera
  Serial.println("Taking a photo...");

  fb = esp_camera_fb_get();
  if (!fb)
  {
    Serial.println("Camera capture failed");
  }

  HTTPClient http;
  http.setAuthorization("test", "user");
  String serverPath = "http://192.168.0.109:3001/api/images";

  // char isoTimeString[200];
  // strftime(isoTimeString, sizeof(isoTimeString), "%FT%T+03:00", &timeinfo);

  http.begin(serverPath.c_str());
  http.addHeader("Content-Type", "application/octet-stream");
  http.addHeader("Picture-FileName", "esp");
  int httpResponseCode = http.sendRequest("POST", fb->buf, fb->len);

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();

  // === GO SLEEP ===

  // struct tm next_time = timeinfo;
  // // next_time.tm_mday = timeinfo.tm_mday == 31 ? 1 : next_time.tm_mday + 1;
  // // next_time.tm_hour = 13 - 2; // too lazy to set timezone
  // // next_time.tm_min = 0;
  // // next_time.tm_sec = 0;
  // next_time.tm_min = timeinfo.tm_min + 2; // dev shortcut

  // double sleep_duration_seconds = (double)difftime(mktime(&next_time), mktime(&timeinfo));
  // Serial.println(&next_time, "Next wake-up %A, %B %d %Y %H:%M:%S");

  // WiFi.mode(WIFI_OFF);
  // btStop();

  // delay(1000);
  // esp_deep_sleep(sleep_duration_seconds * 1000000);
  // }
  // }
}

void loop()
{
  if (!ota_disabled)
  {
    ArduinoOTA.handle();
  }
}
