#include "putnicki.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

PUTNICKO *initializePutnicko(int *idGenerator)
{
    PUTNICKO *local = (PUTNICKO *)malloc(sizeof(PUTNICKO));
    local->id = *idGenerator++;          // bez obzira na to koliko cifara identifikator vozila sadržavao (5 cifara maksimalno), on će uvijek biti prikazan sa 5 cifara, jer ću u svakoj printf naredbi gdje se 'id' polje prikazuje koristiti %05d kao specifikator formata ispisa. dakle, 5-cifreni identifikatori!
    local->brPutnika = (rand() % 5) + 1; // 1-5 putnika generisanih
    local->putnici = (PUTNIK *)malloc(local->brPutnika * sizeof(PUTNIK));
    local->putnici[0].starost = (rand() % 82) + 18;
    /*
    OBJASNJENJE: Prilikom testiranja programa, često mi se dešavalo da se za neko vozilo generiše samo jedan, maloljetni putnik, sto je nemoguće (protivzakonito), pa linijom 'local->putnici[0].starost = (rand() % 82) + 18;' generišem jednog putnika starosti 18-99 godina koji će biti 'vozač' (iako nidgje u programu on nije eksplicitno naznačen kao vozac), a zatim u brojačkoj petlji ispod generišem 'brPutnika-1' dodatnih 'saputnika' (naravno, ako je brPutnika == 1, brojačka petlja se nece izvrsiti...);
    */
    local->brMaleDjece = 0;
    /*
    inicijalizacija broja djece, u nastavku inkrementacija (ukoliko ih ima)
    */
    for (int i = 1; i < local->brPutnika; i++)
    {
        local->putnici[i].starost = (rand() % 100) + 1; // 1-100 starost putnika...
        if (local->putnici[i].starost < 11)
            local->brMaleDjece++;
    }
    return local;
}

void simPutnicko()
{
    printf("Unesite broj vozila koji ce se generisati u ovoj simulaciji: ");
    int n;
    do
    {
        scanf("%d", &n);
    } while (n < 1 || n > 10000); // zadao sam neku smislenu gornju granicu...

    static int idGenerator = 1; // statička promjenljiva koja će pamtiti vrijednost kroz pozive 'initializePutnicko' funkcije, ali će pritom da se resetuje nakon izvršenja brojačke petlje ispod komentara.
    PUTNICKO **array = (PUTNICKO **)malloc(n * sizeof(PUTNICKO *));
    for (int i = 0; i < n; i++)
        *(array + i) = initializePutnicko(&idGenerator); // idGenerator se prenosi po adresi jer se njegova 'static' osobina odražava samo na simPutnicko (zbog scope-a promjenljive)
    /*
    Sve funkcije u ovom programu su napisane u redoslijedu u kom su poredane u ovom dokumentu, pa sam odlučio da alokaciju memorije u dinamičkoj zoni za svako putnicko vozilo pojedinačno vršim unutar funkcije 'initializePutnicko' (umjesto u ovoj brojačkoj petlji), pošto je ona svakako prva funkcija koju sam napisao u ovoj datoteci.
    */
    idGenerator = 1;
    // Pokretanje simulacije...
    printf("========================================================================\n");
    printf("    Na granici se nalazi %d vozila! Podaci ovih vozila su sljedeci...\n", n);
    printf("========================================================================\n");
    for (int i = 0; i < n; i++)
    {
        printf("ID VOZILA: %05d\n", array[i]->id);
        printf("BROJ PUTNIKA U VOZILU: %d\n", array[i]->brPutnika);
        printf("BROJ MALE DJECE U VOZILU: %d\n", array[i]->brMaleDjece);
        for (int j = 0; j < array[i]->brPutnika; j++)
            printf("STAROST PUTNIKA %d: %d\n", j + 1, array[i]->putnici[j].starost);
        printf("\n\n");
    }
    printf("========================================================================\n");
    printf("                         KRAJ SPISKA VOZILA!\n");
    printf("========================================================================\n");
    printf("Otvara se granicni prelaz, a prethodno ispisana vozila sada podlijezu granicnoj kontroli pasosa putnika...\n\n");
    Sleep(5000);
    /*
    Da bih mogao koliko toliko ispratiti šta se dešava u realnom vremenu izvršavanja, dodajem ovu pauzu od 5 sekundi čisto da bih mogao prelistati konzolu i vidjeti šta je ispisano/generisano...
    */
    kontrolaPasosa(array, n);

    free(array);
    /*
    Oslobadjanje memorije za svako putnicko vozilo individualno sam izvrsio na kraju funkcije 'pasosProces', tako sto sam vozila posmatrao prevashodno kao informacioni sadrzaj čvorova uredjene, jednostruko ulancane liste (kojom sam postigao efekat prioritetnog reda). Iz tog razloga, ovdje samo oslobadjam pokazivac na niz pokazivac (double pointer), pokazivaci - clanovi niza pokazivaca su oslobodjeni u 'pasosProces'...
    */
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
    int numCtrl = 0;
    for (int i = 0; i < n; i++)
    {
        pasosProces(&head);
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
        while (lister->next && lister->content->brMaleDjece > new->brMaleDjece)
            lister = lister->next;
        if (lister->content->brMaleDjece > new->brMaleDjece || lister->content->brMaleDjece == new->brMaleDjece)
        {
            return insertAfter(lister, new);
        }
        else
        {
            return insertBefore(lister, new);
        }
    }
}

void pasosProces(NODE **head)
{
    if (*head == NULL)
        return;
    /*
    Nisam uspio shvatiti zašto, ali bez ove if naredbe iznad komentara, funkcija 'pasosProces' se pozivala za 1 put više nego što vozila ima (čvorova u ulančanoj listi, sortiranoj po broju male djece u opadajućem redoslijedu), što je dovodilo do 'segmentation fault' na tom posljednjem, suvišnom pozivu i obaranju cijelog programa. Jako neelegantno rješenje kroz ovu if naredbu, ali drugog rješenja za sada nemam, niti razumijem zašto postoji taj jedan poziv viška...
    */
    printf("Kontrola vozila sa identifikatorom %05d:\n", (*head)->content->id);
    printf("Predaja pasosa...\n");
    NODE *trenutni = *head;
    *head = trenutni->next;
    RED pasosi;
    pasosi.f = pasosi.r = 0;
    pasosi.niz = (PUTNIK *)malloc(sizeof(PUTNIK) * trenutni->content->brPutnika);
    for (int i = 0; i < trenutni->content->brPutnika; i++)
    {
        if (!enqueue(&pasosi, trenutni->content->putnici[i]))
        {
            printf("NEUSPJESNA KONTROLA PASOSA!\n");
            free(pasosi.niz);
            return;
        }
        else
        {
            printf("Predat pasos putnika starog %d. godina!\n", trenutni->content->putnici[i].starost);
        }
    }
    printf("Pasosi predati! Pocetak kontrole...\n");
    for (int i = 0; i < trenutni->content->brPutnika; i++)
    {
        PUTNIK retriever;
        if (!dequeue(&pasosi, &retriever))
        {
            printf("NEUSPJESNA KONTROLA PASOSA!\n");
            free(pasosi.niz);
            return;
        }
        else
        {
            printf("Uspjesno pregledan pasos putnika od %d godina starosti!\n", retriever.starost);
            retriever.starost = 0;
        }
    }
    printf("Zavrsena kontrola vozila sa identifikatorom %05d ...\n\n", trenutni->content->id);
    free(pasosi.niz);
    free(trenutni->content->putnici);
    free(trenutni->content);
    free(trenutni);
}

int isEmpty(RED *kb)
{
    return kb->f == kb->r;
}

int enqueue(RED *kb, PUTNIK novi)
{
    kb->niz[kb->r++] = novi;
    return 1;
}

int dequeue(RED *kb, PUTNIK *retriever)
{
    if (!isEmpty(kb))
    {
        *retriever = kb->niz[kb->f++];
        return 1;
    }
    else
        return 0;
}