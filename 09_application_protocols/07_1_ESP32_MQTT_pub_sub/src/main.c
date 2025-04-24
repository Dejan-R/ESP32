

/*dejan.rakijasic@gmail.com

Primjer: MQTT - pub_sub_example

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
#include "lwip/err.h"
#include "lwip/sys.h"
#include "WiFi.h"
#include "mqtt_client.h"
#include "driver/gpio.h"
#define SUB_TOPIC "sub_topic"
#define PUB_TOPIC "pub_topic"

#define GPIO_OUTPUT_PIN 2

static bool MQTT_connect_status = false;
static esp_mqtt_client_handle_t client;
static QueueHandle_t mqtt_queue;

static void event_handler_mqtt(void *event_handler_arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI("MQTT", "MQTT_EVENT_CONNECTED");
        MQTT_connect_status=true;
        esp_mqtt_client_subscribe(client, SUB_TOPIC, 1);
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

        char message[100];
        snprintf(message, sizeof(message), "%.*s", event->data_len, event->data);
        xQueueSend(mqtt_queue, &message, portMAX_DELAY);

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

static void obrada_mqtt_poruka(void *param)
{
    char poruka[100];
    while (1)
    {
        if (xQueueReceive(mqtt_queue, &poruka, portMAX_DELAY)) {
            ESP_LOGI("MQTT", "Primljena poruka: %s", poruka);
            if (strcmp(poruka, "ON") == 0) {
                gpio_set_level(GPIO_OUTPUT_PIN, 1);
            } else if (strcmp(poruka, "OFF") == 0) {
                gpio_set_level(GPIO_OUTPUT_PIN, 0);
            }
        }
    }
}


int mqtt_slanje_podataka(const char *topic, const char *payload)
{
    return esp_mqtt_client_publish(client, topic, payload, strlen(payload), 1, 0);
}

static void mjerenje_parametara(void *param)
{
   uint8_t temp = 23;
   char buffer[50];
    while (true)
    {
        sprintf(buffer, "Temperatura sa ESP32  %d", temp);
        if(MQTT_connect_status){
        mqtt_slanje_podataka(PUB_TOPIC, buffer);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main() {

mqtt_queue = xQueueCreate(10, sizeof(char[100]));
esp_rom_gpio_pad_select_gpio(GPIO_OUTPUT_PIN);
gpio_set_direction(GPIO_OUTPUT_PIN, GPIO_MODE_OUTPUT);
gpio_set_level(GPIO_OUTPUT_PIN, 0);

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
      .broker.address.uri = "mqtt://192.168.170.187",
      .broker.address.port = 1883,
      .credentials.username = "ssdj",
      .credentials.authentication.password = "komarac",
      .credentials.client_id = "ESP32"}; //ime klijenta (ESP32)

    client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, event_handler_mqtt, NULL);
    esp_mqtt_client_start(client);
  
    xTaskCreate(obrada_mqtt_poruka, "mqtt_gpio", 3 * configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    xTaskCreate(mjerenje_parametara, "temperatura", 3 * configMINIMAL_STACK_SIZE, NULL, 5, NULL);
     
}