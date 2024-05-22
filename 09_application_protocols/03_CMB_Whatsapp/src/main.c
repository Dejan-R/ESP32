/* dejan.rakijasic@gmail.com

Primjer: HTTP CLIENT primjer - Whatsapp callmebot
koristi se QUEUE - task 'mjerenje_parametara' šalje pomoću Queue-a podatke o temperaturi tasku 'slanje_poruke'

Potrebno je dobiti apikey od bot-a prije korištenja API-ja:
1. dodati broj +34644718199 u kontakte
2. poslati poruku "I allow callmebot to send me messages" na gornji broj (koristeći WhatsApp)
3. stići će poruka od bot-a "API Activated for your phone number. Your APIKEY is ******"

*/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_http_client.h"
#include "WiFi.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h"

QueueHandle_t parametri_queue;

// Postavite svoj broj telefona i API ključ
char api_key[] = "*******";
char whatsapp_broj[] = "+385*********";

static const char *TAG = "HTTP_CLIENT";

static void mjerenje_parametara(void *pvParameters)
{
    int temperatura = 23; 

    while (1)
    {
        // Slanje temperature u Queue
        xQueueSend(parametri_queue, &temperatura, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

static void slanje_poruke(void *pvParameters)
{
     int temperatura;
      while (1)
    {
         // Čekanje na podatke o temperaturi u Queue-u
        if (xQueueReceive(parametri_queue, &temperatura, portMAX_DELAY))
        {

    char temperatura_char[50]; 
    // Pretvorba int u char array
   snprintf(temperatura_char, sizeof(temperatura_char), "Temperatura:+%d+C", temperatura);
            // Formatiranje URL-a pomoću snprintf
            char url[512];  //duljina URL niza
    snprintf(url, sizeof(url), "https://api.callmebot.com/whatsapp.php?phone=%s&text=%s&apikey=%s", whatsapp_broj, temperatura_char, api_key);
    esp_http_client_config_t esp_http_client_config = {
        .url = url,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .crt_bundle_attach = esp_crt_bundle_attach, 
    };

     esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
   esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
   ESP_LOGI(TAG, "Status = %d, content_length = %lld",
           esp_http_client_get_status_code(client),
           esp_http_client_get_content_length(client));
}
    esp_http_client_cleanup(client);
}
  vTaskDelay(pdMS_TO_TICKS(10000));
}
  
}

void app_main()
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
      // Inicijalizacija Queue-a
        parametri_queue = xQueueCreate(1, sizeof(int));
        // Kreiranje zadataka za mjerenje parametara i slanje poruke
        xTaskCreate(&mjerenje_parametara, "mjerenje parametara", 4096, NULL, 5, NULL);
        xTaskCreate(&slanje_poruke, "slanje poruke", 4096, NULL, 5, NULL);

}
