/*dejan.rakijasic@gmail.com


Primjer: WiFi - STA

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

#define WIFI_SSID "mreza"
#define WIFI_PASS "12345678"

void wifi_sta() {
 esp_netif_init();
    esp_event_loop_create_default();     
    esp_netif_create_default_wifi_sta(); 

    wifi_init_config_t konfiguracija = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&konfiguracija);

    wifi_config_t wifi_STA_povezivanje = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_STA_povezivanje);
    esp_wifi_start();
    esp_wifi_connect();

}

void app_main() {
    nvs_flash_init();
    esp_log_level_set("wifi", ESP_LOG_NONE);
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_sta();
}