/*ESP32
dejan.rakijasic@gmail.com

Primjer: Struktura C jezika - .c i .h datoteke

.c datoteke (izvornog koda, source files) - koje sadrže implementacije funkcija, varijabli i ostalog koda 
koji se koristi u programu.
.h datoteke (zaglavlja, header files) - koriste se za deklaraciju funkcija, tipova podataka i varijabli koje 
se dijele između više C datoteka kako bi se osigurala dosljednost u programu i omogućila kompilacija bez grešaka.
*/
#include <stdio.h>
#include "izracun.h"

void app_main() {
  int rezultat = zbroj(4,5);
  printf("rezultat = %d\n", rezultat);
}