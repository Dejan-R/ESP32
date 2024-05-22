/*dejan.rakijasic@gmail.com

Primjer: Websocket server 

Ovaj primjer je implementacija ESP-IDF primjera za websocket:
https://github.com/espressif/esp-idf/blob/5f4249357372f209fdd57288265741aaba21a2b1/examples/protocols/http_server/ws_echo_server/main/ws_echo_server.c

WebSocket omogućuje dvosmjernu komunikaciju između klijenta i servera --> i klijent i server mogu slati podatke jedan drugome neovisno, bez potrebe za inicijacijom od strane klijenta.
Primjena Websocketa je korisna kada je potrebna brzna dvosmjerna komunikacija, kao i upravljanje u realnom vremenu.

POTREBNO OMOGUĆITI:
sdkconfig.esp32dev --> CONFIG_HTTPD_WS_SUPPORT=y

TESTIRANJE: (instalacija i pokretanje Websocket klijenta i komunikacija sa ESP32 kao websocket serverom)
1. preduvjet --> instaliran Node.js i NPM 
2. instalacija (websocket client) wscat --> npm install -g wscat
3. pokretanje (websocket client) wscat --> wscat -c ws://ipadress/ws
4. testiranje komunikacija --> {"type": "text", "payload": "Test message"}
5. asinkroni odgovor --> {"type": "text", "payload": "Trigger async"} 

Za više infromacija:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_http_server.html#websocket-server
*/

#include <stdio.h>
#include <string.h>

#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_eth.h"
#include "wifi.h" 


#include <esp_http_server.h>

static const char *TAG = "Websocket server";

//Struktura za argumente asinkrone funkcije slanja
struct async_resp_arg {
    httpd_handle_t hd;  // handle HTTP servera
    int fd;             // file descriptor socket-a
};

//Asinkrona funkcija slanja koju stavljamo u 'work queue' HTTP servera
static void ws_async_send(void *arg)
{
    // Statični podaci koje ćemo poslati
    static const char * data = "Async data";

    struct async_resp_arg *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;  // handle HTTP servera
    int fd = resp_arg->fd; //  file descriptor socket-a
    
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t*)data;
    ws_pkt.len = strlen(data);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
   // Slanje WebSocket okvira asinkrono
    httpd_ws_send_frame_async(hd, fd, &ws_pkt);
    free(resp_arg);
}

//Funkcija za pokretanje asinkronog slanja
static esp_err_t trigger_async_send(httpd_handle_t handle, httpd_req_t *req)
{
    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
    if (resp_arg == NULL) {
        return ESP_ERR_NO_MEM;
    }
    resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    esp_err_t ret = httpd_queue_work(handle, ws_async_send, resp_arg);
    if (ret != ESP_OK) {
        free(resp_arg);
    }
    return ret;
}

//Ovaj handler vraća primljene WebSocket podatke i pokreće asinkrono slanje ako se primi određena poruka
static esp_err_t echo_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
     // Potvrda da je rukovanje (handshake) gotovo i nova konekcija je otvorena
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }
    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
 /* Postavljamo max_len = 0 kako bismo dobili duljinu okvira */
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }
    ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);
    if (ws_pkt.len) {
        /* ws_pkt.len + 1 je za NULL terminaciju jer očekujemo string */
        buf = calloc(1, ws_pkt.len + 1);
        if (buf == NULL) {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        /* Postavljamo max_len = ws_pkt.len kako bismo dobili payload okvira */
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
        ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);
    }
    ESP_LOGI(TAG, "Packet type: %d", ws_pkt.type);
    if (ws_pkt.type == HTTPD_WS_TYPE_TEXT &&
        strcmp((char*)ws_pkt.payload,"Trigger async") == 0) {
        free(buf);
        return trigger_async_send(req->handle, req);
    }

    ret = httpd_ws_send_frame(req, &ws_pkt);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
    }
    free(buf);
    return ret;
}

//Definicija URI-a za WebSocket
static const httpd_uri_t ws = {
        .uri        = "/ws",
        .method     = HTTP_GET,
        .handler    = echo_handler,
        .user_ctx   = NULL,
        .is_websocket = true
};

//Funkcija za pokretanje HTTP servera
static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Registering the ws handler
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &ws);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

//Funkcija za zaustavljanje HTTP servera
static esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    return httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        if (stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop http server");
        }
    }
}

//Handler za događaj uspostavljanja konekcije
static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}


void app_main(void)
{
  

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
   start_webserver(); //pokretanje HTTP servera
}
