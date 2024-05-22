/*ESP32
dejan.rakijasic@gmail.com

Primjer: PWM - LED Control (LEDC)
LEDC namijenjen je upravljanju LED, ali se također može koristiti za općenito PWM upravljanje. Podržava do 16 neovisnih kanala PWM signala, 
omogućuje konfiguraciju različitih svojstava kao što su rezolucija, frekvencija i duty cycle, te se koristi za generiranje preciznih PWM signala
 za LED osvjetljenje i druge aplikacije koje zahtijevaju visoku preciznost i stabilnost.

detaljnije informacije:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_CH0_GPIO       (4)
#define LEDC_HS_CH0_CHANNEL_BIT (1ULL << LEDC_HS_CH0_CHANNEL)

void app_main(void)
{
    // Konfiguracija LEDC timera
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_10_BIT,  //Rezolucija PWM signala
        .freq_hz = 5000,                      // Frekvencija signala
        .speed_mode = LEDC_HS_MODE,           // Način rada (high-speed)
        .timer_num = LEDC_HS_TIMER            // Broj timera
    };
    ledc_timer_config(&ledc_timer);

    // Konfiguracija LEDC kanala
    ledc_channel_config_t ledc_channel = {
        .channel    = LEDC_HS_CH0_CHANNEL,
        .duty       = 0,          //Početni duty cycle
        .gpio_num   = LEDC_HS_CH0_GPIO,
        .speed_mode = LEDC_HS_MODE,
        .hpoint     = 0,    //početna točka brojanja - [0, (2**duty_resolution)-1]
        .timer_sel  = LEDC_HS_TIMER
    };
    ledc_channel_config(&ledc_channel);

    int direction = 1;  // 1 - povećava se intenzitet, 0 - smanjuje se intenzitet
    int duty = 0;

    while (1) {
        ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, duty);
        ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
        vTaskDelay(10 / portTICK_PERIOD_MS);

        if (direction) {
            duty += 5;
            if (duty > 1023) {
                duty = 1023;
                direction = 0;
            }
        } else {
            duty -= 5;
            if (duty < 0) {
                duty = 0;
                direction = 1;
            }
        }
    }
}
