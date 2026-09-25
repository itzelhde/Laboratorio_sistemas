#ifndef OPERACIONES_H
#define OPERACIONES_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct
{
    int AX;
    int BX;
    int CX;
    int DX;
} Registros;


/* Obtiene la direccion del registro solicitado */
int obtenerRegistro(char nombre[], Registros *registros, int **registro)
{
    if (strcmp(nombre, "AX") == 0)
    {
        *registro = &registros->AX;
    }
    else if (strcmp(nombre, "BX") == 0)
    {
        *registro = &registros->BX;
    }
    else if (strcmp(nombre, "CX") == 0)
    {
        *registro = &registros->CX;
    }
    else if (strcmp(nombre, "DX") == 0)
    {
        *registro = &registros->DX;
    }
    else
    {
        return 0;
    }

    return 1;
}


/* Verifica si una cadena contiene un numero entero */
int esEntero(char texto[])
{
    int i = 0;

    if (texto[0] == '\0')
    {
        return 0;
    }

    if (texto[0] == '-')
    {
        i++;
    }

    if (texto[i] == '\0')
    {
        return 0;
    }

    for (; texto[i] != '\0'; i++)
    {
        if (!isdigit((unsigned char)texto[i]))
        {
            return 0;
        }
    }

    return 1;
}


/* Obtiene el valor de un registro o de un numero */
int obtenerValor(char operando[], Registros *registros, int *valor)
{
    int *registro;

    if (obtenerRegistro(operando, registros, &registro))
    {
        *valor = *registro;
        return 1;
    }

    if (esEntero(operando))
    {
        *valor = atoi(operando);
        return 1;
    }

    return 0;
}


int sintaxisValida(char linea[])
{
    int longitud;
    int i;

    longitud = strlen(linea);

    /* Sintaxis de INC Ax o DEC Ax */
    if ((strncmp(linea, "INC", 3) == 0 ||
         strncmp(linea, "DEC", 3) == 0) &&
        longitud == 6 &&
        linea[3] == ' ')
    {
        if ((linea[4] == 'A' || linea[4] == 'B' ||
             linea[4] == 'C' || linea[4] == 'D') &&
            linea[5] == 'X')
        {
            return 1;
        }

        return 0;
    }

    /* Sintaxis de MOV, ADD, SUB, MUL y DIV */

    if (longitud < 8)
    {
        return 0;
    }

    /* Debe existir un espacio entre la operacion y el registro */
    if (linea[3] != ' ')
    {
        return 0;
    }

    /* Debe existir una coma inmediatamente despues del destino */
    if (linea[6] != ',')
    {
        return 0;
    }

    /* No debe existir espacio despues de la coma */
    if (linea[7] == ' ')
    {
        return 0;
    }

    /* No debe haber espacios ni comas dentro del segundo operando */
    for (i = 7; i < longitud; i++)
    {
        if (isspace((unsigned char)linea[i]) ||
            linea[i] == ',')
        {
            return 0;
        }
    }

    return 1;
}


/* Muestra una fila de la tabla */
void mostrarFila(int pc, Registros *registros, char ir[], char estatus[])
{
    printf("%-4d %-4d %-5d %-5d %-5d %-5d %-15s %-25s\n",
           1,
           pc,
           registros->AX,
           registros->BX,
           registros->CX,
           registros->DX,
           ir,
           estatus);
}

void ejecutarInstruccion(char linea[], int pc, Registros *registros)
{
    char copia[100];

    char *operacion;
    char *destino;
    char *fuente;

    int *registroDestino;
    int valor;

    int lineaVacia = 1;
    int i;

    strcpy(copia, linea);

    /* Verificamos si la linea esta vacia o contiene solamente espacios */
    for (i = 0; copia[i] != '\0'; i++)
    {
        if (!isspace((unsigned char)copia[i]))
        {
            lineaVacia = 0;
            break;
        }
    }

    if (lineaVacia)
    {
        mostrarFila(pc, registros, linea, "Linea vacia");
        return;
    }

    /* Obtenemos la operacion */
    operacion = strtok(copia, " ");

    /* Proteccion por si strtok no encuentra nada */
    if (operacion == NULL)
    {
        mostrarFila(pc, registros, linea, "Linea vacia");
        return;
    }

    /* Verificamos si la operacion existe */
    if (strcmp(operacion, "MOV") != 0 &&
        strcmp(operacion, "ADD") != 0 &&
        strcmp(operacion, "SUB") != 0 &&
        strcmp(operacion, "MUL") != 0 &&
        strcmp(operacion, "DIV") != 0 &&
        strcmp(operacion, "INC") != 0 &&
        strcmp(operacion, "DEC") != 0)
    {
        mostrarFila(pc, registros, linea, "Operacion no reconocida");
        return;
    }

    /* Primero verificamos la sintaxis */
    if (!sintaxisValida(linea))
    {
        mostrarFila(pc, registros, linea, "Error de sintaxis");
        return;
    }

    /* Volvemos a separar la instruccion */
    strcpy(copia, linea);

    operacion = strtok(copia, " ");
    destino = strtok(NULL, ",");
    fuente = strtok(NULL, " ");

    /* Obtenemos el registro destino */
    if (!obtenerRegistro(destino, registros, &registroDestino))
    {
        mostrarFila(pc, registros, linea, "Error de operando");
        return;
    }

    /* INC */
    if (strcmp(operacion, "INC") == 0)
    {
        *registroDestino += 1;

        mostrarFila(pc, registros, linea, "OK");
        return;
    }

    /* DEC */
    if (strcmp(operacion, "DEC") == 0)
    {
        *registroDestino -= 1;

        mostrarFila(pc, registros, linea, "OK");
        return;
    }

    /* Las demas operaciones necesitan una fuente */
    if (!obtenerValor(fuente, registros, &valor))
    {
        mostrarFila(pc, registros, linea, "Error de operando");
        return;
    }

    /* Division entre cero */
    if (strcmp(operacion, "DIV") == 0 && valor == 0)
    {
        mostrarFila(pc, registros, linea, "Error: division entre 0");
        return;
    }

    /* Ejecutamos la operacion */
    if (strcmp(operacion, "MOV") == 0)
    {
        *registroDestino = valor;
    }
    else if (strcmp(operacion, "ADD") == 0)
    {
        *registroDestino += valor;
    }
    else if (strcmp(operacion, "SUB") == 0)
    {
        *registroDestino -= valor;
    }
    else if (strcmp(operacion, "MUL") == 0)
    {
        *registroDestino *= valor;
    }
    else if (strcmp(operacion, "DIV") == 0)
    {
        *registroDestino /= valor;
    }

    mostrarFila(pc, registros, linea, "OK");
}


/* Abre y ejecuta el archivo */
void ejecutarArchivo(char nombreArchivo[])
{
    FILE *archivo;

    char linea[100];

    int pc = 1;

    Registros registros = {0, 0, 0, 0};


    archivo = fopen(nombreArchivo, "r");


    /* Verificamos si el archivo existe */
    if (archivo == NULL)
    {
        printf("Error: el archivo '%s' no ha sido encontrado.\n",
               nombreArchivo);

        return;
    }


    printf("\nEjecutando archivo: %s\n\n", nombreArchivo);


    /* Encabezado de la tabla */
    printf("%-4s %-4s %-5s %-5s %-5s %-5s %-15s %-25s\n",
           "ID",
           "PC",
           "Ax",
           "Bx",
           "Cx",
           "Dx",
           "IR",
           "Estatus");


    printf("-------------------------------------------------------------------------------\n");


    /* Leer archivo linea por linea */
    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        /* Quitamos solamente el salto de linea */
        linea[strcspn(linea, "\n")] = '\0';

        /* Ejecutamos la instruccion */
        ejecutarInstruccion(linea, pc, &registros);

        /* Cada linea representa una instruccion */
        pc++;
    }


    fclose(archivo);

    printf("\nFin de la ejecucion.\n");
}

#endif