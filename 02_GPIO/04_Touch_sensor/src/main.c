/*ESP32
dejan.rakijasic@gmail.com

Primjer: Touch sensor
ESP32 ima 10 GPIO veznih za touch senzor (promjena kapacitivnosti) - TOUCH0 - TOUCH9

Opis API-ja podijeljen je u sljedeće skupine:
    -Inicijalizacija upravljačkog programa dodirne pločice
    -Konfiguracija GPIO pinova dodirne ploče
    -Uzimanje mjerenja
    -Podešavanje parametara mjerenja
    -Filtriranje mjerenja
    -Metode detekcije dodira
    -Postavljanje prekida za prijavu otkrivanja dodira
    -Buđenje iz stanja mirovanja nakon prekida

za više detalja pogledajte:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/touch_pad.html
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/touch_pad.h"

#define TOUCH_PAD_GPIO4_CHANNEL TOUCH_PAD_NUM0

void app_main()
{
    touch_pad_init(); //inicijalizacija touch senzora

    /*optimizacija referentnog napona 
    Ova funkcija uzima tri parametra gdje smo odredili maksimalni napon kao 2,7 V i minimalni napon kao 0,5 V. 
    Prigušenje za maksimalni napon postavljeno je na 1V. 
    Ovaj referentni napon će postaviti raspon unutar kojeg će se dodirne pločice puniti ili prazniti.*/
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);

    touch_pad_config(TOUCH_PAD_GPIO4_CHANNEL, -1); //konfiguracija touch senzora za određeni GPIO (GPIO4)
    touch_pad_filter_start(10); //pokretanje filtera za smanjenje buke (period filtera kalibracije u ms)


    uint16_t vrijednost;
    uint16_t touch_filtered = 0;
    uint16_t touch_raw = 0;

    while (true)
    {

        //funkcije za čitanje neobrađenih ili filtriranih mjerenja sa senzora
        touch_pad_read_raw_data(TOUCH_PAD_GPIO4_CHANNEL, &touch_raw);
        touch_pad_read_filtered(TOUCH_PAD_GPIO4_CHANNEL, &touch_filtered);

        /*čitanje vrijednosti brojača dodirnog senzora. 
        Svaki senzor za dodir ima brojač za brojanje ciklusa punjenja/pražnjenja. 
        Kada se blok ne 'dodirne', možemo dobiti broj brojača. 
        Kada se podloga 'dodirne', vrijednost u brojaču će se smanjiti zbog većeg ekvivalentnog kapaciteta.*/
        touch_pad_read(TOUCH_PAD_GPIO4_CHANNEL, &vrijednost);
        printf("vrijednost = %d raw = %d filtered = %d\n", vrijednost, touch_raw, touch_filtered);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

