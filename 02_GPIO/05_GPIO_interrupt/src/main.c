/*ESP32
dejan.rakijasic@gmail.com

Primjer: GPIO interrupt

Korištenje prekida bez RTOS-a može biti prikladno za jednostavne aplikacije ili one s niskim zahtjevima, 
ali za složenije aplikacije ili one koje zahtijevaju višezadaćnost, sinkronizaciju ili bolje upravljanje resursima, 
upotreba RTOS-a je preporučljivija. RTOS pruža ugrađene mehanizme za upravljanje prekidima, sinkronizaciju niti i 
resursa te olakšava razvoj složenijih sustava.*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED 19
#define tipka 15
#define ESP_INR_FLAG_DEFAULT 0

volatile int led_state = 0;
void IRAM_ATTR gpio_isr_handler(void* arg) {
    // prekidna rutina
    // Obrada stanja na GPIO pinu
    led_state = !led_state; // Promijeni stanje LED-a
    gpio_set_level(LED, led_state); // Postavi stanje LED-a

}

void app_main() {
  gpio_set_direction(tipka, GPIO_MODE_INPUT);
  gpio_pullup_en(tipka);
  gpio_pulldown_dis(tipka);


//konfiguracija prekida
  gpio_set_intr_type(tipka, GPIO_INTR_POSEDGE); //prekid na rastući brid
  gpio_install_isr_service(ESP_INR_FLAG_DEFAULT); 
  gpio_isr_handler_add(tipka, gpio_isr_handler, NULL); 

  esp_rom_gpio_pad_select_gpio(LED);
  gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    while (1) {
        // Glavna petlja
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
