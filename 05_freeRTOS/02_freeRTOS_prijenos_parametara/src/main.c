/*ESP32
dejan.rakijasic@gmail.com

Primjer: freeRTOS - prijenos parametara
U ovom primjeru, kreiramo dva zadatka (task1 i task2). Svaki zadatak ima svoju funkciju koja se izvršava kada se zadatak pokrene. 
Parametri se prenose putem pokazivača na memoriju koja sadrži vrijednost parametra. U primjeru, prvi zadatak prima cijeli broj (int),
dok drugi zadatak prima niz znakova (char[]).
Funkcija xTaskCreate se koristi za stvaranje zadatka, a njeni argumenti uključuju funkciju zadatka, naziv zadatka, stog zadatka, 
parametar zadatka, prioritet i ručku zadatka (u ovom primjeru, postavljamo ručku na NULL).

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Funkcija prvog zadatka
void task1(void *parameter) {
    int task_param = *((int *)parameter);

    while (1) {
        printf("Zadatak 1 - Parametar: %d\n", task_param);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// Funkcija drugog zadatka
void task2(void *parameter) {
    char *task_param = (char *)parameter;

    while (1) {
        printf("Zadatak 2 - Parametar: %s\n", task_param);
        vTaskDelay(1500 / portTICK_PERIOD_MS);
    }
}

void app_main() {
    // Postavljanje parametara za zadatke
    int param1 = 42;
    char param2[] = "Hello, ESP32!";

    // Kreiranje prvog zadatka
    xTaskCreate(task1, "Task 1", 2048, &param1, 2, NULL);

    // Kreiranje drugog zadatka
    xTaskCreate(task2, "Task 2", 2048, &param2, 2, NULL);

    // Glavna petlja
    while (1) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
