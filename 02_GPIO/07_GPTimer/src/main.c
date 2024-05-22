/*ESP32
dejan.rakijasic@gmail.com

Primjer: General Purpose Timer (GPTimer)
hardverski, općenamjenski timer ESP32 periferije. 
Koristi se za: 
- 'Free running as a wall clock' - dohvaćanje vremenske oznake visoke razlučivosti u bilo koje vrijeme i na bilo kojem mjestu.
- 'Generate period alarms' - generiranje periodičnih alarma
- 'Generate one-shot alarm' - generiranje jednokratnog alarma (vrijednost koja je postvljena timeru)

U ovom jednostavnom primjeru GPTimer periodično toggla LED na GPIO19.

detaljnije informacije:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gptimer.html#_CPPv421gptimer_get_raw_count16gptimer_handle_tP8uint64_t
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"

#define LED_PIN GPIO_NUM_19

void app_main() {
        // Konfiguracije LED pin kao izlaz
    esp_rom_gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // Konfiguracija GPTimera
gptimer_handle_t gptimer = NULL;
gptimer_config_t timer_config = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
};
ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

ESP_ERROR_CHECK(gptimer_enable(gptimer));
ESP_ERROR_CHECK(gptimer_start(gptimer));

uint64_t count;
uint64_t prev_count = 0;
int led_state = 0;
    while(1) {
        // Ispisuje vrijednost timera
        ESP_ERROR_CHECK(gptimer_get_raw_count(gptimer, &count));
        printf("Vrijednost timera: %llu\n", count);

        // Provjera je li prošla sekunda
        if ((count - prev_count) >= timer_config.resolution_hz) {
            printf("Prošla je sekunda.\n");
            led_state = !led_state;
            gpio_set_level(LED_PIN, led_state);// Toggle LED
            prev_count = count;   // Postavljanje trenutne vrijednost kao prethodne za sljedeću iteraciju
        }
        vTaskDelay(500 / portTICK_PERIOD_MS); //kašnjenje
    }
}