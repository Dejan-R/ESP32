/*ESP32
dejan.rakijasic@gmail.com

PRIMJER: hibernation mode

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
#include "esp_system.h"
#include "esp_sleep.h"

void app_main() {
    // Postavke za hibernaciju
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_ON);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_ON);


    // Konfiguracija RTC timera za buđenje nakon 10 sekundi
    esp_sleep_enable_timer_wakeup(10 * 1000000);
 
    // Nakon buđenja iz hibernacije, nastavite s dodatnim zadacima
    while (1) {
        // Postavljanje nekih zadataka koje ESP32 treba obaviti kad nije u hibernaciji
        printf("ESP32 radi nešto nakon buđenja iz hibernacije...\n");

        vTaskDelay(5000 / portTICK_PERIOD_MS);  // čekanje 5 sekundi 
     printf("ESP32 odlazi u hibernaciju...\n");
     //kako bi se osiguralo da su svi podaci u izlaznom bufferu ispisani prije nego što ESP32 ode u stanje mirovanja.
     fflush(stdout); 

  esp_deep_sleep_start();
    
     // Kod nakon  esp_deep_sleep_start(); nikada neće biti izvršen jer ESP32 ulazi u nisku potrošnju energije
    
    }
}
