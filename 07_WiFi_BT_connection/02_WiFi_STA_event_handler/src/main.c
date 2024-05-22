/*dejan.rakijasic@gmail.com


Primjer: WiFi - STA - dodana funkcija event handler

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
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "wifi.h" 

void app_main(void)
{
   ESP_ERROR_CHECK(nvs_flash_init()); 
   /*esp_err_t - tip podataka (ESP-IDF) za predstavljanje grešaka ili statusa funkcija
      - u ovom slučaju vraća ESP_OK ako je WiFi povezan
  */
esp_err_t wifi_rezultat = wifi_STA_povezivanje("mreza", "12345678");
if (wifi_rezultat == ESP_OK)
{
    printf("Uređaj je povezan na WiFi!\n");
}
else
{
    printf("Povezivanje na WiFi nije uspjelo!\n");
}
}
