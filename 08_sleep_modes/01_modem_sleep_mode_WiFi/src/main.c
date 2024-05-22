
/*ESP32
dejan.rakijasic@gmail.com

PRIMJER: modem sleep mode (WiFi)

ESP32 podržava dva glavna moda rada za uštedu energije: light sleep i deep sleep.
Ovisno o značajkama koje aplikacija koristi postoje i pod modovi štedljivog načina rada, odnosno razne mogućnosti isključivanja pojedinih komponenata hardvera
koje se mogu konfigurirati za dodatno smanjenje potrošnje energije:
- active
- modem-sleep mode - WiFi/BT-OFF
- light-sleep mode - WiFi/BT-OFF, CPU-PAUSED, ULP (ultra-low-power co-processor)-ON
- deep-sleep mode - CPU (i većina periferije) -OFF, ULP (ultra-low-power co-processor)-ON/OFF
- hibernation mode - sva periferija isključena osim RTC Timera
- power-off 
Isto tako postoji nekoliko načina (izvora) buđenja (timer, touchpad, external, ULP co-processor, GPIO, UART, Bluetooth, WiFi) iz štedljivih modova rada
koji se isto tako mogu kombinirati (da se MCU probudi ako je bilo koji od izvora buđenja aktivan)

Za više detalja: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/sleep_modes.html#api-reference-wakeup-source

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

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);

    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);  // Postavljanje WiFi Power Save mode rada
    esp_wifi_start();
    esp_wifi_connect();
}

void app_main() {
    nvs_flash_init();
    esp_log_level_set("wifi", ESP_LOG_NONE);
    esp_event_loop_create_default();
    wifi_sta();

    while (1) {
     
        // Postavljanje Power Save mode rada (modem sleep)
        ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_MAX_MODEM));
        printf("Ulaz u modem sleep...\n");

          //radnje dok je ESP32 u WiFi Power Save Mode radu (modem sleep)

        // Odspajanje s WiFi mreže
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        vTaskDelay(10000 / portTICK_PERIOD_MS);  //Čekanje 10 sekundi u WiFi Power Save mode radu (modem sleep)

        // Ponovno povezivanje s WiFi mrežom
        ESP_ERROR_CHECK(esp_wifi_connect());

        //radnje nakon ponovnog povezivanja

        // Isključivanje WiFi Power Save mode rada (modem sleep)
        ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
        printf("Izlaz iz modem sleep-a...\n");

        vTaskDelay(5000 / portTICK_PERIOD_MS);  //Čekanje 5 sekundi nakon izlaska iz WiFi Power Save mode rada (modem sleep)
    }
}