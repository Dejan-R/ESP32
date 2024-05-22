/*ESP32
dejan.rakijasic@gmail.com*/


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

/*Definiramo queue Queue_primjer kao globalnu varijablu vidljivu u oba task-a*/
QueueHandle_t queue_interrupt;

static void IRAM_ATTR gpio_isr_handler(void *args)
{
  int pin_stanje = (int)args;
  xQueueSendFromISR(queue_interrupt, &pin_stanje, NULL);
}

void LED_control(void *pvParameters)
{
  int pin_stanje = 0;
  for (;;)
  {

    if (xQueueReceive(queue_interrupt, &pin_stanje, portMAX_DELAY) == pdTRUE)
    {
      // Ako je primljena vrijednost preko queue-a 
      gpio_set_level(LED2, pin_stanje);
    }
  }
}

void app_main()
{
  // Inicijalizira se queue_primjer (stvara se instanca), cija je velicina 1 bajt
  queue_interrupt = xQueueCreate(10, sizeof(int));

  // esp_rom_gpio_pad_select_gpio(LED1);
  esp_rom_gpio_pad_select_gpio(LED2);
  // gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED2, GPIO_MODE_OUTPUT);

  gpio_set_direction(tipka, GPIO_MODE_INPUT);
  gpio_pullup_en(tipka);
  gpio_pulldown_dis(tipka);
  gpio_install_isr_service(0);

  gpio_set_intr_type(tipka, GPIO_INTR_POSEDGE);

  gpio_isr_handler_add(tipka, gpio_isr_handler, (void *)tipka);

  // xTaskCreate(provjera_tipke, "šalje poruku", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
  xTaskCreate(LED_control, "prima poruku", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
}