/*ESP32
dejan.rakijasic@gmail.com

Primjer: DAC

ESP32 ima dva 8-bitna DAC (digitalno-analogni pretvarač) kanala povezana na GPIO25 (kanal_1) i GPIO26 (kanal_2). 
Svaki DAC kanal može pretvoriti digitalnu vrijednost 0~255 u analogni napon 0~Vref (referentni napon 'Vref' ovdje je ulazni s pina VDD3P3_RTC, što je idealno jednako VDD napajanja). 

Izlazni napon može se izračunati na sljedeći način:

out_voltage = Vref * digi_val / 255

DAC periferija podržava izlaz analognog signala na sljedeće načine:
    - Izravno emitiranje napona. DAC kanal nastavlja emitirati određeni napon.
    - Izlaz kontinuiranog analognog signala putem DMA. DAC pretvara podatke u međuspremniku na određenoj frekvenciji.
    - Izlaz kosinusnog vala pomoću generatora kosinusnog vala. DAC kanal može emitirati kosinusni val s određenom frekvencijom i amplitudom.

https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/dac.html

*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/dac.h"

void app_main()
{
    dac_output_enable(DAC_CHANNEL_1);
    dac_output_voltage(DAC_CHANNEL_1, 130);
}