#pragma once
#include "putnik.h"
#define MAX 100

typedef struct node NODE;

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
    NODE *next;
} NODE;

typedef struct red
{
    PUTNIK *niz;
    int f, r;
} RED;

PUTNICKO *initializePutnicko(int *);
void simPutnicko();
void kontrolaPasosa(PUTNICKO **, int);
int insertBefore(NODE *, PUTNICKO *);
int insertAfter(NODE *, PUTNICKO *);
int addSorted(NODE **, PUTNICKO *);
NODE *alokator(PUTNICKO *);
void pasosProces(NODE **);
int isEmpty(RED *);
int enqueue(RED *, PUTNIK);
int dequeue(RED *, PUTNIK *);
