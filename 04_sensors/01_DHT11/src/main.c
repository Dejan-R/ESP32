/*ESP32
dejan.rakijasic@gmail.com

PRIMJER: DHT11 senzor temperature
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "dht11.h"
void app_main()
{
    DHT11_init(4);
    while (1)
    {
        printf("Temperatura: %d \n", DHT11_read().temperature);
        printf("Vlaga: %d \n", DHT11_read().humidity);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}