#include "putnicki.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

PUTNICKO *initializePutnicko(int *idGenerator) // idGenerator je proslijedjen po adresi, jer se inicijalizatorska funkcija poziva onoliko puta koliko vozila generišem, a neophodno mi je da on zadrži svoju vrijednost pri izlazu iz funkcije...
{
    PUTNICKO *local = (PUTNICKO *)malloc(sizeof(PUTNICKO));
    local->id = (*idGenerator)++;        // bez obzira na to koliko cifara identifikator vozila sadržavao (5 cifara maksimalno), on će uvijek biti prikazan sa 5 cifara, jer ću u svakoj printf naredbi gdje se 'id' polje prikazuje koristiti %05d kao specifikator formata ispisa. Ovime mogu imati teoretski 5-cifreni broj vozila...
    local->brPutnika = (rand() % 5) + 1; // 1-5 putnika nasumično generisanih
    local->putnici = (PUTNIK *)malloc(local->brPutnika * sizeof(PUTNIK));
    local->putnici[0].starost = (rand() % 82) + 18;
    /*
    OBJASNJENJE: Prilikom testiranja programa, često mi se dešavalo da se za neko vozilo generiše samo jedan, maloljetni putnik, sto je nemoguće (protivzakonito), pa linijom 'local->putnici[0].starost = (rand() % 82) + 18;' generišem jednog putnika starosti 18-99 godina koji će biti 'vozač' (iako nidgje u programu on nije, niti će biti eksplicitno naznačen kao vozac), a zatim u brojačkoj petlji ispod generišem 'brPutnika-1' dodatnih 'saputnika' (naravno, ako je brPutnika == 1, brojačka petlja se nece izvrsiti...);
    */
    local->brMaleDjece = 0;
    /*
    inicijalizacija broja male djece starosti 10 godina i manje, u nastavku inkrementacija te promjenljive (ukoliko ima djece)
    */
    for (int i = 1; i < local->brPutnika; i++) // petlja kreće od 1 a ne 0 jer smo prvog putnika već alocirali! (local->putnici[0].starost=...)
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
    } while (n < 1 || n > 99999); // pošto je identifikator petocifreni zbog ispisa u formatu %05d, ograničavam broj generisanih vozila na najveći petocifreni cijeli broj.

    int idGenerator = 1;
    /*
    promjenljiva koja će pamtiti vrijednost kroz pozive 'initializePutnicko' funkcije, ali će pritom da se resetuje nakon izlaza iz "simPutnicko" (time svakim narednim pokretanjem simulacije dodjela identifikatora kreće od 00001 opet, naravno ovo je moguće tako što idGenerator dobije prefiks "static", i time mu se proširi životni vijek, tako da više ne zavisi od "simPutnicko"...)
    */
    PUTNICKO **array = (PUTNICKO **)malloc(n * sizeof(PUTNICKO *));
    for (int i = 0; i < n; i++)
        *(array + i) = initializePutnicko(&idGenerator);
    // idGenerator se prenosi po adresi jer želim da se promjena nelokalnog stanja (bočni efekat) odrazi na dodjelu identifikatora za naredno vozilo!
    // Pokretanje simulacije...
    printf("========================================================================\n");
    printf("    Na granici se nalazi %d vozila! Podaci ovih vozila su sljedeci...\n", n);
    printf("========================================================================\n");
    printf("ID      BROJ PUTNIKA    BROJ MALE DJECE     STAROST PUTNIKA\n");
    printf("========================================================================\n");
    for (int i = 0; i < n; i++)
    {
        printf("%05d   %-16d%-20d", array[i]->id, array[i]->brPutnika, array[i]->brMaleDjece);
        for (int j = 0; j < array[i]->brPutnika; j++)
            printf("%-d ", array[i]->putnici[j].starost);
        printf("\n");
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
    printf("ZAVRSENA KONTROLA VOZILA!\n");
    free(array);
    /*
    Oslobadjanje memorije za svako putnicko vozilo individualno sam izvrsio na kraju funkcije 'pasosProces', tako sto sam vozila posmatrao prevashodno kao informacioni sadrzaj čvorova uređene, jednostruko ulančane liste (kojom sam postigao efekat prioritetnog reda). Iz tog razloga, ovdje samo oslobadjam pokazivač na pokazivač (double pointer), pokazivači - članovi niza pokazivaca su oslobodjeni u 'pasosProces'...
    */
}

void kontrolaPasosa(PUTNICKO **array, int n)
{
    NODE *head = NULL; // glava liste koja vrši funkciju prioritetnog reda...
    for (int i = 0; i < n; i++)
        if (!addSorted(&head, array[i]))
        {
            printf("PREKID KONTROLE PASOSA!\nPRIVREMENA OBUSTAVA RADA!\n"); // za svaki slučaj imam nasilan izlaz iz programa, prevashodno da se greške mogu debuggovati lakše.
            return;
        }
    for (int i = 0; i < n; i++)
        pasosProces(&head);
    /*
    ovo je u sustini samo nekakva "handler" funkcija koja vrši inicijalizaciju glave liste, a sve ostale procese (operacije dodavanja u listu, kao i kontrole pasoša) delegira na "helper" funkcije koje poziva kroz dvije različite brojačke petlje...
    */
}

NODE *alokator(PUTNICKO *info) //"helper" funkcija koja radi posao alociranja dinamičke memorije, te dodjeljivanja proslijeđene strukture putničkog vozila...
{
    NODE *new = (NODE *)malloc(sizeof(NODE));
    if (new == NULL)
        return NULL; // safeguard mehanizmi...
    new->content = info;
    if (new->content == NULL)
    {
        free(new);
        return NULL; // safeguard mehanizmi...
    }
    new->next = NULL;
    return new;
}

int addSorted(NODE **head, PUTNICKO *new)
{
    NODE *newNode = alokator(new);
    if (newNode == NULL)
        return 0;

    if (*head == NULL || (*head)->content->brMaleDjece < new->brMaleDjece)
    {
        newNode->next = *head;
        *head = newNode;
    }
    else
    {
        NODE *current = *head;
        while (current->next != NULL && (current->next)->content->brMaleDjece >= new->brMaleDjece)
            current = current->next;

        newNode->next = current->next;
        current->next = newNode;
    }
    return 1;
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