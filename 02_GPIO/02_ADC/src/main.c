/*ESP32
dejan.rakijasic@gmail.com

Primjer: ADC

12-bitni ADC (rezolucija se može mijenjati) --> napon 0-3.3V pretvara u cjelobrojnu vrijednost 0-4096
Proučite GPIO priključke, odnosno koji su ADC1 i ADC2 pinovi!

ADC pretvorba: (neobrađeni digitalni rezultat očitanja)
    adc1_get_raw()
    adc2_get_raw()

Za izračunavanje napona na temelju neobrađenih rezultata ADC-a može se koristiti izraz:

Vout = Dout * Vmax / Dmax       

gdje je:

Vout - napon
Dout - ADC neobrađeni digitalni rezultat očitanja
Vmax - Maksimalni mjerljivi ulazni analogni napon, pogledajte ADC prigušenje
Dmax - Maksimalni rezultat neobrađenog digitalnog očitavanja izlaznog ADC-a (4095)

esp_adc_cal_raw_to_voltage() - koristiti se za dobivanje kalibriranih rezultata pretvorbe i vraća stvarni napon (u mV). 
Ako se API-ji za kalibraciju ADC-a koriste na pločama bez eFuse ADC kalibracijskih bitova, generirat će se upozorenja.

ADC prigušenje:
ESP32 ADC mogu mjeriti analogne napone od 0 V do Vref. Među različitim čipovima, Vref varira (medijan je 1,1 V). 
Zadani ADC napon je za prigušenje od 0 dB i naveden je u donjoj tablici. Postavljanjem većeg prigušenja moguće je očitati veće iznose napona.
Zbog karakteristika ADC-a, najtočniji rezultati se dobivaju unutar "predloženog raspona" prikazanog u sljedećoj tablici.

 - adc1_config_channel_atten() - odabir parametra prigušenja prema:
    0dB 	ADC_ATTEN_DB_0 	    ~100-950 mV
    2,5 dB 	ADC_ATTEN_DB_2_5 	~100-1250 mV
    6 dB 	ADC_ATTEN_DB_6  	~150-1750 mV
    11 dB 	ADC_ATTEN_DB_11 	~150-2450 mV

ADC smetnje:
ESP32 ADC može biti osjetljiv na šum što dovodi do velikih odstupanja u očitanjima ADC-a. 
Ovisno o upotrebi, može se spojiti premosni kondenzator (npr. keramički kondenzator od 100 nF) na ADC ulaznu pločicu koja se koristi, kako bi smanjili šum. 
Osim toga, višestruko uzorkovanje također se može koristiti za dodatno ublažavanje učinaka smetnji.

ADC kalibracija:
esp_adc_cal /include/esp_adc_cal.h API pruža funkcije za ispravljanje razlika u izmjerenim naponima uzrokovanih varijacijama ADC referentnih napona (Vref) između čipova.
Po dizajnu ADC referentni napon je 1100 mV, međutim pravi referentni napon može biti u rasponu od 1000 mV do 1200 mV među različitim ESP32.
 
Za detalje pogledajte: 
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc_calibration.html#introduction

*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

void app_main()
{

   adc1_config_width(ADC_WIDTH_BIT_12);
   adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11); 

    int Dout;
   while (true)
   {
       Dout = adc1_get_raw(ADC1_CHANNEL_4);
       printf("ADC vrijednost= %d\n", Dout);
   }
   
}