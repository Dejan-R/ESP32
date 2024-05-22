/*dejan.rakijasic@gmail.com

Primjer: OTA - (Over The Air Updates) - preuzimanje firmware-a sa Google diska

- ovaj primjer će preuzeti firmware.bin sa Google diska samo ako je taj primjer viša verzija (u suprotnom nema nadogradnje)
- primjer na Google driveu je verzija 1.0.2, te dodaje novu funkcionalnost - LED toggle (GPIO 19) - 500ms

OTA omogućuje update firmware-a na temelju primljenih podataka dok je normalni firmware pokrenut (putem WiFi-a ili Bluetooth-a)
OTA zahtijeva konfiguriranje particijskih tablica uređaja s najmanje dvije particije utora za OTA aplikacije i OTA particiju podataka: ota_0 i ota_1

PARTICIJSKA TABLICA:
1. potrebno je stvoriti particijsku tablicu: 'partitions_ota.csv'
2. dodati u platformio.ini 'board_build.partitions = partitions_two_ota.csv'
3. u 'sdkconfig.esp32dev' :

                            #
                            # Partition Table
                            #
                            # CONFIG_PARTITION_TABLE_SINGLE_APP is not set
                            CONFIG_PARTITION_TABLE_TWO_OTA=y
                            # CONFIG_PARTITION_TABLE_CUSTOM is not set
                            CONFIG_PARTITION_TABLE_CUSTOM_FILENAME="partitions.csv"
                            CONFIG_PARTITION_TABLE_FILENAME="partitions_two_ota.csv"
                            CONFIG_PARTITION_TABLE_OFFSET=0x8000
                            CONFIG_PARTITION_TABLE_MD5=y
                            # end of Partition Table

GOOGLE DRIVE: 
1. na Google drive uploadati firmware.bin koji ESP32 treba preuzeti:

            .pio/build/esp32dev/
                    |-- ...
                    |-- firmware.bin

2. link preurediti tako da vodi izravno do firmware.bin:

- https://drive.google.com/uc?export=download&id='ID datoteke'


FIRMWARE verzija: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/misc_system_api.html
 - firmware verzija pohranjena je u strukturi esp_app_desc_t i nalazi se u DROM sektoru.
 - za ručno postavljanje verzije firmwarea (projekta) --> u  CMakeLists.txt postaviti 'set(PROJECT_VER "1.0.0")'
 - Ako je postavljen CONFIG_APP_PROJECT_VER_FROM_CONFIG = 1, učitat će se verzija CONFIG_APP_PROJECT_VER:
    --> platfromio.ini:
                            build_flags =
                            -DCONFIG_APP_PROJECT_VER_FROM_CONFIG=1
                            -DCONFIG_APP_PROJECT_VER=1.0.0
- u suprotnom će verzija biti dohvaćena iz version.txt (ili korištenjem git describe)
- ako ni jedno od ovoga nije dostupno, verzija će se postaviti na 1.

Za više infromacija:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp_https_ota.html
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h"
#include "esp_https_ota.h"
#include "string.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "wifi.h" 

#define TAG "OTA"
esp_https_ota_handle_t https_ota_handle;


esp_err_t do_firmware_upgrade() {
    // Konfiguracija HTTP klijenta
    esp_http_client_config_t config = {
        .url = "https://drive.google.com/uc?export=download&id=1f5Q9h8KZZGrtL0AAnluxDUQCXPXEddMj",
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    // Konfiguracija HTTPS OTA
    esp_https_ota_config_t ota_config = {
        .http_config = &config,
      
    };

    esp_err_t ret = esp_https_ota_begin(&ota_config, &https_ota_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    // Preuzeta verzija firmware-a
    esp_app_desc_t new_app_info;
    ret = esp_https_ota_get_img_desc(https_ota_handle, &new_app_info);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get image descriptor");
        return ret;
    }

    ESP_LOGI(TAG, "target version: '%s'", new_app_info.version);

    const esp_partition_t *current_partition = esp_ota_get_running_partition();
    esp_app_desc_t current_app_info;
    if (esp_ota_get_partition_description(current_partition, &current_app_info) == ESP_OK) {
        ESP_LOGI(TAG, "Running firmware version: %s", current_app_info.version);
    }

    // Usporedi verzije i izvrši OTA samo ako je nova verzija veća
    if (memcmp(new_app_info.version, current_app_info.version, sizeof(new_app_info.version)) > 0) {
        ESP_LOGI(TAG, "New firmware version available. Performing OTA update");


  // Čeka dok se OTA ažuriranje ne dovrši
        while (esp_https_ota_perform(https_ota_handle) == ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }

        ret = esp_https_ota_finish(https_ota_handle);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "OTA update successful. Rebooting...");
            esp_restart();  // Ponovno pokreni uređaj nakon uspješnog OTA ažuriranja
        } else {
            ESP_LOGE(TAG, "OTA update failed");
        }

    } else {
        ESP_LOGI(TAG, "No new firmware version available. Skipping OTA update");
    }

    return ret;
}


void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init()); 
    esp_err_t wifi_result = wifi_STA_povezivanje("mreza", "12345678");
    
    if (wifi_result == ESP_OK) {
        printf("Uređaj je povezan na WiFi!\n");
        vTaskDelay(5000/portTICK_PERIOD_MS);
    
    } else {
        printf("Povezivanje na WiFi nije uspjelo!\n");
    }

       while (1) {
            vTaskDelay(10000/portTICK_PERIOD_MS);
            do_firmware_upgrade();
        
        }
}
