#include <Arduino.h>
#include "WiFi.h"

#include "config.h"
#include "http_camera.h"
#include "ntp.h"
#include "sleep.h"

void setup()
{
  delay(1000);
  Serial.begin(9600);

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

  ntp::sync_clock_from_ntp();
  http_camera::take_and_send_image();

  WiFi.mode(WIFI_OFF);
  btStop();

  device_sleep::go_to_sleep();
}

void loop()
{
}