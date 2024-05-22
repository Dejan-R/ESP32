/*dejan.rakijasic@gmail.com

primjer: nimBLE - (GATT) client 

ESP32 čita podatke i šalje podatke GATT serveru, nakon što ga pronađe (primjer 05_nimble_GATT_server) i poveže se.

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

#include "sdkconfig.h"
#include "driver/gpio.h"

#define DEVICE_NAME "ESP32_nimBLE_GATT_server"

static uint8_t ble_addr_type;
void ble_app_scan(void);
    static uint16_t read_attr_handler;//varijabla za rukovanje atributom za čitanje
    static uint16_t write_attr_handler; //varijabla za rukovanje atributom za pisanje
static uint16_t conn_handle;

static bool is_connected = false;

#define READ_UUID 0xAAAA
#define WRITE_UUID 0xBBBB


// Definiranje callback funkcije za čitanje atributa
static int read_attribute(uint16_t conn_handle, const struct ble_gatt_error *error, struct ble_gatt_attr *attr, void *arg)
{
    if (error->status == 0)
    {
        printf("Pročitana vrijednost sa GATT servera: %.*s\n", attr->om->om_len, attr->om->om_data);
    }
    return error->status;
}


//provjerava UUID svake otkrivene karakteristike, te postavlja odgovarajuće handlere za čitanje/pisanje.
int get_all_chars(uint16_t conn_handle, const struct ble_gatt_error *error, const struct ble_gatt_chr *chr, void *arg)
{

    char buffer[50];
    switch (error->status)
    {
    case BLE_HS_EDONE:
        break;
    case 0:
        memset(buffer, 0, sizeof(buffer));
        ble_uuid_to_str(&chr->uuid.u, buffer);
        ESP_LOGI("CHAR", "CHAR %s", buffer);
        if (chr->uuid.u.type == BLE_UUID_TYPE_16)
        {
            if (chr->uuid.u16.value == READ_UUID) // Provjera UUID-a karakteristike za čitanje
            {
                read_attr_handler = chr->val_handle;
            }
            else if (chr->uuid.u16.value == WRITE_UUID) // Provjera UUID-a karakteristike za pisanje
            {
                write_attr_handler = chr->val_handle;
            }
        }
        break;
    default:
        break;
    }

    return error->status;
}



int service_discovery(uint16_t conn_handle, const struct ble_gatt_error *error, const struct ble_gatt_svc *service, void *arg)
{
    static uint16_t start, end;
    char buffer[50];
    switch (error->status)
    {
    case BLE_HS_EDONE:
        ble_gattc_disc_all_chrs(conn_handle, start, end, get_all_chars, NULL);
        break;
    case 0:
        memset(buffer, 0, sizeof(buffer));
        ble_uuid_to_str(&service->uuid.u, buffer);
        ESP_LOGI("SERVICE", "Service %s", buffer);
        if (service->uuid.u.type == BLE_UUID_TYPE_16 && service->uuid.u16.value == 0x180) // Provjera UUID-a usluge
        {
            start = service->start_handle;
            end = service->end_handle;
        }
        break;
    default:
        break;
    }

    return error->status;
}



static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_hs_adv_fields fields;

    switch (event->type)
    {
    case BLE_GAP_EVENT_DISC:
        ble_hs_adv_parse_fields(&fields, event->disc.data, event->disc.length_data);
        if (fields.name_len == strlen(DEVICE_NAME) && memcmp(fields.name, DEVICE_NAME, strlen(DEVICE_NAME)) == 0)
        {
            printf("Uređaj pronađen!\n");
            ble_gap_disc_cancel();
            ble_gap_connect(BLE_OWN_ADDR_PUBLIC, &event->disc.addr, 1000, NULL, ble_gap_event, NULL);
        }
        break;
    case BLE_GAP_EVENT_CONNECT:
        // Otkrivamo sve usluge na serveru nakon uspostave veze
        conn_handle = event->connect.conn_handle;
        ble_gattc_disc_all_svcs(conn_handle, service_discovery, NULL);
        is_connected = true;
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI("GAP", "BLE_GAP_EVENT_DISCONNECT");
        is_connected = false;
                    printf("Pokušavam se ponovno povezati...\n");
            ble_app_scan(); // Ponovno pokreće skeniranje za uređajem
        break;
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI("GAP", "BLE_GAP_EVENT_ADV_COMPLETE");
        break;
    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI("GAP", "BLE_GAP_EVENT_SUBSCRIBE");
        break;
    default:
        break;
    }
    return 0;
}



// Funkcija za traženje uređaja
void ble_app_scan(void)
{
    printf("Tražim uređaje...\n");

    struct ble_gap_disc_params disc_params;
    disc_params.filter_duplicates = 1;
    disc_params.passive = 1;
    disc_params.itvl = 0;
    disc_params.window = 0;
    disc_params.filter_policy = 0;
    disc_params.limited = 0;

    ble_gap_disc(ble_addr_type, BLE_HS_FOREVER, &disc_params, ble_gap_event, NULL);
}

// Callback funkcija za inicijalizaciju BLE stack-a
void ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type);
    ble_app_scan();
}

// host task (beskonačna petlja)
void host_task(void *param)
{
    nimble_port_run();
}

void citanje_vrijednosti(void *pvParameter)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(2000));
        // Pročitamo i ispišemo vrijednost s GATT servera
        if (is_connected) {
            ble_gattc_read(conn_handle, read_attr_handler, read_attribute, NULL);
 
    }
    }
}

// Funkcija koja periodički šalje "LED ON" na GATT server
void slanje_vrijednosti(void *pvParameter)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(5000)); // Čeka 5 sekundi prije slanja sljedeće poruke
        if (is_connected)
        {
            char message[] = "LED ON";
            struct os_mbuf *om = ble_hs_mbuf_from_flat(message, sizeof(message) - 1);
            ble_gattc_write_no_rsp_flat(conn_handle, write_attr_handler, om, NULL);
            os_mbuf_free_chain(om);
        }
    }
}


void app_main()
{
    nvs_flash_init(); // Inicijalizacija NVS flash-a
    esp_nimble_hci_init();      //Inicijalizaicja ESP konktorlera
    nimble_port_init();    // Inicijalizacija host stack-a
    ble_svc_gap_device_name_set("ESP32_nimBLE_GATT_client");    // Postavljanje imena klijenta
    ble_svc_gap_init();  // Inicijalizacija gap servisa
    ble_hs_cfg.sync_cb = ble_app_on_sync;     // Postavljanje callback funkcije za sinkronizaciju
    nimble_port_freertos_init(host_task);    // Inicijalizacija zadatka za host
    xTaskCreate(&citanje_vrijednosti, "čitanje vrijednosti sa GATT servera", 2048, NULL, 5, NULL);
  xTaskCreate(&slanje_vrijednosti, "čitanje vrijednosti sa GATT servera", 2048, NULL, 5, NULL);
}