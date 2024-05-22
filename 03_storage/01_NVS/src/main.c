/*
dejan.rakijasic@gmail.com

primjer: NVS
NVS koristi "Key-value pair" (par ključ-vrijednost) oblik podataka u kojem svaki podatak (vrijednost) u skupu podataka ima jedinstveni identifikator (ključ). 
Ključ je naziv pod kojim se podatak identificira, dok je vrijednost sama informacija koja se pohranjuje.

ESP32 osim RAM (Volatile), ima i ugrađenu (SPI) FLASH (Non Volatile) memoriju, a može se i dodatno proširiti EEPROM-om, te SD i eMMC karticama.
FLASH - Non Volatile Storage (NVS) - memorija 4/8/16MB
non-OTA configuration: 
 -NVS library - pohranjuju se parametri korisničke aplikacije 
 -PHY_init - upravljački program za Bluetooth i Wi-Fi (implementacija fizičkog mrežnog sloja)
 -factory - bootloader
OTA (Over-The-Air Upadate) configuration: NVS library, PHY_init, factory + dodatne 3 particije od kojih se jedna 'OTAdata' koristi za koristi za pohranu OTA aplkacije, a druge dvije za OTA ažuriranja

DATOTEČNI SUSTAV:
    -NVS (Non Volatile Storage) - služi za pohranu postavki, konfiguracija (npr. WiFi password & username) i drugih informacija koje trebaju biti trajno zapisane između ponovnih pokretanja uređaja
    -SPIFFS (SPI Flash File System) - datotečni sustav male do srednje veličine - pohrana konfiguracijskih datoteka, slika, web stranica
    -FAT (File Allocation Table) - datotečni sustav kompatibilan s drugim uređajima i sustavima (npr. SD i eMMC kartice) - prikladan za za rad s većim datotekama (audio i video zapisi, slike) i izmjenjivim medijima
    -VFS (Virtual file system) - apstraktni datototečni sustav, kada su na ESP32 iz određenih razloga potrebni i SPIFFS i FAT datotečni sustavi

Platfomio partition tables:
https://docs.platformio.org/en/latest/platforms/espressif32.html#partition-tables

detaljnije:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/index.html

*/

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"

void app_main() {
    // Inicijalizacija NVS sustava
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // Ako nema dovoljno slobodnih stranica ili je pronađena nova verzija, obriši NVS i pokušaj ponovno
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Pisanje podataka u NVS
    nvs_handle_t my_handle;
    ret = nvs_open("pohrana", NVS_READWRITE, &my_handle);
    if (ret != ESP_OK) {
       printf("Greška (%s) prilikom otvaranja NVS handle-a!\n", esp_err_to_name(ret));
    } else {
        // Postavljanje vrijednosti u NVS
        int32_t vrijednost = 42;
        ret = nvs_set_i32(my_handle, "kljuc", vrijednost);
        printf("Postavljanje 'kljuc' na %ld\n", vrijednost);

        // Spremanje promjena
        ret = nvs_commit(my_handle);

        // Zatvaranje NVS handle-a
        nvs_close(my_handle);
    }

    // Čitanje podataka iz NVS-a
    int32_t pohranjena_vrijednost = 0;  // Vrijednost koja će se pročitati
    ret = nvs_open("pohrana", NVS_READWRITE, &my_handle);
    if (ret == ESP_OK) {
        // Čitanje vrijednosti iz NVS-a
        ret = nvs_get_i32(my_handle, "kljuc", &pohranjena_vrijednost);
        switch (ret) {
            case ESP_OK:
                printf("Čitanje 'kljuc' iz NVS-a: %ld\n", pohranjena_vrijednost);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                  printf("Vrijednost još nije inicijalizirana!\n");
                break;
            default:
                printf("Greška (%s) prilikom čitanja!\n", esp_err_to_name(ret));
        }

        // Zatvaranje NVS handle-a
        nvs_close(my_handle);
    } else {
       printf("Greška (%s) prilikom otvaranja NVS handle-a!\n", esp_err_to_name(ret));
    }
}