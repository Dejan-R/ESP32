/*ESP32
dejan.rakijasic@gmail.com

Primjer: freRTOS timer 
FreeRTOS timer je softverski mehanizam koji nudi mogućnost izvođenja određenih zadataka ili prekida u određenim vremenskim intervalima. 
Koristi se unutar FreeRTOS operativnog sustava i omogućava programerima da rasporede zadatke ili prekide prema željenom vremenskom rasporedu.

Ovaj primjer koristi se za toggle LED na GPIO 19 (u funkciji toggleLED)

detaljnije informacije:
https://www.freertos.org/FreeRTOS-Software-Timer-API-Functions.html
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_19

TimerHandle_t toggle_timer;
int led_state = 0;

void toggleLED(TimerHandle_t xTimer) {
    led_state = 1 - led_state;
    gpio_set_level(LED_PIN, led_state);
}

void app_main() {
    esp_rom_gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    TimerHandle_t toggle_timer = xTimerCreate("ToggleTimer", pdMS_TO_TICKS(1000), pdTRUE, (void*)0, toggleLED); // Stvaranje FreeRTOS timera

    if (toggle_timer != NULL) {
        if (xTimerStart(toggle_timer, 0) != pdPASS) {
            printf("Greška pri pokretanju timera.\n");
        }
    }

    while (1) {
        // Ovdje možete obavljati druge zadatke
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
