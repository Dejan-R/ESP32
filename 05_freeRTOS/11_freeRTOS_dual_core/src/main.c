/*ESP32
dejan.rakijasic@gmail.com

Primjer: freeRTOS dual core
Primjer korištenja obje jezgre ESP32 tako da svaka jezgra zasebno upravlja svojim taskom u kojemu toggla LED.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED1 18
#define LED2 19

// Definiranje taska za svaku jezgru
void core1_task(void *pvParameters) {
  int led_state = 0;
  for (;;)
  {
    printf("Core 1 toggling LED\n");
    led_state = !led_state;
    gpio_set_level(LED1, led_state);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void core2_task(void *pvParameters) {
  int led_state = 0;
  for (;;)
  {
    printf("Core 2 toggling LED\n");
    led_state = !led_state;
    gpio_set_level(LED2, led_state);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void app_main() {
    esp_rom_gpio_pad_select_gpio(LED1);
    gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
    esp_rom_gpio_pad_select_gpio(LED2);
    gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
      // Kreiranje taska za svaku jezgru
    xTaskCreatePinnedToCore(&core1_task, "Core 1 Task", 2048, NULL, 5, NULL, 0); //jezgra 1
    xTaskCreatePinnedToCore(&core2_task, "Core 2 Task", 2048, NULL, 5, NULL, 1); //jezgra 0
}
