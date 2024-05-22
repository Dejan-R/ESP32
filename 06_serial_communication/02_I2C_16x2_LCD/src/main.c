/* dejan.rakijasic@gmail.com

primjer: primjena I2C protokola za ispis na LCD 16x2 (HD44780)

I2C (Inter Integrated Circuit) Philipsov protokol za serijsku komunikaciju
 - HALF-DUPLEX komunikacija
 - master-slave pristup
 - sabirnička (bus) topologija - moguće adresirati do 128 uređaja
 - dvije linije (komunikacijska kanala): SDA (podatkovna linija) i SCL (takt)

- koristi se I2C library (maxsydney)  za LCD 16x2 (HD44780):
https://github.com/maxsydney/ESP32-HD44780

POVEZIVANJE display-a:
 - SCL - 22
 - SDA - 21
 - GND - GND
 - VCC - 5V

detaljnije o I2C protokolu:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2c.html
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <driver/i2c.h>
#include "HD44780.h" //https://github.com/maxsydney/ESP32-HD44780

#define LCD_ADDR 0x27//ili 0x3F
#define SDA_PIN  21
#define SCL_PIN  22
#define LCD_COLS 16
#define LCD_ROWS 2


void app_main(void)
{
    LCD_init(LCD_ADDR, SDA_PIN, SCL_PIN, LCD_COLS, LCD_ROWS);
        LCD_home();
        LCD_clearScreen();
        LCD_setCursor(1, 0);
        LCD_writeStr("Strukovna skola");
        LCD_setCursor(3, 1);
        LCD_writeStr("Djurdjevac");
}