/*
dejan.rakijasic@gmail.com

primjer: FAT primjer 
Primjer pisanja i čitanja:
U mapi projekta treba napraviti mapu 'data'.
Zatim:
1. potrebno je stvoriti particijsku tablicu: 'partitions.csv' (detaljnije na donjoj poveznici)
2. dodati u platformio.ini 'board_build.partitions = partitions.csv'
3. Platformio > esp32dev > Platform > 1. Build Filesystem Image, 2. Upload Filesystem Image
Maknuti komentare u dijelu koda za pisanje da bi se stvorila data.txt i u njoj sadržaj 'FAT primjer.'
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_system.h"

void app_main(void)
{
  wl_handle_t wl_handle;

// Konfiguracija FATFS
esp_vfs_fat_mount_config_t esp_vfs_fat_mount_config = {
    .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
    .max_files = 4,
    .format_if_mount_failed = true,
};
esp_err_t ret = esp_vfs_fat_spiflash_mount_rw_wl("/spiflash", "storage", &esp_vfs_fat_mount_config, &wl_handle);

if (ret != ESP_OK) {
    printf("Nije moguće montirati FATFS. (Error %d)\n", ret);
    return;
}
/*
    // Otvaranje datoteke za pisanje
    FILE* f = fopen("/spiflash/data.txt", "w");
    if (f == NULL) {
        printf("Nije moguće otvoriti datoteku za pisanje.\n");
        esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash", wl_handle);
        return;
    }

    // Pisanje u datoteku
    fprintf(f, "FAT primjer.");

    // Zatvaranje datoteke
    fclose(f);
    */

    // Otvaranje datoteke za čitanje
      FILE* f = fopen("/spiflash/data.txt", "r");
    if (f == NULL) {
        printf("Nije moguće otvoriti datoteku za čitanje.\n");
        esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash", wl_handle);
        return;
    }

    // Čitanje iz datoteke
    char buffer[100];
    fgets(buffer, sizeof(buffer), f);
    printf("Sadržaj datoteke: %s\n", buffer);

    // Zatvaranje datoteke
    fclose(f);

    // Demontiranje FATFS
    esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash", wl_handle);
}