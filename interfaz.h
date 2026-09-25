#ifndef INTERFAZ_H
#define INTERFAZ_H

#include <stdio.h>
#include <string.h>

#include "operaciones.h"


int reconocerComando(char entrada[])
{
    char copia[100];

    char *comando;
    char *archivo;


    strcpy(copia, entrada);

    comando = strtok(copia, " \n");
    archivo = strtok(NULL, " \n");


    if (comando == NULL)
    {
        printf("Error: no se ingreso ningun comando.\n");

        return 0;
    }


    if (strcmp(comando, "salir") == 0)
    {
        printf("Saliendo del sistema...\n");

        return 1;
    }


    if (strcmp(comando, "ejecutar") == 0)
    {
        if (archivo == NULL)
        {
            printf("Error: falta indicar el nombre del archivo.\n");

            return 0;
        }

        ejecutarArchivo(archivo);

        return 0;
    }


    printf("Error: comando no reconocido.\n");

    return 0;
}


#endif