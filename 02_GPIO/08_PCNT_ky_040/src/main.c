/*ESP32
dejan.rakijasic@gmail.com

Primjer: Pulse Counter PCNT - primjer brojanja impulsa sa enkodera ky-040
PCNT - broji rastuće i/ili padajuće bridove ulaznog signala. 

Ky-040 - rotacijski enkoder (daje 4 impulsa po zakretu) - dva izlaza (A i B) koji generiraju impulsne signale kada se enkoder rotira. 
Svaki ciklus rotacije generira četiri impulsa jer se svaka faza (A i B) mijenja četiri puta tijekom potpunog okreta. Enkoder koristi Gray-ov kod.

A   +-----+     +-----+     +-----+
          |     |     |     |
          |     |     |     |
          +-----+     +-----+
B      +-----+     +-----+     +-----+
             |     |     |     |
             |     |     |     |
             +-----+     +-----+
    -------------------------------->
                CW smjer

detaljnije informacije:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/pcnt.html
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"


#define EXAMPLE_PCNT_HIGH_LIMIT 400
#define EXAMPLE_PCNT_LOW_LIMIT -400


#define ENCODER_GPIO_A 0
#define ENCODER_GPIO_B 2

void app_main(void)
{
    pcnt_unit_config_t unit_config = {
        .high_limit = EXAMPLE_PCNT_HIGH_LIMIT,
        .low_limit = EXAMPLE_PCNT_LOW_LIMIT,
    };
    pcnt_unit_handle_t pcnt_unit = NULL;
    pcnt_new_unit(&unit_config, &pcnt_unit);

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config);

    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = ENCODER_GPIO_A,
        .level_gpio_num = ENCODER_GPIO_B,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a);
    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = ENCODER_GPIO_B,
        .level_gpio_num = ENCODER_GPIO_A,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b);

    pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE);
    pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);
    pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE);
    pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);


    pcnt_unit_enable(pcnt_unit);
    pcnt_unit_clear_count(pcnt_unit);
    pcnt_unit_start(pcnt_unit);

    int brojac = 0;
    int b=0;
    while (1) {
     
            pcnt_unit_get_count(pcnt_unit, &brojac);

        if(brojac !=b){
        printf("%d\r\n", brojac/4);
        b=brojac;
        }
            vTaskDelay(100/portTICK_PERIOD_MS);
        }
    }
