#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "putnicki.h"
#include "teretni.c"
#include "putnik.h"

int main()
{
    printf("Da li zelite pokrenuti simulaciju za putnicka ili za teretna vozila?\n");
    char unos;
    do
    {
        printf("==========================================\n");
        printf("1. Simulacija za putnicka\n");
        printf("2. Simulacija za teretna vozila\n");
        printf("K. Kraj rada\n");
        printf("==========================================\n");
        printf("Vas izbor: ");
        scanf(" %c", &unos);
        if (unos == '1')
        {
            printf("Putnicki placeholder.\n");
        }
        else if (unos == '2')
        {
            printf("Teretni placeholder.\n");
        }
        else if (unos == 'k' || unos == 'K')
        {
            break;
        }
        else
            printf("Nevalidan unos! Molimo izaberite jednu od validnih ponuđenih opcija...\n");
    } while (1);
    printf("Kraj rada!\n");
    return 0;
}