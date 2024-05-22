/*ESP32
dejan.rakijasic@gmail.com

Primjer: Event groups
Definirane su tri zadatka koji rade paralelno. taskSetEvent0 i taskSetEvent1 periodički postavljaju određene bitove, dok taskMonitorEvents 
čeka na oba postavljena bita i reagira kada se dogode. Event group omogućuje sinhronizaciju i komunikaciju između ovih zadataka.

Event groups - mehanizam u FreeRTOS-u koji omogućuje sinkronizaciju i komunikaciju između različitih zadataka (tasks).
https://www.freertos.org/FreeRTOS-Event-Groups.html
*/


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"


// Definiranje Event Group
EventGroupHandle_t eventGroup;

// Događaji koje ćemo pratiti
#define EVENT_0_BIT (1 << 0)
#define EVENT_1_BIT (1 << 1)

// Funkcija koja simulira neki proces koji će postaviti EVENT_0
void taskSetEvent0(void *pvParameter) {
    while (1) {
        printf("Task setting EVENT_0\n");
        xEventGroupSetBits(eventGroup, EVENT_0_BIT);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// Funkcija koja simulira neki proces koji će postaviti EVENT_1
void taskSetEvent1(void *pvParameter) {
    while (1) {
        printf("Task setting EVENT_1\n");
        xEventGroupSetBits(eventGroup, EVENT_1_BIT);
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

// Funkcija koja prati i reagira na događaje u Event Group
void taskMonitorEvents(void *pvParameter) {
    while (1) {
        EventBits_t bits = xEventGroupWaitBits(
            eventGroup,
            EVENT_0_BIT | EVENT_1_BIT,
            pdTRUE,  // očisti bitove nakon izlaska
            pdTRUE, // čekaj na sve postavljene bitove
            portMAX_DELAY);

        if (bits & EVENT_0_BIT) {
            printf("EVENT_0 received\n");
        }

        if (bits & EVENT_1_BIT) {
            printf("EVENT_1 received\n");
        }
    }
}

void app_main() {
    // Stvaranje Event Group
    eventGroup = xEventGroupCreate();

    //Stvaranje task-ova
    xTaskCreate(&taskSetEvent0, "taskSetEvent0", 2048, NULL, 1, NULL);
    xTaskCreate(&taskSetEvent1, "taskSetEvent1", 2048, NULL, 1, NULL);
    xTaskCreate(&taskMonitorEvents, "taskMonitorEvents", 2048, NULL, 2, NULL);
}
