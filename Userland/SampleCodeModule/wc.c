// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "wc.h"

void wc(int argc, char *argv[])
{
    if (argc != 1)
    {
        printError("Cantidad de parametros incorrecto.\n");
        return;
    }
    int c;
    int count = 1;
    while ((c = getChar()) != -1)
    {   
        putChar(c);
        if (c == '\n')
        {
            count++;
        }
    }
    printf("\nLa cantidad de lineas es: %d.\n", count);
    return;
}