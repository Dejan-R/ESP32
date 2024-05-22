#ifndef VANJSKA_DATOTEKA_H
#define VANJSKA_DATOTEKA_H

/* Da bi varijabla 'neka_varijabla' bila vidljiva u ostalim datotekama (main.c) mora joj se kod deklaracije 
dodati riječ 'extern', mora biti istog naziva i tipa kao i ona deklarirana u datoteci vanjska_datoteka.c, 
te joj ovdje (vanjska_datoteka.h) ne smije biti dodijeljena vrijsdnost. 
*/
extern int neka_varijabla; 

#endif