/*ESP32
dejan.rakijasic@gmail.com

Primjer: Pokazivači i funkcije

prijenos varijable u funkciju preko pokazivača
-korisiti se radi uštede memorije jer na taj način se ne kopira vrijednost u nove varijable,
nego samo pokazuje na vrijednost.
- također, pass-by-pointer omogućava da se varijabla unutar funkcije promijeni i da se ta promijenjena
vrijednost zadrži i izvan funkcije
*/
#include <stdio.h>

int a=3;
int b=5;

int *a_pok = &a;
int *b_pok = &b;

int zbroj(int *a_pok, int *b_pok){
    return *a_pok + *b_pok;
}
void app_main() {
  int rezultat = zbroj(a_pok,b_pok);
  printf("rezultat = %d\n", rezultat);
}

