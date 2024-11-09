#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "putnicki.h"
#include "teretni.h"
#include "putnik.h"

int main()
{
    srand(time(NULL)); // postavljanje nasumičnog generatora za upotrebu rand() funkcije, ove funkcije su demonstrirane na laboratorijskoj vježbi kursa C programiranja na prvoj godini
    printf("Da li zelite pokrenuti simulaciju za putnicka ili za teretna vozila?\n");
    char unos[2]; //"input buffer" nosi 2 karaktera: slovo izbora + '\0'...
    do
    {
        printf("==========================================\n");
        printf("1. Simulacija za putnicka\n");
        printf("2. Simulacija za teretna vozila\n");
        printf("K. Kraj rada\n");
        printf("==========================================\n");
        printf("Vas izbor: ");
        fflush(stdin); // sigurnosna metoda u slučaju da neki karakter,simbol ili terminator "ostane" iz prošlog unosa...
        scanf("%s", unos);
        if (!strcmp(unos, "1"))
        {
            simPutnicko();
        }
        else if (!strcmp(unos, "2"))
        {
            simTeretni();
        }
        else if (!strcmp(unos, "k") || !strcmp(unos, "K"))
        {
            break;
        }
        else
            printf("Nevalidan unos! Molimo izaberite jednu od validnih ponudjenih opcija...\n");
    } while (1); // nemam konkretan uslov za izlaz iz petlje jer već imam "break" uslov ukoliko "unos" == "K" || "unos" == "k"...
    printf("Kraj rada!\n");
    return 0;
}