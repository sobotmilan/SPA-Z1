#include "putnik.h"

typedef struct putnicko
{
    int id;          // jedinstvena numeracija vozila...
    int brPutnika;   // racunajuci djecu
    int brMaleDjece; //<11 god...
    // brDjece < brPutnika za svako vozilo!
    PUTNIK *putnici;
} PUTNICKO;

typedef struct node
{
    PUTNICKO *content;
    struct node *next;
} NODE;
/*
ovom NODE strukturom predstavljen je jedan čvor jednostruko ulančane liste, koja je iskorištena kao implementacija prioritetnog reda...
*/

typedef struct red
{
    PUTNIK *niz;
    int f, r;
} RED; // implementacija sekvencijalnog reda, iskorištenog za kontrolu pasoša...

PUTNICKO *initializePutnicko(int *); // funkcija koja inicijalizuje jedno vozilo koristeci PUTNICKO strukturu
void simPutnicko();                  // funkcija koja se poziva u main-u, sa njom započinje i završava simulacija...
void kontrolaPasosa(PUTNICKO **, int);
int addSorted(NODE **, PUTNICKO *); // funkcija kojom se postiže prioritetni red
NODE *alokatorPutnicki(PUTNICKO *); // funkcija koja vodi brigu o alokaciji memorije za čvor ulančane liste
void pasosProces(NODE **);
int isEmptyPutnicki(RED *);
int enqueue(RED *, PUTNIK);
int dequeue(RED *, PUTNIK *);
