#include "wc.h"

int wc(int argc, char *argv[])
{
 if (argc != 1){
     printError("Cantidad de parametros incorrecto.\n");
     return -1;
 }
 int c;
 int count =0;
 while ((c=getChar()) != -1)
 {
     if(c == '\n'){
         count++;
     }
 }
 printf("La cantidad de lineas es: %d.\n", count);
 return 1;
}