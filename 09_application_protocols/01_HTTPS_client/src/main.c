
/*dejan.rakijasic@gmail.com


Primjer: HTTPS client

HTTP i RESTful API-ji su standard za komunikaciju, iako nije ponekad idealan izbor za IoT aplikacije koje periodički prenose malu količinu podataka.

Više o HTTP-u: https://www.w3schools.com/tags/ref_httpmethods.asp
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_http_client.html#https-request

ESP-TLS: (HTTP se obično koristi preko TLS-a koji osigurava sigurnu komunikaciju)
ESP-TLS pruža pojednostavljeno API (defaultno Mbed TLS ili wolfSSL library) sučelje za pristup najčešće korištenim TLS funkcijama:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_tls.html

ESP x509 Certificate Bundle API pruža jednostavan način uključivanja paketa prilagođenih x509 korijenskih certifikata za provjeru TLS poslužitelja. 

Platformio - Embedding Binary Data:
https://docs.platformio.org/en/latest/platforms/espressif32.html#embedding-binary-data

Više detalja: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_crt_bundle.html
*/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "WiFi.h"
#include "esp_http_client.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h"

static const char *TAG = "HTTPS CLIENT:";

esp_err_t http_podaci(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
    
        ESP_LOGI(TAG, "Length=%d", evt->data_len);
        printf("%.*s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        break;
    }
    return ESP_OK;
}

void dohvati_podatke(){
        esp_http_client_config_t esp_http_client_config = {
        .url = "https://fc743729-f964-4c97-b168-7348070148c9.mock.pstmn.io/MOCK_server",
        .event_handler = http_podaci,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .crt_bundle_attach = esp_crt_bundle_attach, 
        };

   esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
   esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
   ESP_LOGI(TAG, "HTTP GET status = %d, content_length = %lld",
           esp_http_client_get_status_code(client),
           esp_http_client_get_content_length(client));
}
    else
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
 esp_http_client_cleanup(client);
}


void app_main() {
        ESP_ERROR_CHECK(nvs_flash_init());
 esp_err_t wifi_rezultat = wifi_STA_povezivanje("mreza", "12345678");
if (wifi_rezultat == ESP_OK)
{
    printf("Uređaj je povezan na WiFi!\n");
}
else
{
    printf("Povezivanje na WiFi nije uspjelo!\n");
}
    dohvati_podatke();
}