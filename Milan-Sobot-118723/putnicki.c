#include "putnicki.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

PUTNICKO *initializePutnicko()
{
    static int idGenerator = 1; // static jer cemo pamtiti vrijednost kroz pozive funkcije za svako naredno vozilo...
    PUTNICKO *local = (PUTNICKO *)malloc(sizeof(PUTNICKO));
    local->id = idGenerator;                 // bez obzira na to koliko cifara identifikator vozila sadrzavao (5 cifara maksimalno), on ce uvijek biti prikazan sa 5 cifara, jer cu u svakoj printf naredbi gdje se 'id' polje prikazuje koristiti %05d kao specifikator formata ispisa. dakle, 5-cifreni identifikatori!
    idGenerator = (idGenerator % 99999) + 1; // opet, ogranicavam raspon generisanih identifikatora na maksimalno 5 cifara.
    local->brPutnika = (rand() % 5) + 1;     // 1-5 putnika generisanih
    local->putnici = (PUTNIK *)malloc(local->brPutnika * sizeof(PUTNIK));
    for (int i = 0; i < local->brPutnika; i++)
        local->putnici[i].starost = (rand() % 100) + 1; // 1-100 starost putnika...
    local->brMaleDjece = 0;
    /*
    inicijalizacija broja djece, u nastavku dodjela
    */
    for (int i = 0; i < local->brPutnika; i++)
        if (local->putnici[i].starost < 11)
            local->brMaleDjece++;
    return local;
}

void simPutnicko()
{
    int n = (rand() % 100) + 1; // radi ustede vremena, implementirana ogranicenja - br. vozila generisanih u simulaciji je 1-100.
    PUTNICKO **array = (PUTNICKO **)malloc(n * sizeof(PUTNICKO *));
    for (int i = 0; i < n; i++)
        *(array + i) = initializePutnicko();

    // run sim...
    printf("========================================================================\n");
    printf("    Na granici se nalazi %d vozila! Podaci ovih vozila su sljedeci...\n", n);
    printf("========================================================================\n");
    for (int i = 0; i < n; i++)
    {
        printf("ID VOZILA: %d\n", array[i]->id);
        printf("BROJ PUTNIKA U VOZILU: %d\n", array[i]->brPutnika);
        for (int j = 0; j < array[i]->brPutnika; j++)
            printf("STAROST PUTNIKA %d: %d\n", j + 1, array[i]->putnici[j].starost);
        printf("\n\n");
    }
    printf("========================================================================\n");
    printf("                         KRAJ SPISKA VOZILA!\n");
    printf("========================================================================\n");
    printf("Otvara se granicni prelaz, a prethodno ispisana vozila sada podlijezu granicnoj kontroli pasosa putnika...\n\n");
    Sleep(5000);
    kontrolaPasosa(array, n);

    for (int i = 0; i < n; i++)
    {
        free((*(array + i))->putnici);
        free(*(array + i));
    }

    free(array);
}

void kontrolaPasosa(PUTNICKO **array, int n)
{
    NODE *head = NULL;
    for (int i = 0; i < n; i++)
        if (!addSorted(&head, array[i]))
        {
            printf("PREKID KONTROLE PASOSA!\nPRIVREMENA OBUSTAVA RADA!\n");
            return;
        }
    Sleep(2000);
    for (int i = 0; i < n; i++)
    {
        printf("Kontrola %d. vozila:\n", i + 1);
        pasosProces(array[i]);
        printf("Zavrsena kontrola %d. vozila...\n\n", i + 1);
    }
}

NODE *alokator(PUTNICKO *info)
{
    NODE *new = (NODE *)malloc(sizeof(NODE));
    if (new == NULL)
        return NULL; // safeguard mehanizmi...
    new->content = info;
    if (new->content == NULL)
        return NULL; // safeguard mehanizmi...
    new->next = NULL;
    return new;
}

int insertBefore(NODE *target, PUTNICKO *new)
{
    if (target == NULL || new == NULL)
        return 0;
    NODE *newNode = alokator(target->content);
    if (newNode == NULL)
        return 0;
    newNode->next = target;
    target->content = new;
    return 1;
}
int insertAfter(NODE *target, PUTNICKO *new)
{
    NODE *newNode = alokator(new);
    if (newNode == NULL)
        return 0;
    newNode->next = target->next;
    target->next = newNode;
    return 1;
}

int addSorted(NODE **head, PUTNICKO *new)
{
    if (new == NULL)
        return 0; // greska pri prosljedjivanju...
    if (*head == NULL)
    {
        *head = alokator(new);
        return (*head != NULL);
    }
    else
    {
        NODE *lister = *head;
        while (lister->next && lister->content->brMaleDjece < new->brMaleDjece)
            lister = lister->next;
        if (lister->content->brMaleDjece < new->brMaleDjece)
        {
            return insertAfter(lister, new);
        }
        else
        {
            return insertBefore(lister, new);
        }
    }
}

void pasosProces(PUTNICKO *trenutni)
{
    printf("Predaja pasosa...\n");
    BAFER pasosi;
    pasosi.f = pasosi.r = 0;
    pasosi.niz = (PUTNIK *)malloc(sizeof(PUTNIK) * trenutni->brPutnika);
    for (int i = 0; i < trenutni->brPutnika; i++)
    {
        if (!enqueue(&pasosi, trenutni->putnici[i]))
        {
            printf("NEUSPJESNA KONTROLA PASOSA!\n");
            return;
        }
        else
        {
            printf("Predat pasos putnika starog %d. godina!\n", trenutni->putnici[i].starost);
        }
    }
    for (int i = 0; i < trenutni->brPutnika; i++)
    {
        PUTNIK retriever;
        if (!dequeue(&pasosi, &retriever))
        {
            printf("NEUSPJESNA KONTROLA PASOSA!\n");
            return;
        }
        else
        {
            printf("Uspjesno pregledan pasos putnika od %d godina starosti!\n", retriever.starost);
            retriever.starost = 0;
        }
    }
}

int isEmpty(BAFER *kb)
{
    return kb->f == kb->r;
}

int enqueue(BAFER *kb, PUTNIK novi)
{
    kb->niz[kb->r] = novi;
    kb->r++;
    return 1;
}

int dequeue(BAFER *kb, PUTNIK *retriever)
{
    if (!isEmpty(kb))
    {
        retriever->starost = kb->niz[kb->f].starost;
        kb->f++;
        return 1;
    }
    else
        return 0;
}