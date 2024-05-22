

/*dejan.rakijasic@gmail.com

Primjer: MQTT - SSL(HiveMQ) - JSON - primjer

MQTT (Message Queueuing Telemetry Transport) - robustan protokol za laganu i pouzdanu razmjenu podataka u IoT ekosustavima.
 - PUBLISH-SUBSRCIBE model
 - BROKER - posrednik između uređaja koji se povezuju radi objavljivanja tema ili pretplate na njih.
 - TOPIC - niz za kategorizaciju poruka (uređaji mogu objavljivati poruke na temu ili se pretplatiti na primanje poruka iz određene teme)
    - struktura stabla pomoću '/', zamjenski nizovi koriste '#'
 - QoS (Quality of Service) - kvaliteta usluge
    QOS 0  -  Almost Once (Fire and forget) - broker dostavlja poruku pretplatnicima bez potvrde
    QOS 1 - At least Once (Acknowledged Delivery) - osigurava da se poruka isporuči pretplatnicima barem jednom
    QOS 2 -  Exactly Once (Assured Delivery) - osigurava da se poruka isporuči pretplatnicima 'točno' jednom
 - RETAIN - broker može zadržati poruku i pričekati da se uređaj poveže, a zatim odmah poslati obavijest
 - LAST WILL poruka - ako se veza prekine bez odgovarajućeg razloga (uređaj ostaje bez struje i sl.) broker može objaviti 'Last will' poruku.


SSL Secure Sockets Layer - siguran protokol koji omogućuje sigurno slanje informacija putem interneta.
- moramo imati SSL certifikat - digitalna datoteka koja potvrđuje vlasništvo ili autentičnost web stranice.
- za HiveMQ možemo preuzeti ovdje: https://letsencrypt.org/certs/isrgrootx1.pem

Uključiveanje certifikata u projekt:
Platformio - Embedding Binary Data: https://docs.platformio.org/en/latest/platforms/espressif32.html
1. dodati u platformio.ini --> board_build.embed_txtfiles = src/certifikat.pem
2. dodati u src/CMakeList.txt target_add_binary_data(${COMPONENT_TARGET} "certifikat.pem" TEXT)
3. NE uključiti strukturu mape u naziv simbola (extern const uint8_t certifikat_pem_start[] asm("_binary_certifikat_pem_start");)
4. ako je projekt već build-an prije izmjene src/CMakeList.txt, izbrisati .pio/build!

JSON (JavaScript Object Notation) - je tekstualni format za pohranu i prijenos podataka.

https://docs.platformio.org/en/latest/platforms/espressif32.html#embedding-binary-data

Za više infromacija:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/mqtt.html
*/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "nvs.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "WiFi.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include "cJSON.h"  

#define SUB_TOPIC "sub_topic"
#define PUB_TOPIC "pub_topic"
bool MQTT_connect_status=false;

extern const uint8_t certifikat_pem_start[] asm("_binary_certifikat_pem_start");
extern const uint8_t certifikat_pem_end[] asm("_binary_certifikat_pem_end");

static esp_mqtt_client_handle_t client;

static void event_handler_mqtt(void *event_handler_arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI("MQTT", "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(client, SUB_TOPIC, 1);
        MQTT_connect_status=true;
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI("MQTT", "MQTT_EVENT_DISCONNECTED");
         MQTT_connect_status=false;
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI("MQTT", "MQTT_EVENT_SUBSCRIBED");
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI("MQTT", "MQTT_EVENT_UNSUBSCRIBED");
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI("MQTT", "MQTT_EVENT_PUBLISHED");
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI("MQTT", "MQTT_EVENT_DATA");
        printf("topic: %.*s\n", event->topic_len, event->topic);
        printf("message: %.*s\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
           if (event->error_handle->esp_transport_sock_errno != 0)
        {
            ESP_LOGE("MQTT", "ERROR %s", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        break;
    }
}



int mqtt_slanje_podataka(const char *topic, const char *payload)
{
    return esp_mqtt_client_publish(client, topic, payload, strlen(payload), 1, 0);
}

static void mjerenje_parametara(void *param)
{
    while (true)
    {
        cJSON *json_root = cJSON_CreateObject();
        cJSON *json_data = cJSON_CreateObject();
        cJSON *json_temperature_array = cJSON_CreateArray();

        uint8_t temp = 23;
        cJSON_AddItemToArray(json_temperature_array, cJSON_CreateNumber(temp));
        cJSON_AddItemToObject(json_data, "temperatura", json_temperature_array);
        cJSON_AddItemToObject(json_root, "d", json_data);

        char *json_str = cJSON_Print(json_root);
 if(MQTT_connect_status){
        mqtt_slanje_podataka(PUB_TOPIC, json_str);
 }
        free(json_str);
        cJSON_Delete(json_root);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
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
 
       esp_mqtt_client_config_t mqtt_config = {
      .broker.address.uri = "***********************.s2.eu.hivemq.cloud",
      .broker.address.port = 8883,
      .credentials.username = "**********",
      .credentials.authentication.password = "**********",
      .broker.verification.certificate = (const char *)certifikat_pem_start,
      .credentials.client_id = "ESP32"}; //ime klijenta (ESP32)

    client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, event_handler_mqtt, NULL);
    esp_mqtt_client_start(client);

    xTaskCreate(mjerenje_parametara, "temperatura", 3 * configMINIMAL_STACK_SIZE, NULL, 5, NULL);

}
