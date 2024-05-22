/*ESP32
dejan.rakijasic@gmail.com

Primjer: Pokazivači i polja
*/

#include <stdio.h>

int polje_podataka[10] = { 0,1,2,3,4,5,6,7,8,9 };
int *p = polje_podataka;

void app_main() {
for (int i = 0; i < 10; i++) {

 printf(" %d\n", *(p + i));

}
}