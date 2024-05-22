/*ESP32
dejan.rakijasic@gmail.com

MUTEX - semafor:
Ispravna uporaba semafora je za signaliziranje s jednog zadatka na drugi.
Mutex je namijenjen za preuzimanje i otpuštanje od strane svakog zadatka koji koristi zajednički resurs.
Zadaci koji koriste semafore - ili signaliziraju ili čekaju (ne oboje)

varijabla 'dijeljena_varijabla' koristi se u zadacima task1 i task2  u kojima se izmjenjuje njena vrijednost.
Kada zadaci preuzmu MUTEX, imaju ekskluzivan pristup dijeljenom varijablom (mutex služi kao mehanizam za zaključavanje resursa)
Nakon završetka promjene varijable, MUTEX se oslobađa kako bi drugi zadatak mogao preuzeti resurs.
Korištenje MUTEX-a osigurava da samo jedan zadatak može mijenjati varijablu 'dijeljena_varijabla' u određenom trenutku,
što sprječava utrke za resursom, tj. osigurava sinkronizaciju pristupa dijeljenom resursu.
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

SemaphoreHandle_t mutex;

int dijeljena_varijabla = 0;

void task1(void *pvParameters)
{
  while (1)
  {
    xSemaphoreTake(mutex, portMAX_DELAY);
    dijeljena_varijabla++; // Promjena dijeljene varijable
    printf("Task1: dijeljena_varijabla = %d\n", dijeljena_varijabla);
    xSemaphoreGive(mutex);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void task2(void *pvParameters)
{
  while (1)
  {
    xSemaphoreTake(mutex, portMAX_DELAY);
    dijeljena_varijabla--; // Promjena dijeljene varijable
    printf("Task2: dijeljena_varijabla = %d\n", dijeljena_varijabla);
    xSemaphoreGive(mutex);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void app_main()
{
  mutex = xSemaphoreCreateMutex();
  xTaskCreate(task1, "task1", 2048, NULL, 1, NULL);
  xTaskCreate(task2, "task2", 2048, NULL, 1, NULL);
}