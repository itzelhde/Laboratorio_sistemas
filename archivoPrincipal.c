#include <stdio.h>
#include "interfaz.h"


int main()
{
    char entrada[100];

    int salir = 0;


    while (!salir)
    {
        printf("Sistema Operativo IJ> ");

        fgets(entrada, sizeof(entrada), stdin);

        salir = reconocerComando(entrada);
    }


    return 0;
}