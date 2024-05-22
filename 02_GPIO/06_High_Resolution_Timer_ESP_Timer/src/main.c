/*ESP32
dejan.rakijasic@gmail.com

Primjer: High Resolution Timer (ESP Timer)
- koristi se za mjerenje vremena i odgađanje izvršenja zadataka u programu. Omogućuje stvaranje neovisnih softverskih mjerača vremena, 
svaki sa svojim vremenom isteka i funkcijom povratnog poziva.

Promjena stanja LED-a vrši se u funkciji toggle_led, koja se poziva svaki put kad istekne vremenski interval
definiran u esp_timer_start_periodic. U ovoj funkciji, stanje LED-a se prebacuje pomoću varijable led_state,
a zatim se postavlja na GPIO pin koristeći gpio_set_level.

detaljnije informacije:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp_timer.html?highlight=esp%20timer
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#define LED 19

void toggle_led(void *args) {
    static bool led_state = false;
    led_state = !led_state;
    gpio_set_level(LED, led_state);
}

void app_main() {
    esp_rom_gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &toggle_led,
        .name = "periodic_toggle"
    };
    esp_timer_handle_t periodic_timer;
    esp_timer_create(&periodic_timer_args, &periodic_timer);
    esp_timer_start_periodic(periodic_timer, 1000000);  // 1 second interval

    while (1) {
     /*esp_timer_dump(stdout) funkcija - koristi se za ispis informacija o svim trenutno aktivnim timerima,
         korisna je za provjeru trenutnog stanja timera i može pomoći u debagiranju problema povezanih s timerom.
        */
       esp_timer_dump(stdout);
      //  vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay 1 second
    }
}