/* dejan.rakijasic@gmail.com

primjer: primjena I2C protokola za ispis na OLED 128x64 (SSD1306)

I2C (Inter Integrated Circuit) Philipsov protokol za serijsku komunikaciju
 - HALF-DUPLEX komunikacija
 - master-slave pristup
 - sabirnička (bus) topologija - moguće adresirati do 128 uređaja
 - dvije linije (komunikacijska kanala): SDA (podatkovna linija) i SCL (takt)

- koristi se I2C library (NaitikSanas) za OLED (SSD1306):
https://github.com/NaitikSanas/SSD1306-Driver-esp32-with-ESP-IDF/tree/main

POVEZIVANJE display-a:
 - SCL - 22
 - SDA - 21
 - GND - GND
 - VCC - 3.3V
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2c.html

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "ssd1306.h" //https://github.com/NaitikSanas/SSD1306-Driver-esp32-with-ESP-IDF/tree/main
#include "fonts.h" //https://github.com/NaitikSanas/SSD1306-Driver-esp32-with-ESP-IDF/tree/main

int i = 0;

void ispis_na_OLED(){
    char str [16];
    while (1)
    {
        SSD1306_GotoXY(50,40);
        vTaskDelay(10);
        sprintf(str,"%d",i);
        SSD1306_Puts(str,&Font_11x18,1);
        i = i + 1;
        SSD1306_UpdateScreen();
    }  
}

void app_main(){
    SSD1306_Init();
    
    char str[20];
    SSD1306_GotoXY(0,1);
    sprintf(str,"Strukovna skola");
    SSD1306_Puts(str, &Font_7x10, 1);
    SSD1306_GotoXY(10,20);
    sprintf(str,"Djurdjevac");
    SSD1306_Puts(str, &Font_7x10, 1);
    SSD1306_UpdateScreen();
    vTaskDelay(1);
    SSD1306_GotoXY(10,10);
    
    xTaskCreate(&ispis_na_OLED, "ispis_na_OLED", 2046, NULL, 5, NULL);
}

