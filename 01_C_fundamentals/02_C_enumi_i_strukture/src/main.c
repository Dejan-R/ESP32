/*ESP32
dejan.rakijasic@gmail.com

Primjer: enumi i strukture

enum - vrsta varijable definirana od strane korisnika
struktura -kolekcija jedne ili više varijabli (moguće različitih tipova) grupiranih zajedno pod jednim imenom radi lakšeg rukovanja podacima.
*/
#include <stdio.h>

typedef enum
{
    no_connection,
    connected
}connection_status;

typedef struct
{
connection_status connection_state;
uint8_t network_ID;
uint8_t password;
}network_s;

typedef struct
{
network_s network;
uint8_t zone;
}IoT_s;

IoT_s myhomenetwork;

void app_main() {

myhomenetwork.network.connection_state = no_connection;
myhomenetwork.network.network_ID=2;
myhomenetwork.network.password=123;
myhomenetwork.zone=4;

}