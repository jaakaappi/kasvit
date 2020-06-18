#include "plant_camera.h"

/*
    Mounts SPIFFS, configures camera, inits camera, takes picture, saves into SPIFFS

    @param filename Picture filename in SPIFFS.

    @return Picture File object

    Based on https://randomnerdtutorials.com/esp32-cam-take-photo-display-web-server/
*/
File take_image(String filename)
{
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
    bool ok = 0;            // Boolean indicating if the picture has been taken correctly

    do
    {
        // Take a photo with the camera
        Serial.println("Taking a photo...");

        fb = esp_camera_fb_get();
        if (!fb)
        {
            Serial.println("Camera capture failed");
            return SPIFFS.open("", FILE_READ);
        }

        // Photo file name
        Serial.printf("Picture file name: %s\n", filename);
        File file = SPIFFS.open(filename, FILE_WRITE);

        // Insert the data in the photo file
        if (!file)
        {
            Serial.println("Failed to open file in writing mode");
        }
        else
        {
            file.write(fb->buf, fb->len); // payload (image), payload length
            Serial.print("The picture has been saved in ");
            Serial.print(filename);
            Serial.print(" - Size: ");
            Serial.print(file.size());
            Serial.println(" bytes");
        }
        // Close the file
        file.close();
        esp_camera_fb_return(fb);

        // check if file has been correctly saved in SPIFFS
        File f_pic = SPIFFS.open(filename);
        unsigned int pic_sz = f_pic.size();
        ok = pic_sz > 100;
    } while (!ok);

    return SPIFFS.open(filename);
}