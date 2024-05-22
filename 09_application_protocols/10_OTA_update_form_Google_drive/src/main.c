/*dejan.rakijasic@gmail.com


Primjer: OTA - (Over The Air Updates) - preuzimanje firmware-a sa Google diska
- firmware.bin koji se preuzme sa Google diska ima novu funkcionalnost - LED toggle (GPIO 19) - 500ms

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

esp_err_t do_firmware_upgrade()
{
    esp_http_client_config_t config = {
             .url = "https://drive.google.com/uc?export=download&id=1f5Q9h8KZZGrtL0AAnluxDUQCXPXEddMj",
    
            .crt_bundle_attach = esp_crt_bundle_attach
    };
    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK) {
        esp_restart();
    } else {
        return ESP_FAIL;
    }
    return ESP_OK;
}


void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init()); 
    esp_err_t wifi_rezultat = wifi_STA_povezivanje("mreza", "12345678");
    
    if (wifi_rezultat == ESP_OK) {
        printf("Uređaj je povezan na WiFi!\n");
        vTaskDelay(5000/portTICK_PERIOD_MS);
    } else {
        printf("Povezivanje na WiFi nije uspjelo!\n");
    }
    while(1) {
    vTaskDelay(10000/portTICK_PERIOD_MS);
      do_firmware_upgrade();
    }
}

