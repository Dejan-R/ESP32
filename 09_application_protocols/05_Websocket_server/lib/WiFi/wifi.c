/*dejan.rakijasic@gmail.com*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include <netdb.h>

static const char *TAG = "WiFi";

/* FreeRTOS event group*/
static EventGroupHandle_t wifi_status_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static esp_netif_t *esp_netif;

void event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case WIFI_EVENT_STA_CONNECTED:
        break;
    case IP_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_status_event_group, WIFI_CONNECTED_BIT);
    break;
    case WIFI_EVENT_STA_DISCONNECTED:
        //ako WiFi nije povezan, povezuje se ponovno
        esp_wifi_connect();
        ESP_LOGI(TAG, "nisam povezan na WiFi, pokušavam se ponovno povezati...");
        xEventGroupSetBits(wifi_status_event_group, WIFI_FAIL_BIT);
        break;
    default:
        break;
    }
}

esp_err_t wifi_STA_povezivanje(char *ssid, char *password)
{   
    
     wifi_status_event_group = xEventGroupCreate();

    //WiFi inicijalizacija
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t konfiguracija = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&konfiguracija));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    esp_netif = esp_netif_create_default_wifi_sta();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    //Za postavljanje statičke IP adrese:
    // ESP_ERROR_CHECK(esp_netif_dhcpc_stop(esp_netif));
    // esp_netif_ip_info_t ip_info;
    // ip_info.ip.addr = ipaddr_addr("192.168.43.150");
    // ip_info.gw.addr = ipaddr_addr("192.168.43.1");
    // ip_info.netmask.addr = ipaddr_addr("255.255.255.0");
    // ESP_ERROR_CHECK(esp_netif_set_ip_info(esp_netif, &ip_info));

    wifi_config_t wifi_STA_podaci = {};
    strlcpy((char *)wifi_STA_podaci.sta.ssid, ssid, sizeof(wifi_STA_podaci.sta.ssid));
    strlcpy((char *)wifi_STA_podaci.sta.password, password, sizeof(wifi_STA_podaci.sta.password));
  
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_STA_podaci));
    ESP_ERROR_CHECK(esp_wifi_start());
    
     //očisti bitove nakon izlaska, ako je bilo koji uvjet zadovoljen
    EventBits_t wifi_status = xEventGroupWaitBits(wifi_status_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
    

    if (wifi_status & WIFI_CONNECTED_BIT)
   {
     return ESP_OK;
   }
   else if(wifi_status & WIFI_FAIL_BIT)
   {
     return ESP_FAIL;
   }
   else{
       return ESP_FAIL;
   }
}