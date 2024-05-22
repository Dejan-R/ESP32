/*ESP32
dejan.rakijasic@gmail.com

Primjer: statičke varijable

Identifikatori memorijske klase (auto, extern, static i register) opisuju:
    - opseg varijable
    - vijek trajanja varijable
    - mjesto pohrane varijable
    - inicijaliziranu vrijednost varijable
    - pristup varijabli
- Lokalane varijable se pohranjuju na stog, a globalne i statičke u RAM.

AUTO - automatska varijabla - sve varijable deklarirane (bez 'STATIC') unutar bloka su auto, a izvan blokova statičke varijable.
EXTERN - globalne varijable - može im se pristupiti iz bilo kojeg bloka
STATIC 
- globalne statičke - definiraju se izvan f-ja (kao i obične globalne), ali nisu vidljive u drugim datotekama
-lokalne statičke varijable - definirane su i vidljive unutar f-je ali zadržavaju vrijednost i nakon izlaska iz f-je (za razliku od običnih lokalnih varijabli)
REGISTER - koristi se kada varijablu želimo (radi bržeg pristupa) pohraniti u registar. Kako registar nema adresu, nad ovim varijablama nisu primjenjivi adresni operatori
  
  primjer: 
        register int a = 55;
*/
#include <stdio.h>
#include "vanjska_datoteka.h"

extern int neka_varijabla;  // Deklaracija vanjske varijable

void app_main() {
    printf("Vrijednost vanjske varijable: %d\n", neka_varijabla);
}