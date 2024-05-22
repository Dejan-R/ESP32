/*ESP32
dejan.rakijasic@gmail.com

Primjer: Pokazivači
Pokazivač (eng. Pointer) - varijabla koja sadrži adresu druge varijable. Koriste se za indirektan pristup podacima (preko adrese)

& (adresni operator) - vraća adresu objekta u memoriji
* vraća vrijednost sadržaja memorije s adrese koju sadrži pokaziva

*/

#include <stdio.h>

int a = 5;
int *a_pok = &a;

void app_main() {
 printf("\n");
 printf("vrijednost varijable a: %d\n", a);
 printf("adresa varijable a: %p\n", &a);
 printf("vrijednost sadržaja memorije na koju pokazivač *a_pok pokazuje: %d\n", *a_pok); 
 printf("adresa varijable a na koju pokazivač a_pok pokazuje: %p\n", a_pok);
 printf("adresa pokazivača a_pok je: %p\n", &a_pok);
}