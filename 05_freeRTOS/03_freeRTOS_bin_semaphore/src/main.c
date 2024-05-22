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

SemaphoreHandle_t binarni_semafor;

void provjera_tipke(void *pvParameters)
{
  for (;;)
  {
    if ((gpio_get_level(tipka)) == 0)
    {
      // ako je tipka pritisnuta, otpušta semafor
      xSemaphoreGive(binarni_semafor);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void toggleLED2(void *pvParameters)
{
  int stanje = 0;
  /* Pokusava se zauzeti semafor 'binarni_semafor', odnosno ovdje čeka zauvijek dok se ga ne zauzme,
 što određuje parametar portMAX_DELAY. Umjesto njega se moze staviti odredeno vrijeme kao kod pozivanja funkcije vTaskDelay(). */
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

  xTaskCreate(provjera_tipke, "otpušta semafor", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
  xTaskCreate(toggleLED2, "zauzima semafor", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
}