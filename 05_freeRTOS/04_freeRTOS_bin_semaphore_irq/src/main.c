/*ESP32
dejan.rakijasic@gmail.com

BINARNI semafor - koristi se za sinkronizaciju zadataka, te za sinkronizaciju prekidne rutine(IRQ) u zadatku
Njegova vrijednost može biti '1' - dostupan ili '0' - nedostupan (zauzet drugim zadatkom).
Binarni semafor može se koristiti za nekoliko zadataka, ali samo jedan zadatak ga može dobiti odjednom.
Binarni semafor se obično koristi za situacije gdje samo jedan zadatak treba pristupiti određenom resursu u jednom trenutku.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/semphr.h"

// #define LED1 18
#define LED2 19
#define tipka 15
#define ESP_INR_FLAG_DEFAULT 0

TaskHandle_t ISR = NULL;

SemaphoreHandle_t binarni_semafor;

void IRAM_ATTR button_isr_handler(void *arg)
{
  xSemaphoreGiveFromISR(binarni_semafor, NULL);
  xTaskResumeFromISR(ISR);
}

void interrupt_task(void *arg)
{

  while (1)
  {
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void toggleLED2(void *pvParameters)
{
  int stanje = 0;
  /* Pokusava se zauzeti semafor 'binarni_semafor', odnosno ovdje čeka zauvijek dok se ga ne zauzme,
 što određuje parametar portMAX_DELAY. Umjesto njega se moze staviti odredeno vrijeme
 kao kod pozivanja funkcije vTaskDelay(). */
  for (;;)
  {

    if (xSemaphoreTake(binarni_semafor, portMAX_DELAY) == pdTRUE)
    {
      stanje = !stanje;
      gpio_set_level(LED2, stanje);
    }
  }
}

void app_main()
{
  // Inicijalizacija semafora 'binarni_semafor' (stvara se instanca)
  binarni_semafor = xSemaphoreCreateBinary();

  // esp_rom_gpio_pad_select_gpio(LED1);
  esp_rom_gpio_pad_select_gpio(LED2);
  // gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED2, GPIO_MODE_OUTPUT);

  gpio_set_direction(tipka, GPIO_MODE_INPUT);
  gpio_pullup_en(tipka);
  gpio_pulldown_dis(tipka);

  gpio_set_intr_type(tipka, GPIO_INTR_POSEDGE);
  gpio_install_isr_service(ESP_INR_FLAG_DEFAULT);
  gpio_isr_handler_add(tipka, button_isr_handler, NULL);

  xTaskCreate(toggleLED2, "zauzima semafor", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

  xTaskCreate(interrupt_task, "otpuštanje semafora iz interrupta", 4096, NULL, 10, &ISR);
}