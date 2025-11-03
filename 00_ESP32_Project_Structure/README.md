# ESP-IDF Projekt — Struktura i Osnove

## ESP-IDF instalacija
- [ESP-IDF instalacija](https://dl.espressif.com/dl/esp-idf/)
---
## ESP-IDF Project Structure — primjer temeljne strukture ESP-IDF projekta

- korištenje 'CMake build sustava'  
- rad s 'komponentama'  
- konfiguracija putem 'Kconfig' i 'menuconfig'  
- definiranje 'partition table' i 'sdkconfig' datoteka  
- te osnovna 'FreeRTOS' struktura (app_main)

### Struktura projekta
```text
ESP_IDF_project_structure/
│
├── CMakeLists.txt          # glavni ulaz u build sustav
├── sdkconfig               # generirana konfiguracija iz menuconfig-a
├── sdkconfig.defaults      # zadane vrijednosti konfiguracije
├── partition_table/
│ └── partition_table.csv   # definira raspored memorije (flash particije)
│
├── main/
│ ├── main.c                # ulazna točka programa (app_main)
│ └── CMakeLists.txt        # definira "main" komponentu
│
└── components/
└── hello_lib/
├── hello.c                 # izvorni kod biblioteke
├── hello.h                 # zaglavlje biblioteke
├── CMakeLists.txt          # registracija komponente u ESP-IDF
└── Kconfig                 # konfiguracija za menuconfig
```

## 1. Build sustav (CMake, Ninja, idf.py)
---

ESP-IDF koristi **CMake** i **Ninja** kao build sustav.  
Skripta `idf.py` automatski poziva CMake, generira build direktorij i poziva Ninja da izgradi firmware.

**Primjer `CMakeLists.txt`:**

```cmake
cmake_minimum_required(VERSION 3.16)                # postavlja minimalnu verziju CMake-a
project(ESP32_project_structure)                    # definira naziv projekta
include($ENV{IDF_PATH}/tools/cmake/project.cmake)   # uključuje ESP-IDF build sustav (automatski postavlja alatni lanac, putanje i detektira komponente)
```

**idf.py** je glavni alat za rad s ESP-IDF projektom, najčešće idf.py naredbe:

```bash
idf.py set-target esp32     # postavi ciljanu platformu (esp32...)
idf.py menuconfig           # otvori konfiguracijski izbornik (Kconfig)
idf.py build                # izgradi projekt
idf.py flash                # flash-aj firmware na uređaj
idf.py monitor              # otvori serijski terminal
idf.py fullclean            # potpuno čisti build direktorij i konfiguracije
```
Dokumentacija: ESP-IDF Build System Guide

## 2. Komponente — modularna organizacija i zavisnosti (REQUIRES)
---
Svaka mapa unutar components/ sadrži:

    svoj CMakeLists.txt
    opcionalni Kconfig
    .c i .h datoteke

Primjer components/hello_lib/CMakeLists.txt:
```cmake
idf_component_register(
    SRCS "hello.c"           # izvorne datoteke
    INCLUDE_DIRS "."         # putanja do .h datoteka
)
```
ESP-IDF automatski povezuje sve registrirane komponente prilikom buildanja projekta.

## 3. Kconfig i Menuconfig
---
Kconfig omogućava definiranje konfiguracijskih opcija koje se mogu uključivati ili isključivati u menuconfig.

Primjer components/hello_lib/Kconfig:
```Kconfig
menu "Hello component configuration"

config ENABLE_HELLO
    bool "Enable Hello message"
    default y
    help
        Ako je uključeno, prikazuje se 'Hello from component!'

endmenu
```

Primjena u kodu (hello.c):
```c
#ifdef CONFIG_ENABLE_HELLO
    printf("Hello from component!\n");
#endif
```
U menuconfig se ova opcija pojavljuje kao:
Component config → Hello component → [*] Enable Hello message

## 4. Partition Table
---
Dokumentacija: Partition Tables Guide

Primjer partition_table/partition_table.csv:

# Name,   Type, SubType, Offset,  Size, Flags
nvs,      data, nvs,     0x9000,  0x5000,
phy_init, data, phy,     0xE000,  0x1000,
factory,  app,  factory, 0x10000, 1M,

Opis flash memorije:

    nvs — spremište za postavke
    phy_init — inicijalizacija PHY-a (Wi-Fi)
    factory — glavna aplikacija

## 5. sdkconfig i sdkconfig.defaults
---
    sdkconfig se automatski generira nakon idf.py menuconfig
    sdkconfig.defaults može sadržavati zadane postavke (npr. veličina stacka, UART brzina, enable debug, itd.)

## 6. FreeRTOS i app_main()
---
ESP-IDF automatski pokreće FreeRTOS nakon inicijalizacije.
Funkcija app_main() postaje glavni FreeRTOS task.

Primjer main/main.c:
```c
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
```
## 7. Alati u pozadini
---
CMake - jezik za opisivanje build procesa (kako se program gradi iz .c i .h datoteka)
      - generira build sistem (npr. Makefile, ninja, Visual Studio project, itd.).


| Alat                 | Uloga                               |
| -------------------- | ----------------------------------- |
| CMake                | generira build konfiguraciju        |
| Ninja                | izvršava stvarni build              |
| esptool.py           | flash-a firmware na ESP32           |
| idf.py               | glavni interfejs — poziva sve alate |
| Toolchain (gcc, ar…) | kompajlira i linka koda             |



## 8. Build i konfiguracijske datoteke
---
### .gitignore — preporučeno za Git projekte

Preporučuje se ignorirati privremene i generirane datoteke:
```text
build/
sdkconfig
sdkconfig.old
.vscode/
__pycache__/
```
### Logging — debug poruke

ESP-IDF koristi ugrađeni logging sustav (esp_log.h):

```c
#include "esp_log.h"

static const char *TAG = "MAIN";

void app_main(void) {
    ESP_LOGI(TAG, "Hello ESP-IDF!");
    ESP_LOGW(TAG, "Ovo je upozorenje!");
    ESP_LOGE(TAG, "Ovo je greška!");
}
```
Log razina se može podesiti:
menuconfig → Component config → Log output → Default log verbosity

## Reference
- [Build System Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html)
- [Partition Tables Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html)
