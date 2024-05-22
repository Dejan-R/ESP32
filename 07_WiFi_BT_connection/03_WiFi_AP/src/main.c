/*dejan.rakijasic@gmail.com


Primjer: WiFi - (Soft)AP

WiFi biblioteke pružaju podršku za konfiguriranje i nadzor ESP32 WiFi mrežne funkcije za:

- STA način rada ili WiFi klijentski način rada - ESP32 se povezuje s pristupnom točkom
- AP način rada (Soft-AP način rada ili Access Point način rada) - stanice se povezuju s ESP32
- STA/AP - ESP32 je istodobno pristupna točka i stanica povezana s drugom pristupnom točkom
- različiti sigurnosni načini za gore navedeno (WPA, WPA2, WPA3 itd.)
- traženje pristupnih točaka (aktivno i pasivno skeniranje).
- način praćenja IEEE802.11 Wi-Fi paketa.

Za više infromacija:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_wifi.html
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#define AP_SSID "ESP32"
#define AP_PASS "12345678"

void wifi_init_softap() {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = AP_SSID,
            .password = AP_PASS,
            .max_connection = 5, // postavke za max broj spojenih uređaja
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
    esp_wifi_start();
}

void app_main() {
        nvs_flash_init();
    esp_log_level_set("wifi", ESP_LOG_NONE);
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_softap();
}