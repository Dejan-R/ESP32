/*ESP32
dejan.rakijasic@gmail.com

Primjer: COUNTING SEMAPHORE

U ovom primjeru kreirana su 3 zadatka (task). Svaki task pokušava zauzeti semafor (kontrolira pristup resursima) pomoću xSemaphoreTake.
Ako uspije, pristupa resursu, spava 1 sekundu, oslobađa semafor pomoću xSemaphoreGive, i spava dodatnih 0.5 sekundi. 
Ako ne uspije odmah dobiti semafor, čeka (portMAX_DELAY omogućava čekanje do beskonačnosti).
Sva tri zadatka mogu istovremeno pristupiti resursima jer je maksimalni broj dozvoljenih pristupa postavljen na 3.

Promijenite drugi argument funkcije xSemaphore = xSemaphoreCreateCounting(3, 3); na 2, pa na 1 i provjerite što se događa.

Counting semafor - koristi se za situacije gdje više zadataka treba pristupiti ograničenom broju dostupnih resursa - može imati vrijednost veću od 1 i može kontrolirati pristup više od jednog zadatka, može  se inicijalizirati na određenu početnu vrijednost (cjelobrojna vrijednost veća od nule)
Kad zadaci zauzmu counting semaphore, vrijednost se smanjuje, a kad ga oslobode, vrijednost semafora se povećava. 
Vrijednost counting semapfore (veća od nula) - označava da broj zadataka koji mogu dobiti pristup dijeljenom resursu odjednom.

*/


#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

SemaphoreHandle_t xSemaphore;

void task1(void *pvParameters) {
    while (1) {
        // Pokušaj dohvata semafora
       if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE){
            printf("Task 1 pristupa resursu 1\n");
              vTaskDelay(1000 / portTICK_PERIOD_MS);
             // Oslobađanje semafora nakon završetka pristupa resursu
            xSemaphoreGive(xSemaphore);
            //kašnjenje prije nego što se semafor oslobodi, kako bi drugi zadaci dobili mogućnost pristupa semaforu
                vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
}

void task2(void *pvParameters) {
    while (1) {
            if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE){
            printf("Task 2 pristupa resursu 2\n");
                   vTaskDelay(1000 / portTICK_PERIOD_MS);
           // Oslobađanje semafora nakon završetka pristupa resursu
            xSemaphoreGive(xSemaphore);
             //kašnjenje prije nego što se semafor oslobodi, kako bi drugi zadaci dobili mogućnost pristupa semaforu
                  vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
}

void task3(void *pvParameters) {
    while (1) {
       if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE){
            printf("Task 3 pristupa resursu 3\n");
              vTaskDelay(1000 / portTICK_PERIOD_MS);
            // Oslobađanje semafora nakon završetka pristupa resursu
            xSemaphoreGive(xSemaphore);
             //kašnjenje prije nego što se semafor oslobodi, kako bi drugi zadaci dobili mogućnost pristupa semaforu
                  vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
}

void app_main() {
    /*Funkcija xSemaphoreCreateCounting koristi se za stvaranje counting semafora
     - prvi argument - početna vrijednost semafora
     - drugi argument - max vrijednost semafora, istovremeno tri taska mogu dobiti pristup resursima.
      Ako sva tri taska zauzmu semafor, sljedeći task koji pokuša dohvatiti semafor će morati čekati dok se barem jedan task ne vrati semafor.*/
    xSemaphore = xSemaphoreCreateCounting(3, 3);

    if (xSemaphore != NULL) {
        // Tri taska koji pristupaju različitim resursima
        xTaskCreate(task1, "Task 1", 2048, NULL, 5, NULL);
        xTaskCreate(task2, "Task 2", 2048, NULL, 5, NULL);
        xTaskCreate(task3, "Task 3", 2048, NULL, 5, NULL);
    }
}
