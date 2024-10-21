/*dejan.rakijasic@gmail.com

ADC Single Read Example
prema primjeru: https://github.com/espressif/esp-idf/tree/master/examples/peripherals/adc/oneshot_read
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#define EXAMPLE_ADC2_CHAN0 ADC_CHANNEL_5  // GPIO12 = ADC2_5
#define EXAMPLE_ADC_ATTEN ADC_ATTEN_DB_12  // Postavljanje atenuacije na 11dB
void app_main(void)
{
    adc_oneshot_unit_handle_t adc2_handle;
    adc_oneshot_unit_init_cfg_t init_config2 = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config2, &adc2_handle));
    adc_oneshot_chan_cfg_t config = {
        .atten = EXAMPLE_ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, EXAMPLE_ADC2_CHAN0, &config));
    while (1) {
        int adc_raw;
        ESP_ERROR_CHECK(adc_oneshot_read(adc2_handle, EXAMPLE_ADC2_CHAN0, &adc_raw));
      printf( "ADC2 Channel[%d] Raw Data: %d", EXAMPLE_ADC2_CHAN0, adc_raw);
        vTaskDelay(pdMS_TO_TICKS(1000));  // Čekanje 1 sekundu
    }
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc2_handle));
}
