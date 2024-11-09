#include "teretni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void simTeretni()
{
    printf("Unesite broj teretnih vozila koji se generise za simulaciju: ");
    int n;
    do
    {
        scanf("%d", &n);
    } while (n < 1 || n > 10000); // racionalna gornja granica

    FILE *listaRobe;
    listaRobe = fopen("roba.txt", "r");
    /*
    NAPOMENA: UVIJEK PROSLIJEDITI SPISAK ROBE KAO DOKUMENT SA NAZIVOM 'ROBA.TXT', I SMJESTITI GA U ISTI DIREKTORIJUM U KOM SE NALAZI IZVRŠNA DATOTEKA PROGRAMA!
    */

    if (listaRobe == NULL)
    {
        printf("Neuspjesno otvaranje datoteke za citanje spiska robe...\n");
        return; // nasilni kraj izvrsavanja ukoliko se datoteka ne moze otvoriti...
    }

    static int idGenerator = 1; // statička promjenljiva koja će pamtiti vrijednost ID-a kroz pozive 'initializeTeretno' funkcije, ali će pritom da se resetuje nakon izvršenja brojačke petlje ispod komentara.

    char **artikli = (char **)malloc(INT_MAX * sizeof(char *));
    /*
    pokazivač na niz pokazivača na podatak tipa char, suštinski dinamički alociran niz stringova koji će pročitati sve artikle iz datoteke i smjestiti ih u memoriju, time smanjuje broj pristupa datoteci u spoljašnjoj memoriji, a nazive artikala "seli" u radnu memoriju (efikasniji pristup listi artikala)...
    */

    if (artikli == NULL)
    {
        printf("Neuspjesno citanje ulazne datoteke...\n");
        fclose(listaRobe);
        return; // ukoliko alokacija iznad ne uspije, nasilno izlazimo iz programa...
    }

    char buffer[256]; // sumnjam da postoji artikl koji u nazivu nosi 255 slova...
    int i = 0;        // i = broj artikala procitanih iz datoteke 'roba.txt'

    while (fgets(buffer, sizeof(buffer), listaRobe) != NULL)
    /*
    NAPOMENA: Pretpostavljam da se
    */
    {
        int len = strlen(buffer);
        if (buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';
        artikli[i] = (char *)malloc((strlen(buffer) + 1) * sizeof(char));
        // alokacija memorije za svaki clan niza (svaki artikal pojedinacno)...

        if (artikli[i] == NULL)
            return; // opet, provjera pravilne alokacije memorije za individualne stringove...
        strcpy(artikli[i], buffer);
        i++;
    }
    artikli = (char **)realloc(artikli, i * sizeof(char *));
    TERETNO **array = (TERETNO **)malloc(n * sizeof(TERETNO *));

    for (int j = 0; j < n; j++)
        *(array + j) = initializeTeretno(&idGenerator, artikli, i);
    printf("====================================================================\n");
    printf("    Uspjesno je generisano %d vozila! Pocinje parkiranje vozila:\n", n);
    printf("====================================================================\n");
    printf("ID        REDOSLIJED UTOVARA ROBE\n");
    printf("====================================================================\n");
    QNODE *front = NULL;
    QNODE *rear = NULL;
    for (int j = 0; j < n; j++)
    {
        put(&front, &rear, array[j]);                                                                                               // parkiranje vozila
        printf("%05d     %-15s%-15s%-15s\n", array[j]->id, array[j]->roba->niz[0], array[j]->roba->niz[1], array[j]->roba->niz[2]); // ovdje se može argumentovati da kršim pravila steka(pristupam elementima koji nisu na vrhu steka), ali za potrebe ispisa artikala nemam drukčiji nacin, jedino da praznim stek pa ga ponovo punim artiklima, što je besmisleno...
    }
    printf("====================================================================\n");
    printf("\nParkiranje vozila zavrseno, otvara se granicni prelaz i zapocinje se pregledanje utovarene robe...\n");
    for (int j = 0; j < n; j++)
    {
        printf("\nZapocinje se pregled vozila sa identifikatorom %05d.\n", array[j]->id);
        pregledRobe(&array[j]);
        get(&front, &rear);
    }
    printf("\n\nZAVRSEN PREGLED VOZILA!\n\n");
    free(rear);
    free(front);
    for (int j = 0; j < n; j++)
    {
        free(array[j]->roba->niz);
        free(array[j]);
    }
    for (int j = 0; j < i; j++)
        free(artikli[j]);
    free(array);
    free(artikli);
    fclose(listaRobe);
    idGenerator = 1;
}
QNODE *alokatorTeretni(TERETNO *info)
{
    QNODE *newNode = (QNODE *)malloc(sizeof(QNODE));
    if (newNode == NULL)
        return NULL;
    newNode->next = NULL;
    newNode->content = info;
    return newNode;
}
int pregledRobe(TERETNO **trenutni)
{
    char *niz[3];
    int i = 0;
    while (!isEmptyTeretni((*trenutni)->roba))
    {
        pop((*trenutni)->roba, &niz[i]);
        printf("Artikl '%s' je istovaren!\n", niz[i++]);
    }
    free((*trenutni)->roba);
    printf("Pregled robe je zavrsen, vrsi se ponovni utovar robe nazad u vozilo!\n");

    (*trenutni)->roba = (STEK *)malloc(sizeof(STEK));
    (*trenutni)->roba->tos = -1;

    for (int j = i - 1; j > -1; j--)
    {
        pushInOrder((*trenutni)->roba, niz, j);
    }
    printf("Zavrsen pregled vozila sa identifikatorom %d!\n", (*trenutni)->id);

    return 1;
}
int put(QNODE **front, QNODE **rear, TERETNO *info)
{
    if (*front == NULL ^ *rear == NULL)
        return 0;
    if (*front == NULL && *rear == NULL)
    {
        *rear = alokatorTeretni(info);
        *front = *rear;
        return (*rear != NULL);
    }
    else
    {
        QNODE *new = alokatorTeretni(info);
        (*rear)->next = new;
        *rear = new;
        return (new != NULL);
    }
}
int get(QNODE **front, QNODE **rear)
{
    if (*front == NULL)
        return 0;
    QNODE *r = *front;
    *front = r->next;
    if (*front == NULL)
        *rear = NULL;
    free(r);
    return 1;
}
int conflict(STEK *s, char *str)
{
    for (int i = 0; i < s->tos + 1; i++)
    {
        if (s->niz[i])
            if (!strcmp(s->niz[i], str))
                return 1;
    }
    return 0;
}
int isFull(STEK *s)
{
    return s->tos >= 2;
}
int isEmptyTeretni(STEK *s)
{
    return s->tos == -1;
}
int pushInOrder(STEK *s, char **niz, int i)
{
    if (!isFull(s))
    {
        s->tos++;
        s->niz[s->tos] = (char *)malloc((strlen(niz[i]) + 1) * sizeof(char));
        if (s->niz[s->tos] == NULL)
        {
            printf("Neuspjela alokacija memorije...\n");
            return 0;
        }
        strcpy(s->niz[s->tos], niz[i]);
        printf("Utovaren artikl '%s' nazad u vozilo.\n", s->niz[s->tos]);
        return 1;
    }
    return 0;
}
int push(STEK *s, char **niz, int n)
{
    if (!isFull(s))
    {
        int i = (rand() % n);
        while (conflict(s, niz[i]))
            i = (rand() % n);
        s->tos++;
        if (niz[i] != NULL)
            s->niz[s->tos] = (char *)malloc(strlen(niz[i]) * sizeof(char) + 1);
        if (niz[i] == NULL)
            return 0;
        strcpy(s->niz[s->tos], niz[i]);
        return 1;
    }
    return 0;
}
int pop(STEK *s, char **retriever)
{
    if (!isEmptyTeretni(s))
    {
        *retriever = (char *)malloc(strlen(s->niz[s->tos]) * sizeof(char) + 1);
        if (*retriever == NULL)
            return 0;
        strcpy(*retriever, s->niz[s->tos]);
        free(s->niz[s->tos]);
        s->niz[s->tos] = NULL;
        s->tos--;
        return 1;
    }
    return 0;
}

TERETNO *initializeTeretno(int *idGenerator, char **artikli, int n)
{
    TERETNO *local = (TERETNO *)malloc(sizeof(TERETNO));
    if (local == NULL)
    {
        printf("Neuspjesno izvrsavanje simulacije.\n");
        return NULL;
    }
    local->id = (*idGenerator)++;
    local->kolicinaRobe = (rand() % 10000) + 1;
    local->roba = (STEK *)malloc(sizeof(STEK));
    local->roba->niz[0] = local->roba->niz[1] = local->roba->niz[2] = NULL;
    if (local->roba == NULL)
    {
        printf("Neuspjesno izvrsavanje simulacije.\n");
        free(local);
        return NULL;
    }
    local->roba->tos = -1; // stek prazan...
    for (int i = 0; i < 3; i++)
    {
        if (!push(local->roba, artikli, n))
        {
            printf("NEUSPJESAN UTOVAR ROBE U VOZILO!\n");
            free(local->roba);
            free(local);
            return NULL;
        }
    }
    printf("Uspjesno utovareno vozilo sa identifikatorom %05d.\n", local->id);
    return local;
}