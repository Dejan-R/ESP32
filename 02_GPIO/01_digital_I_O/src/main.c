/*ESP32
dejan.rakijasic@gmail.com

Primjer: GPIO (General-Purpose Input/Output)
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/_images/esp32-devkitC-v4-pinout.png

ESP32 čip ima 34 fizička GPIO pina (GPIO0 ~ GPIO19, GPIO21 ~ GPIO23, GPIO25 ~ GPIO27 i GPIO32 ~ GPIO39). 
Svaki pin se može koristiti kao I/O opće namjene ili se može spojiti na interni periferni signal.

Detaljnije:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html#application-example

*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_pin 18
#define tipka_pin 15
void app_main(){

  esp_rom_gpio_pad_select_gpio(LED_pin);
  gpio_set_direction(LED_pin,GPIO_MODE_OUTPUT);

  esp_rom_gpio_pad_select_gpio(tipka_pin);
  gpio_set_direction(tipka_pin,GPIO_MODE_INPUT);
  gpio_pullup_en(tipka_pin);
  while(1){
      int LED_status = gpio_get_level(tipka_pin);
     gpio_set_level(LED_pin, LED_status);
    //vTaskDelay(1000 / portTICK_PERIOD_MS);
     //gpio_set_level(LED_pin, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}