/*ESP32
dejan.rakijasic@gmail.com

PRIMJER: light-sleep mode

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
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_sleep.h>

void app_main() {
    // Konfiguracija RTC timera za buđenje nakon 10 sekundi
    esp_sleep_enable_timer_wakeup(10 * 1000000);

    while (1) {
        printf("ESP32 odlazi u light-sleep mod..\n");
        // Unutar light-sleep moda, ESP32 će zaspati i probuditi se nakon 10 sekundi
        esp_light_sleep_start();
        // Ovaj kod će se izvršiti nakon buđenja iz light-sleep moda
        printf("ESP32 izlazi is light-sleep moda i ponovno ulazak...\n");
    }
}
