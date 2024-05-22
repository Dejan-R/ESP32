/*dejan.rakijasic@gmail.com

primjer: nimBLE - (GATT) server

Za testiranje možemo koristiti nRF connect  (iOS i Android) aplikacije od Nordic Semiconductor.

OMOGUĆAVANJE BLUETOOTH (Bluedroid):
TERMINAL --> pio run -t menuconfig
        → (Top) → Component config → Bluetooth → Bluetooth → Host  
                (X) NimBLE - BLE only   


ESP32 podržava oba bluetooth standarda: 
- Bluetooth Classic (BR/EDR) - starija verzija, podržava veći protok podataka i kontinuiranu razmjenu podataka, ali i troši više energije (nego BLE) 
- BLE (Bluetooh Low Energy) - manji protok podataka, niska potrošnja energije - primjena kod IoT-a 
- bluetooth verzije: 
        - 1.1, 1.2, 2.0, 2.1 i 3.0 --> Bluetooth classic
        - 4.0 --> podržava oboje (classic i BLE)
        - sve kasnije verzije su BLE

- Na HOST strani, ESP32 može koristiti (tzv. Bluetooth stack) - dio softverske infrastrukture na ESP32 koji omogućuje komunikaciju putem Bluetootha:
    -Bluedroid (open source, Google) - Bluetooth Classic (BR/EDR) i Bluetooth Low Energy (BLE)
    -nimBLE (Espressif) - posebno optimiziran za Bluetooth Low Energy (BLE)

BLE: arhitektura (Bluetooth LE protocol stack):
                        |---------------APPLICATION---------------|
                        |-----------------------------------------|
                        |         GAPP             GATT           |
                HOST--> |         ATT              SMP            |
                        |                 L2CAP                   |
                        |-----Host Controller Interface (HCI)-----|
          CONTROLLER--> |              Link Layer                 |
                        |            Phisical Layer               |
                        |-----------------------------------------|

 HOST - koristi se za razvoj aplikacija, 
 CONTROLLER - zadužen za slanje i primanje poruka, upravljanje fizičkom vezom i dr.

- PHY (fizički sloj) -  određuje bežični frekvencijski pojas i modulacijski način korišteni u Bluetooth LE. 
- LL (sloj veze) - središtu stoga BLE - samo šalje ili prima podatke (analizu podataka radi GAP ili ATT). 
- HCI pruža sredstvo komunikacije između host-a i kontrolera, može biti implementiran ili hardverskim sučeljem ili softverskim API-jem.
- L2CAP (Logički upravljač veze i protokol prilagodbe) - pruža usluge povezivanja i omogućuje kontrolu protoka po kanalu i ponovno slanje.
- ATT (protokol atributa) - definira podatke za korisničke naredbe i naredbe operacija (čitanje/pisanje). Bluetooth LE uvodi pojam atributa, koji se koristi za opisivanje podataka u dijelu.
                            ATT također definira naredbe koje podaci mogu koristiti
- SMP (Protokol upravljanja sigurnošću) - šifriranje i sigurnost Bluetooth LE veza.
- GATT (Generički profil atributa) - omogućuje organizaciju i razmjenu podataka između BLE uređaja kroz definirane servise i karakteristike.
- GAP (Generički pristupni profil) - definira korisne podatke u LL-u, pružajući najjednostavniji način za analizu LL opterećenja (emitiranje, skeniranje i iniciranje veza)

Uloge BLE uređaja:
 - Advertiser - emitira oglašivačke pakete koji sadrže informacije o uređaju ili uslugama koje pruža (primjer beacon)
 - Scanner - detektira oglašivače u okolini i može reagirati na njihove emitirane podatke.
 - Initiator - inicira uspostavu Bluetooth veze s drugim uređajem.

    Master mode: uređaj djeluje kao centralna točka i obavlja funkcije poput skeniranja za druge uređaje (slaves) i iniciranja veze.
        U kontekstu BLE specifikacije, uređaj u master modu djeluje kao GATT klijent koji traži i povezuje se s GATT serverima radi pristupa uslugama i atributima.
    Slave mode: uređaj oglašava svoje prisustvo i čeka na uspostavu veze od strane master uređaja.
        U kontekstu BLE specifikacije, uređaj u slave modu djeluje kao GATT server koji pruža usluge i atribute te čeka na zahtjeve za pristupom od strane GATT klijenata.

    Bluetooth mreža: (prema topologiji)
     - piconet (svaka uspostava Bluetooth veze) - dva ili više uređaja koji zauzimaju isti fizički kanal.
     - scatternet - sastoji se od više piconet mreža, svaki uređaj može biti master u jednom piconetu i slave u drugom.
     - mesh - decentralizirana arhitektura - svi uređaji u mreži djeluju kao čvorovi koji mogu slati, primati i proslijediti podatke drugim čvorovima u mreži.

    BLE beacon omogućuje slanje informacija o lokaciji ili identifikaciji na pametne telefone ili druge uređaje koji podržavaju BLE.
    - iBeacon je Appleov protokol koji koristi BLE (Bluetooth Low Energy) za slanje signala, ali postoji niz drugih beacons protokola koje ESP-IDF može podržati: 
    - Eddystone - otvoreni standard (Google), omogućava uređajima emitiranje različitih vrste podataka putem BLE (URL, UID, TLM (telemetry)
    - AltBeacon, ...

    Broadcaster: periodično oglašavanje prisutnosti i slanje obavijesti, oglasnih poruka ili informacija o uređaju drugim uređajima u okolini.
    observer: pasivno promatranje prisutnih uređaja u okolini i prikupljanje informacija o njima bez uspostavljanja aktivnih veza.
Za više informacija:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/bluetooth.html
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "sdkconfig.h"
#include "driver/gpio.h"

#define LED_pin 19
#define BUTTON_pin 18

static uint8_t ble_addr_type;
void ble_app_advertise(void);

/*Definiranje UUID-ova za čitanje i pisanje stanja GPIO-a
   UUID (Uniform Unique Identifier) - koristi se za identifikaciju različitih atributa u BLE uređajima. 
   Ovdje koirstimo 16-bitne UUID (READ_UUID i WRITE_UUID) kako bismo definirali karakteristike koje se koriste za čitanje i pisanje stanja GPIO pina.
   */
#define READ_UUID 0xAAAA
#define WRITE_UUID 0xBBBB


// Funkcija za primanje podataka na ESP32 konfiguriran kao server
static int primanje_podataka(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
  char *podaci = (char *) ctxt->om->om_data;
if (podaci != NULL) {
    printf("Primljeni podaci: %s\n", podaci);
    if (strncmp(podaci, "LED ON", ctxt->om->om_len) == 0) {
        printf("LED ON\n");
        gpio_set_level(LED_pin, 1);
    } else if (strncmp(podaci, "LED OFF", ctxt->om->om_len) == 0) {
        printf("LED OFF\n");
        gpio_set_level(LED_pin, 0);
    } else {
        printf("Nepoznati podaci: %s\n", podaci);
    }
} else {
    printf("Nema primljenih podataka.\n");
}
    return 0;
}


// Funkcija za slanje podataka prilikom čitanja stanja GPIO-a
static int slanje_podataka(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint g_button_state = gpio_get_level(BUTTON_pin);
    char gpio_state_str[10];
    // Pretvaranje stanja GPIO-a u string
    snprintf(gpio_state_str, sizeof(gpio_state_str), "%d", g_button_state);
    // Slanje stanja GPIO-a klijentu
    os_mbuf_append(ctxt->om, gpio_state_str, strlen(gpio_state_str));
    return 0;
}



// Definicija GATT usluge
static const struct ble_gatt_svc_def gatt_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x180),
        .characteristics = (struct ble_gatt_chr_def[]){
            {
                .uuid = BLE_UUID16_DECLARE(READ_UUID), 
                .flags = BLE_GATT_CHR_F_READ,
                .access_cb = slanje_podataka
            },
            {
                .uuid = BLE_UUID16_DECLARE(WRITE_UUID), 
                .flags = BLE_GATT_CHR_F_WRITE,
                .access_cb = primanje_podataka
            },
            {0}
        }
    },
    {0}
};


// Handler za BLE događaje
static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
    // Advertise (oglašavanje)
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
        if (event->connect.status != 0)
        {
            ble_app_advertise();
        }
        break;
    // ponovno oglašavanje
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT DISCONNECTED");
        break;
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI("GAP", "BLE GAP EVENT");
        ble_app_advertise();
        break;
    default:
        break;
    }
    return 0;
}

// Funkcija za oglašavanje uređaja
void ble_app_advertise(void)
{
    // GAP - definicija imena uređaja
    struct ble_hs_adv_fields fields;
    const char *device_name;
    memset(&fields, 0, sizeof(fields));
    device_name = ble_svc_gap_device_name();
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);

    // GAP - definiranje povezivanja uređaja
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND; 
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

// Callback funkcija za inicijalizaciju BLE stack-a
void ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type); 
    ble_app_advertise();                  
}

// host task (beskonačna petlja)
void host_task(void *param)
{
    nimble_port_run(); 
}

void app_main()
{

  esp_rom_gpio_pad_select_gpio(LED_pin);
  gpio_set_direction(LED_pin,GPIO_MODE_OUTPUT);
  esp_rom_gpio_pad_select_gpio(BUTTON_pin);
  gpio_set_direction(BUTTON_pin,GPIO_MODE_INPUT);
  gpio_pullup_en(BUTTON_pin);

    nvs_flash_init(); // Inicijalizacija NVS flash-a
    nimble_port_init();    // Inicijalizacija host stack-a
    ble_svc_gap_device_name_set("ESP32_nimBLE_GATT_server");    // Postavljanje imena servera
    ble_svc_gap_init();  // Inicijalizacija gap servisa
    ble_svc_gatt_init();   // Inicijalizacija gatt servisa
    ble_gatts_count_cfg(gatt_svcs);    // Konfiguracija broja gatt servisa
    ble_gatts_add_svcs(gatt_svcs);   // Dodavanje gatt servisa
    ble_hs_cfg.sync_cb = ble_app_on_sync;     // Postavljanje callback funkcije za sinkronizaciju
    nimble_port_freertos_init(host_task);    // Inicijalizacija zadatka za host

}
