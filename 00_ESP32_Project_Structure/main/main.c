/*dejan.rakijasic@gmail.com

 ESP-IDF Project Structure — primjer temeljne strukture ESP-IDF projekta:

- korištenje 'CMake build sustava',  
- rad s 'komponentama',  
- konfiguracija putem 'Kconfig' i 'menuconfig',  
- definiranje 'partition table' i 'sdkconfig' datoteka,  
- te osnovna 'FreeRTOS' strukturu (app_main).

ESP_IDF_project_structure/
│
├── CMakeLists.txt               # glavni ulaz u build sustav
├── sdkconfig                    # generirana konfiguracija iz menuconfig-a
├── sdkconfig.defaults            # zadane vrijednosti konfiguracije
├── partition_table/
│   └── partition_table.csv       # definira raspored memorije (flash particije)
│
├── main/
│   ├── main.c                    # ulazna točka programa (app_main)
│   └── CMakeLists.txt            # definira "main" komponentu
│
└── components/
    └── hello_lib/
        ├── hello.c               # izvorni kod biblioteke
        ├── hello.h               # zaglavlje biblioteke
        ├── CMakeLists.txt        # registracija komponente u ESP-IDF
        └── Kconfig               # konfiguracija za menuconfig

*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "hello.h"

static const char *TAG = "MAIN";

void hello_task(void *pvParameter)
{
    while (1) {
        ESP_LOGI(TAG, "Pozdrav iz hello_task-a!");
        hello_message();
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "ESP32 Project Structure Demo starting...");

    // Kreiranje task-a
    xTaskCreate(
        hello_task,    // funkcija taska
        "hello_task",  // naziv taska
        2048,          // veličina stacka u bajtovima
        NULL,          // parametar (nije potreban)
        5,             // prioritet
        NULL           // handle (nije potreban)
    );
}
