/*dejan.rakijasic@gmail.com

Primjer: JSON
JSON (JavaScript Object Notation) - je tekstualni format za pohranu i prijenos podataka.
*/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"

// Funkcija za stvaranje JSON objekta
cJSON *create_json_object() {
    cJSON *json_root = cJSON_CreateObject();
    cJSON *json_data = cJSON_CreateObject();
    cJSON *json_temperature_array = cJSON_CreateArray();

    // Dodavanje podataka u JSON objekt
    cJSON_AddItemToArray(json_temperature_array, cJSON_CreateNumber(23));

    // Dodavanje podobjekta u root objekt
    cJSON_AddItemToObject(json_data, "temperatura", json_temperature_array);
    cJSON_AddItemToObject(json_root, "d", json_data);

    return json_root;
}

// Funkcija za slanje JSON podataka
void send_json_data(cJSON *json_data) {
    char *json_str = cJSON_Print(json_data);
    printf("Šaljem JSON podatke: %s\n", json_str);
    // Ovdje ide logika za slanje podataka, npr. putem MQTT-a, HTTP-a, ...
    free(json_str);
}

// Funkcija za primanje i obradu JSON podataka
void receive_and_process_json(const char *json_str) {
    cJSON *json_root = cJSON_Parse(json_str);
    if (json_root == NULL) {
        printf("Greška kod raščlambe JSON-a!\n");
        return;
    }

    cJSON *json_data = cJSON_GetObjectItem(json_root, "d");
    if (json_data != NULL) {
        cJSON *json_temperature_array = cJSON_GetObjectItem(json_data, "temperatura");
        if (json_temperature_array != NULL && cJSON_IsArray(json_temperature_array)) {
            cJSON *json_temperature = cJSON_GetArrayItem(json_temperature_array, 0);
            if (json_temperature != NULL && cJSON_IsNumber(json_temperature)) {
                printf("Primljeni podaci o temperaturi: %d\n", json_temperature->valueint);
            }
        }
    }

    cJSON_Delete(json_root);
}

void app_main() {
    // Stvaranje i slanje JSON objekta
    cJSON *json_data_to_send = create_json_object();
    send_json_data(json_data_to_send);

    //Primanje (simulacija) JSON podataka
    const char *received_json_str = "{\"d\":{\"temperatura\":[25]}}";
    receive_and_process_json(received_json_str);
}
