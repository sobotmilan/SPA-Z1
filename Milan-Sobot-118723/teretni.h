typedef struct stek
{
    char *niz[3];
    int tos;
} STEK;

typedef struct teretno
{
    int id;           // jedinstvena numeracija vozila...
    int kolicinaRobe; // iako nije bitna količina, čuvaću za svaki slučaj...
    STEK *roba;       // niz čuva 3 različita tipa robe, ne broj artikala...
} TERETNO;

typedef struct qnode
{
    TERETNO *content;
    struct qnode *next;
} QNODE;

void simTeretni();
QNODE *alokatorTeretni(TERETNO *);
int pregledRobe(TERETNO **);
int put(QNODE **, QNODE **, TERETNO *);
int get(QNODE **, QNODE **);
int conflict(STEK *s, char *str);
int isFull(STEK *);
int isEmptyTeretni(STEK *);
int pushInOrder(STEK *, char **, int);
int push(STEK *, char **, int);
int pop(STEK *, char **);
TERETNO *initializeTeretno(int *, char **, int);
