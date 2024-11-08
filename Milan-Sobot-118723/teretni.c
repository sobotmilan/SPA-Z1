#include "teretni.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <windows.h>

void simTeretni()
{
    printf("Unesite broj teretnih vozila koji se generise za simulaciju: ");
    int n;
    do
    {
        scanf("%d", &n);
    } while (n < 1 || n > 10000); // racionalna gornja granica
    FILE *listaRobe;
    listaRobe = fopen("roba.txt", "r+"); // Pretpostavljam da će se spisak robe koje se utovara UVIJEK biti proslijeđen u direktorijumu u kom se i datoteke nalaze kao 'roba.txt'

    if (listaRobe == NULL)
    {
        printf("Neuspjesno otvaranje datoteke za citanje spiska robe...\n");
        return;
    }
    static int idGenerator = 1; // statička promjenljiva koja će pamtiti vrijednost kroz pozive 'initializeTeretno' funkcije, ali će pritom da se resetuje nakon izvršenja brojačke petlje ispod komentara.
    char **robaNiz = (char **)malloc(INT_MAX * sizeof(char *));
    char *buffer = (char *)malloc(INT_MAX * sizeof(char));
    int i = 0;
    while (fscanf(listaRobe, "%s\n", buffer))
    {
        robaNiz[i] = (char *)malloc(strlen(buffer) * sizeof(char) + 1);
        strcpy(robaNiz[i++], buffer);
        buffer[0] = '\0';
    }
    robaNiz = (char **)realloc(robaNiz, i * sizeof(char *));
    TERETNO **array = (TERETNO **)malloc(n * sizeof(TERETNO *));
    for (int i = 0; i < n; i++)
        *(array + i) = initializeTeretno(&idGenerator, robaNiz, i);
    printf("Uspjesno je generisano %d vozila! Pocinje parkiranje vozila:\n");
    QNODE *front = NULL;
    QNODE *rear = NULL;
    for (int j = 0; j < n; j++)
    {
        put(&front, &rear, array[j]); // parkiranje vozila
        printf("%d %s,%s,%s\n", array[j]->id, array[j]->roba->niz[0], array[j]->roba->niz[1], array[j]->roba->niz[2]);
    }
    printf("Parkiranje vozila zavrseno, otvara se granicni prelaz i zapocinje se pregledanje utovarene robe...\n");
    for (int j = 0; j < n; j++)
    {
        pregledRobe(array[j]);
        get(&front, &rear);
    }
    printf("ZAVRSEN PREGLED VOZILA!\n");
    free(rear);
    free(front);
    for (int j = 0; j < n; j++)
    {
        free(array[j]->roba->niz);
        free(array[j]->roba);
        free(array[j]);
    }
    for (int j = 0; j < i; j++)
        free(robaNiz[j]);
    free(robaNiz);
    free(buffer);
    free(array);
    fclose(listaRobe);
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
int pregledRobe(TERETNO *trenutni)
{
    char *niz[3];
    while (!isEmptyTeretni(trenutni->roba))
    {
        int i = 0;
        pop(trenutni->roba, &niz[i++]);
        printf("Roba %s je istovarena!");
    }
    printf("Pregled robe je zavrsen, vrsi se ponovni utovar robe nazad u vozilo!\n");
    while (!isFull(trenutni->roba))
    {
        push(trenutni->roba, niz, 3);
    }
    printf("Zavrsen pregled vozila sa identifikatorom %d!\n", trenutni->id);
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
    for (int i = 0; i < 3; i++)
    {
        if (s->niz[i])
            if (!strcmp(s->niz[i], str))
                return 1;
    }
    return 0;
}
int isFull(STEK *s)
{
    return s->tos == 2;
}
int isEmptyTeretni(STEK *s)
{
    return s->tos == -1;
}
int pushInOrder(STEK *s, char **niz, int i)
{
    if (!isFull(s))
    {
        s->niz[s->tos] = (char *)malloc(strlen(niz[i]) * sizeof(char) + 1);
        strcpy(s->niz[++s->tos], niz[i]);
        return 1;
    }
    return 0;
}
int push(STEK *s, char **niz, int n)
{
    if (!isFull(s))
    {
        int i = (rand() % n) + 1;
        while (conflict(s, niz[i]))
            i = (rand() % n) + 1;
        s->niz[s->tos] = (char *)malloc(strlen(niz[i]) * sizeof(char) + 1);
        strcpy(s->niz[++s->tos], niz[i]);
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
        free(s->niz[s->tos--]);
        return 1;
    }
    return 0;
}

TERETNO *initializeTeretno(int *idGenerator, char **robaNiz, int n)
{
    TERETNO *local = (TERETNO *)malloc(sizeof(TERETNO));
    if (local == NULL)
    {
        printf("Neuspjesno izvrsavanje simulacije.\n");
        return NULL;
    }
    local->id = *idGenerator++;
    local->kolicinaRobe = (rand() % 10000) + 1;
    local->roba = (STEK *)malloc(sizeof(STEK));
    if (local->roba == NULL)
    {
        printf("Neuspjesno izvrsavanje simulacije.\n");
        free(local);
        return NULL;
    }
    local->roba->tos = -1; // stek prazan...
    for (int i = 0; i < 3; i++)
    {
        if (!push(local->roba, robaNiz, n))
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