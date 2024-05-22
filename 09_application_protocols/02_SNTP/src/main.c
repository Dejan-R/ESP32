
/*dejan.rakijasic@gmail.com

Primjer: SNTP-a (Simple Network Time Protocol)

Za više infromacija:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system_time.html#sntp-time-synchronization
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
#include "esp_sntp.h"
#include <time.h>

#include "WiFi.h"
#define NTP_SERVER "1.hr.pool.ntp.org"
static const char *TAG = "SNTP TIME";


SemaphoreHandle_t got_time_semaphore;

void print_time(){
    /*  time_t - tip podatka koji se koristi za reprezentaciju vremenskog trenutka (sekunde od .1.1970.)
        time(&now) - postavlja varijablu 'now' na trenutno vrijeme
        struct tm  - struktura koja sadrži informacije o vremenu(godina, mjesec, dan, sat, minuta, sekunde...)
        localtime(&now) - konvertira vremenski trenutak iz time_t formata u lokalno vrijeme 
                        i vraća pokazivač na strukturu struct tm koja sadrži informacije o lokalnom vremenu. */
    time_t now = 0;
    time(&now); 
    struct tm *time_info = localtime(&now);
    char time_buffer[50];
    strftime(time_buffer, sizeof(time_buffer), "%c", time_info);
    ESP_LOGI(TAG, "************ %s ***********", time_buffer);
}

void on_got_time(struct timeval *tv)
{

    printf("on got callback %lld\n", tv->tv_sec);
    xSemaphoreGive(got_time_semaphore);
}

void app_main() {
    got_time_semaphore = xSemaphoreCreateBinary();

    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);  // Postavi vremensku zonu sa podrškom za ljetno računanje vremena
    tzset();

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
        //inicijalizacija NTP-a
        esp_sntp_init();
        esp_sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED); //sinkronizacija NTP-a odmah
        esp_sntp_setservername(0, NTP_SERVER);
        /*postavlja callback funkciju(on_got_time) koja će se pozvati kada ESP32 završi sinhronizaciju vremena putem SNTP-a (Simple Network Time Protocol).*/
        esp_sntp_set_time_sync_notification_cb(on_got_time);

/*ispisivanje vremena nakon uspješne sinhronizacije vremena putem NTP-a.
 semafor (got_time_semaphore) osigurava da se glavna nit zaustavi dok ne primi signal (semaphore) od callback funkcije (on_got_time), 
 tj. da se printanje vremena izvrši nakon što je NTP sinhronizacija završena.
*/
 if (xSemaphoreTake(got_time_semaphore, portMAX_DELAY) == pdTRUE)
    {
        print_time();

    }

}
