/*ESP32
dejan.rakijasic@gmail.com

Primjer: Pokazivači i strukture

Definiramo strukturu s_zbroj koja sadrži dva podatka (a i b), te funkciju izracun() koja kao argument prima pokazivač na strukturu
s_zbroj, zbroji brojeve a i b, te vraća rezultat.

U glavnom programu definiramo dvije varijable (s1 i s2) tipa s_zbroj, inicijaliziramo ih brojevima koje želimo zbrojiti i pozivamo funkciju izracun() za svaku od njih.
Rezultat ispisujemo preko Serial monitora.
*/

#include <stdio.h>
/*Strukture koristimo radi jednostavnijeg načina pristupa varijablama. Npr. ako imamo nekoliko varijabli koje želimo kao argumente prebaciti u funkciju, 
stvorit ćemo strukturu s članovima i tada, umjesto funcije s puno argumenata, jednu strukturu kao argument prenosimo u funkciju.
*/

typedef struct
{
  uint8_t a;
  uint8_t b;
} s_zbroj;

/*Strukture u funkcije prenosimo preko pokazivača da bi vrijednosti strukture postavljene u funkciji bile zapamćene i izvan nje
 "arrow" operator -> služi za pristupanje strukutrama, s tim da možemo pristupiti na drugi način kako je navedno dolje u komentaru
 return (*s).a + (*s).b
*/
int izracun(s_zbroj *s)
{
  return s->a + s->b;
  // return (*s).a + (*s).b;
}

void app_main() {

  s_zbroj s1 = {3, 4};
  s_zbroj s2 = {5, 5};

    printf("Zbroj z1= %d\n", izracun(&s1));
    printf("Zbroj z2= %d\n", izracun(&s2)); // u funkciju se prenosi adresa strukture
}