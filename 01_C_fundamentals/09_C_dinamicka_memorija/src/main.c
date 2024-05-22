/*ESP32
dejan.rakijasic@gmail.com

Primjer: dinamička alokacija memorije

Dinamička alokacija memorije je postupak u kojem se tijekom izvođenja programa alocira memorija prema stvarnim potrebama programa i oslobađa kada više nije potrebna. 
Ovaj proces omogućuje fleksibilno upravljanje memorijom i dinamičko prilagođavanje zahtjevima programa.
Primjer korištenja kod ESP32 može uključivati dinamičku alokaciju za strukture podataka poput nizova, redova, ili drugih varijabilnih 
podataka gdje se veličina ne može unaprijed odrediti. 
Važno je koristiti dinamičku alokaciju pažljivo kako bi se izbjeglo curenje memorije (memory leaks) ili neefikasno upravljanje resursima.

malloc (funkcija) - uzima jedan argument - broj bajtova koji treba alocirati. Vraća pokazivač (adresu bajta) tipa void*,
pa ga treba prebaciti u potrebni tip pokazivača

calloc (funkcija) - uzima dva argumenta - broj elemenata i veličinu svakog elementa u bajtovima koje treba alocirati. Vraća pokazivač (adresu bajta) tipa void*,
pa ga treba prebaciti u potrebni tip pokazivača.

realloc (funkcija) - uzima dva argumenta - pokazivač na prethodno alociranu memoriju i broj bajtova koji treba alocirati nakon realokacije. 
Vraća pokazivač (adresu bajta) tipa void*, pa ga treba prebaciti u potrebni tip pokazivača.

free - oslobađa alociranu memoriju koja je bila alocirana putem malloc, calloc ili realloc.
Važno je osloboditi memoriju kako bi se izbjeglo curenje memorije - memorija koja se dodjeljuje a nikad ne oslobađa.
*/

#include <stdio.h>
#include <stdlib.h>

void app_main() {
    // Primjer korištenja malloc - alocira memoriju za jedan integer.
    int *p_malloc = (int*)malloc(sizeof(int));
    if (p_malloc == NULL) {   /*uvijek treba provjeriti je li vraćeni pokazivač NULL*/
        printf("Alokacija memorije za p_malloc nije uspjela.\n");
        // Dodajte dodatne postupke u slučaju neuspjele alokacije
    }
    *p_malloc = 55;
    printf("Vrijednost p_malloc: %d\n", *p_malloc);

    
   // Primjer korištenja calloc s 3 elemenata, inicijalizira sve elemente na nulu
    int *p_calloc = (int*)calloc(3, sizeof(int));
    if (p_calloc == NULL) {
        printf("Alokacija memorije za p_calloc nije uspjela.\n");
        // Dodajte dodatne postupke u slučaju neuspjele alokacije
    }

    printf("Vrijednosti p_calloc nakon calloc:\n");
    for (int i = 0; i < 3; ++i) {
        printf("%d ", p_calloc[i]);
    }
    printf("\n");

    // Primjer korištenja realloc - Proširuje alocirani blok memorije koji je bio alociran s calloc na veličinu za deset integera
    int *p_realloc = (int*)realloc(p_calloc, 7 * sizeof(int));
    if (p_realloc == NULL) {
        printf("Realokacija memorije nije uspjela.\n");
        free(p_calloc);  // Ako realloc nije uspio, oslobodi prethodno alociranu memoriju
        // Dodajte dodatne postupke u slučaju neuspjele alokacije
    }
    // Inicijalizacija dodanih elemenata na nulu
    for (int i = 3; i < 7; ++i) {
        p_realloc[i] = 0;
    }

    printf("Vrijednosti p_realloc nakon realloc:\n");
    for (int i = 0; i < 7; ++i) {
        printf("%d ", p_realloc[i]);
    }
    printf("\n");

    free(p_realloc);
}
