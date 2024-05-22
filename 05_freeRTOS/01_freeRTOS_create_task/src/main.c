/*ESP32
dejan.rakijasic@gmail.com

Primjer: freeRTOS - kreiranje zadatka

U ovom primjeru kreirana su dva zadatka (task) 'toggleLED1' i 'toggleLED2' koji upravljaju GPIO 18 i GPIO 19 (toggle LED)

RTOS (Real Time Operating System) - operacijski sustav za rad u stvarnom vremenu. 
Softverska je platforma za raspoređivanje zadataka s vremenskim ograničenjima, tako da izgleda kao da se više programa 
izvodi u isto vrijeme na jednoj procesorskoj jezgri. freeRTOS: https://www.freertos.org/

- zadaci (tasks) mogu biti u različitim stanjima ovisno o njihovom trenutačnom izvršenju i uvjetima sustava:
RUNING – zadatak je u stanju "izvršavanje" kada trenutno izvršava svoj kod. Samo jedan zadatak može biti u stanju "izvršavanje" u određenom trenutku. Ako sustav ima više jezgri (multicore sustav), različiti zadaci mogu izvršavati svoj kod na različitim jezgrama
READY – zadaci koji se mogu, ali se ne izvode jer je drugi zadatak jednakog ili višeg prioriteta već u stanju RUNING.
BLOCKED – trenutno čeka na privremeni ili vanjski događaj. Npr. ako zadatak pozove vTaskDelay() blokirat će se (blocked) sve dok razdoblje odgode ne istekne.
SUSPENDED – zadaci se ne mogu odabrati za ulazak u stanje RUNING, isto kao i kod stanja BLOCKED, ali u ovom stanju zadaci nemaju vremensko ograničenje. U ovo stanje ulaze/izlaze putem API poziva vTaskSuspend() i xTaskResume().

KREIRANJE ZADATKA: 
- xTaskCreate() https://www.freertos.org/a00125.html

pvTaskCode- pokazivač na funkciju unosa zadatka (samo naziv funkcije koja implementira zadatak)
pcName	- naziv zadatka (uglavnom se koristi za lakše otklanjanja pogrešaka, ali i za identifikator zadatka)
usStackDepth	- broj riječi (ne bajtova!) koje je potrebno dodijeliti za stog zadatka(task).
pvParametri	- vrijednost koja će biti proslijeđena kreiranom zadatku kao parametar zadatka. Ako nema parametara, postavlja se na NULL.
uxPriority- prioritet s kojim će se stvoreni zadatak izvršiti (1-10 za opću upotrebu). tskIDLE_PRIORITY - najniži prioritet.
pxCreatedTask - koristi se za prosljeđivanje oznake kreiranom zadatku iz funkcije xTaskCreate(). pxCreatedTask nije obavezan i može se postaviti na NULL.

BRISANJE ZADATKA:
- vTaskDelete() https://www.freertos.org/a00126.html
- brisanje zadatka - programer mora osigurati da su svi resursi (memorija, resursi hardvera, itd.) koji su alocirani za taj zadatak budu ispravno oslobođeni
 kako bi se spriječile curenje resursa. U praksi, većina zadatka u embedded sustavima radi stalno, a vTaskDelete() se često koristi samo u određenim situacijama
 koje zahtijevaju dinamičko upravljanje zadacima ili specifične uvjete prekida rada zadatka.

KAŠNJENJA (DELAY):
 - koristi za obustavu izvršenja zadatka na određeno vrijeme.
 - kad jedan zadatak uđe u stanje kašnjenja (delay), drugi se zadaci pokreću i obrnuto.

 - pdMS_TO_TICKS - koristi se za konverziju vremena u 'tickove' (freeRTOS tick). 
 - portTICK_PERIOD_MS - predstavlja izravnu vrijednost dužine jednog 'ticka' u milisekundama. 
 
 - tick (tick period) - jedan ciklus "tick-a" unutar FreeRTOS kernela. Frekvencija tickova, odnosno broj tickova u jednoj sekundi, 
  definira se parametrom configTICK_RATE_HZ, ovisno o konkretnoj konfiguraciji FreeRTOS-a. Na primjer, ako je postavka configTICK_RATE_HZ
  postavljena na 1000, to znači da je FreeRTOS konfiguriran da generira 1000 tickova u jednoj sekundi. U tom slučaju, trajanje jednog ticka bilo bi 1 ms.

postavke (terminal): pio run -t menuconfig 
                             → Component config → FreeRTOS → Kernel
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED1 18
#define LED2 19

void toggleLED1(void *pvParameters)
{
  int led_state = 0;
  for (;;)
  {
    led_state = !led_state;
    gpio_set_level(LED1, led_state);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void toggleLED2(void *pvParameters)
{
  int led_state = 0;
  for (;;)
  {
    led_state = !led_state;
    gpio_set_level(LED2, led_state);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void app_main()
{
  esp_rom_gpio_pad_select_gpio(LED1);
  esp_rom_gpio_pad_select_gpio(LED2);
  gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED2, GPIO_MODE_OUTPUT);

  xTaskCreate(toggleLED1, "task_LED1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate(toggleLED2, "task_LED2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
}