/*ESP32
dejan.rakijasic@gmail.com
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

// #define LED1 18
#define LED2 19
#define tipka 15

/*Definiramo queueu Queue_primjer kao globalnu varijablu vidljivu u oba task-a*/
QueueHandle_t queue_primjer;

void provjera_tipke(void *pvParameters)
{
  uint8_t brojac = 0;
  for (;;)
  {
    if ((gpio_get_level(tipka)) == 0)
    {
      brojac++;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
    xQueueSend(queue_primjer, &brojac, portMAX_DELAY);
  }
}

void toggleLED2(void *pvParameters)
{
  uint8_t stanje = 0;
  uint8_t poruka = 0;
  for (;;)
  {

    if (xQueueReceive(queue_primjer, &poruka, portMAX_DELAY) == pdTRUE)
    {
      // Ako je primljena vrijednost preko queue-a jednaka 5
      if (poruka == 5)
      {
        stanje = !stanje;
        gpio_set_level(LED2, stanje);
      }
    }
  }
}

void app_main()
{
  // Inicijalizira se queue_primjer (stvara se instanca), cija je velicina 1 bajt
  queue_primjer = xQueueCreate(1, sizeof(uint8_t));

  // esp_rom_gpio_pad_select_gpio(LED1);
  esp_rom_gpio_pad_select_gpio(LED2);
  // gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED2, GPIO_MODE_OUTPUT);

  gpio_set_direction(tipka, GPIO_MODE_INPUT);
  gpio_pullup_en(tipka);
  gpio_pulldown_dis(tipka);

  xTaskCreate(provjera_tipke, "šalje poruku", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
  xTaskCreate(toggleLED2, "prima poruku", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
}