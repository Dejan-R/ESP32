/*
dejan.rakijasic@gmail.com

primjer: SPIFFS primjer 
Primjer pisanja i čitanja:
U mapi projekta treba napraviti mapu 'data'.
Zatim:
1. potrebno je stvoriti particijsku tablicu: 'partitions.csv' (detaljnije na donjoj poveznici)
2. dodati u platformio.ini 'board_build.partitions = partitions.csv'
3. Platformio > esp32dev > Platform > 1. Build Filesystem Image, 2. Upload Filesystem Image
Maknuti komentare u dijelu koda za pisanje da bi se stvorila data.txt i u njoj sadržaj 'SPIFFS primjer.'
NAPOMENA: 
FLASH memorija ima ograničen ciklus brisanja/pisanja!

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
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "esp_spiffs.h"

void app_main(void)
{
    // Konfiguracija SPIFFS
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    // Montiranje SPIFFS
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        printf("Nije moguće montirati SPIFFS. (Error %d)\n", ret);
        return;
    }

/*
    // Otvaranje datoteke za pisanje
    FILE* f = fopen("/spiffs/data.txt", "w");
    if (f == NULL) {
        printf("Nije moguće otvoriti datoteku za pisanje.\n");
        return;
    }

    // Pisanje u datoteku
    fprintf(f, "SPIFFS primjer.");

    // Zatvaranje datoteke
    fclose(f);
  */
    // Otvaranje datoteke za čitanje
    FILE* f = fopen("/spiffs/data.txt", "r");
    if (f == NULL) {
        printf("Nije moguće otvoriti datoteku za čitanje.\n");
        return;
    }

    // Čitanje iz datoteke
    char buffer[50];
    fgets(buffer, sizeof(buffer), f);
    printf("Sadržaj datoteke: %s\n", buffer);

    // Zatvaranje datoteke
    fclose(f);

    // Demontiranje SPIFFS
    esp_vfs_spiffs_unregister(NULL);
}
