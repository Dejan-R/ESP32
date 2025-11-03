ESP-IDF instalacija:

\--> https://dl.espressif.com/dl/esp-idf/

\# ESP-IDF Project Structure — primjer temeljne strukture ESP-IDF projekta:

\- korištenje 'CMake build sustava',

\- rad s 'komponentama',

\- konfiguracija putem 'Kconfig' i 'menuconfig',

\- definiranje 'partition table' i 'sdkconfig' datoteka,

\- te osnovna 'FreeRTOS' strukturu (app\_main).

Struktura projekta

ESP\_IDF\_project\_structure/

│

├── CMakeLists.txt # glavni ulaz u build sustav

├── sdkconfig # generirana konfiguracija iz menuconfig-a

├── sdkconfig.defaults # zadane vrijednosti konfiguracije

├── partition\_table/

│ └── partition\_table.csv # definira raspored memorije (flash particije)

│

├── main/

│ ├── main.c # ulazna točka programa (app\_main)

│ └── CMakeLists.txt # definira "main" komponentu

│

└── components/

└── hello\_lib/

├── hello.c # izvorni kod biblioteke

├── hello.h # zaglavlje biblioteke

├── CMakeLists.txt # registracija komponente u ESP-IDF

└── Kconfig # konfiguracija za menuconfig

1\. Build sustav (CMake, Ninja, idf.py) - https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html

ESP-IDF koristi CMake i Ninja kao svoj build sustav.

Skripta idf.py automatski poziva CMake, generira build direktorij i poziva Ninja da izgradi firmware.

cmake\_minimum\_required(VERSION 3.16) #postavlja minimalnu verziju CMake-a,

project(ESP32\_project\_structure) #definira naziv projekta,

include($ENV{IDF\_PATH}/tools/cmake/project.cmake) #uključuje ESP-IDF build sustav (automatski postavlja alatni lanac, putanje i detektira komponente).

idf.py je glavni alat za rad s ESP-IDF projektom:

najčešće korištene naredbe

idf.py set-target esp32 # postavi ciljanu platformu (esp32...)

idf.py menuconfig # otvori konfiguracijski izbornik (Kconfig)

idf.py build # izgradi projekt

idf.py flash # flash-aj firmware na uređaj

idf.py monitor # otvori serijski terminal

idf.py fullclean # potpuno čisti build direktorij i konfiguracije

2.Komponente -modularna organizacija i zavisnosti (REQUIRES)

Svaka mapa unutar components/ sadrži:

svoj CMakeLists.txt

opcionalni Kconfig

.c i .h datoteke

Primjer: components/hello\_lib/CMakeLists.txt

idf\_component\_register(

SRCS "hello.c" # izvorne datoteke

INCLUDE\_DIRS "." # putanja do .h datoteka

)

registrira hello\_lib kao komponentu.

ESP-IDF automatski povezuje sve registrirane komponente prilikom buildanja projekta.

3\. Kconfig i Menuconfig

Kconfig omogućava definiranje konfiguracijskih opcija koje se mogu uključivati ili isključivati u menuconfig.

components/hello\_lib/Kconfig:

menu "Hello component configuration"

config ENABLE\_HELLO

bool "Enable Hello message"

default y

help

Ako je uključeno, prikazuje se 'Hello from component!'

endmenu

U kodu (hello.c):

#ifdef CONFIG\_ENABLE\_HELLO

printf("Hello from component!\\n");

#endif

U menuconfig se ova opcija pojavljuje kao:

Component config → Hello component → \[\*\] Enable Hello message

4\. Partition Table - https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html

partition\_table/partition\_table.csv:

\# Name, Type, SubType, Offset, Size, Flags

nvs, data, nvs, 0x9000, 0x5000,

phy\_init, data, phy, 0xE000, 0x1000,

factory, app, factory, 0x10000, 1M,

Definira raspored flash memorije:

nvs — spremište za postavke

phy\_init — inicijalizacija PHY-a (Wi-Fi)

factory — glavna aplikacija

5\. sdkconfig i sdkconfig.defaults

sdkconfig se automatski generira nakon idf.py menuconfig.

sdkconfig.defaults može sadržavati zadane postavke (npr. veličina stacka, UART brzina, enable debug, itd.)

6\. FreeRTOS i app\_main()

ESP-IDF automatski pokreće FreeRTOS nakon inicijalizacije.

Funkcija app\_main() postaje glavni FreeRTOS task.

main/main.c:

#include "freertos/FreeRTOS.h"

#include "freertos/task.h"

#include "hello.h"

void app\_main(void) {

hello\_message();

while (1) {

vTaskDelay(pdMS\_TO\_TICKS(1000));

}

}

7\. Alati u pozadini

CMake - nije jezik programiranja, nego jezik za opisivanje build procesa (kako se program gradi iz .c i .h datoteka)

\- generira build sistem (npr. Makefile, ninja, Visual Studio project, itd.).

AlatUloga

CMakegenerira build konfiguraciju

Ninjaizvršava stvarni build

esptool.pyflasha firmware na ESP32

idf.pyglavni interfejs — poziva sve alate

Toolchain (gcc, ar, etc.)compile i link koda

8\. Dodatne preporuke i razvojne prakse

.gitignore — preporučeno za Git projekte

Preporučuje se ignorirati privremene i generirane datoteke:

\# Build i konfiguracijske datoteke

build/

sdkconfig

sdkconfig.old

.vscode/

\_\_pycache\_\_/

Logging — debug poruke - ESP-IDF koristi ugrađeni logging sustav (esp\_log.h):

#include "esp\_log.h"

static const char \*TAG = "MAIN";

void app\_main(void) {

ESP\_LOGI(TAG, "Hello ESP-IDF!");

ESP\_LOGW(TAG, "Ovo je upozorenje!");

ESP\_LOGE(TAG, "Ovo je greška!");

}

Log razina se može podesiti:

menuconfig → Component config → Log output → Default log verbosity.
