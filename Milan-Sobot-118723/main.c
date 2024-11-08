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
    srand(time(NULL));
    printf("Da li zelite pokrenuti simulaciju za putnicka ili za teretna vozila?\n");
    char unos[5];
    do
    {
        printf("==========================================\n");
        printf("1. Simulacija za putnicka\n");
        printf("2. Simulacija za teretna vozila\n");
        printf("K. Kraj rada\n");
        printf("==========================================\n");
        printf("Vas izbor: ");
        scanf(" %s", unos);
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
    } while (1);
    printf("Kraj rada!\n");
    return 0;
}