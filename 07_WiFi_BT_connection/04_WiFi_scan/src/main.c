/*dejan.rakijasic@gmail.com


Primjer: WiFi - scan

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
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"

void wifi_inicijalizacija()
{
  nvs_flash_init();
  esp_netif_init();
  esp_event_loop_create_default();

  wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&wifi_config);
  esp_wifi_set_mode(WIFI_MODE_STA);
esp_wifi_start();
}

static char *getAuthModeName(wifi_auth_mode_t auth_mode)
{
  char *names[] = {"OPEN", "WEP", "WPA PSK", "WPA2 PSK", "WPA WPA2 PSK", "MAX"};
  return names[auth_mode];
}

void app_main()
{
  wifi_inicijalizacija();

  wifi_scan_config_t scan_konfiguracija = {
      .ssid = 0,
      .bssid = 0,
      .channel = 0,
      .show_hidden = true};

  esp_wifi_scan_start(&scan_konfiguracija, true);

  wifi_ap_record_t wifi_spremljeno[10]; //max broj skeniranih mreža

  uint16_t max_scan_broj = 10;
  esp_wifi_scan_get_ap_records(&max_scan_broj, wifi_spremljeno);

    printf("Skenirane mreže:\n");
    for (int i = 0; i < max_scan_broj; i++)
    {
        printf("SSID: %s, Signal: %d dBm, Auth Mode: %s\n",
               (char *)wifi_spremljeno[i].ssid,
               wifi_spremljeno[i].rssi,
               getAuthModeName(wifi_spremljeno[i].authmode));
    }

    esp_wifi_stop();
    esp_wifi_deinit();
}
