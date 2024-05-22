
/*dejan.rakijasic@gmail.com

Primjer: HTTPS server
HTTP i RESTful API-ji su standard za komunikaciju, iako nije ponekad idealan izbor za IoT aplikacije koje periodički prenose malu količinu podataka.

- omogućavanje HTTPS library-a (#include "esp_https_server.h")
sdkconfig.esp32dev --> CONFIG_ESP_HTTPS_SERVER_ENABLE=y

-kreiranje certifikata: 
NAPOMENA: samopotpisani certifikat (koristi se za razvoj i testiranje) uzrokovat će sigurnosna upozorenja u preglednicima, jer nije izdan od strane pouzdane CA (Certification Authority)

openSSL: https://slproweb.com/products/Win32OpenSSL.html  (openssl (all commands))
openssl req -newkey rsa:2048 -nodes -keyout prvtkey.pem -x509 -days 3650 -out servercert.pem -subj "/CN=ESP32 HTTPS server example"

Uključivanje certifikata u projekt:
Platformio - Embedding Binary Data: https://docs.platformio.org/en/latest/platforms/espressif32.html
1. dodati u platformio.ini -->  board_build.embed_txtfiles = src/cacert.pem
                                src/prvtkey.pem
2. dodati u src/CMakeList.txt -->   target_add_binary_data(${COMPONENT_TARGET} "cacert.pem" TEXT)
                                    target_add_binary_data(${COMPONENT_TARGET} "prvtkey.pem" TEXT)
3. NE uključiti strukturu mape u naziv simbola (extern const uint8_t certifikat_pem_start[] asm("_binary_certifikat_pem_start");)
4. ako je projekt već build-an prije izmjene src/CMakeList.txt, izbrisati .pio/build!

Platformio - Embedding Binary Data:
https://docs.platformio.org/en/latest/platforms/espressif32.html#embedding-binary-data

za više detelja:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_https_server.html
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

#include "esp_https_server.h"
#include "esp_tls.h"

static const char *TAG = "HTTPS SERVER";

static esp_err_t get_metoda(httpd_req_t *req)
{
  ESP_LOGI(TAG, "URL: %s", req->uri);
 	 httpd_resp_sendstr(req, "Primjer GET metode - GET odgovor.");
    return ESP_OK;
}

esp_err_t post_metoda(httpd_req_t *req)
{
    char buffer[100];
    memset(buffer, 0, sizeof(buffer));
    httpd_req_recv(req, buffer, req->content_len);
    // Postavljanje HTTP odgovora
    httpd_resp_set_status(req, "200 OK");
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    // Slanje odgovora
    const char resp[] = "POST odgovor.";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    // Ispis primljenih podataka
    printf("\nPOST sadržaj: %s\n", buffer);
    return ESP_OK;
}

static const httpd_uri_t url_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_metoda
};

static const httpd_uri_t url_post = {
    .uri = "/",
    .method = HTTP_POST,
    .handler = post_metoda
};

static httpd_handle_t pokretanje_servera(void)
{
    //Pokretanje servera
    ESP_LOGI(TAG, "Starting server");
    
    httpd_ssl_config_t config = HTTPD_SSL_CONFIG_DEFAULT();
    httpd_handle_t server = NULL;

    extern const uint8_t servercert_pem_start[] asm("_binary_servercert_pem_start");
    extern const uint8_t servercert_pem_end[] asm("_binary_servercert_pem_end");
    config.servercert = servercert_pem_start;
    config.servercert_len = servercert_pem_end - servercert_pem_start;


    extern const uint8_t prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
    extern const uint8_t prvtkey_pem_end[] asm("_binary_prvtkey_pem_end");
    config.prvtkey_pem = prvtkey_pem_start;
    config.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;

    esp_err_t ret = httpd_ssl_start(&server, &config);
    if (ESP_OK != ret)
    {
        ESP_LOGI(TAG, "Error starting server!");
        return NULL;
    }
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &url_get);
    httpd_register_uri_handler(server, &url_post);
    return server;
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
    pokretanje_servera();
}