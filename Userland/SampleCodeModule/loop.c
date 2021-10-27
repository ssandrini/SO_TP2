#include "loop.h"

void waitSeconds(int seconds)
{
     // int time = ticks_elapsed() + seconds;
     // while (ticks_elapsed() < time)
     //       ;
     //me tira error pero ticks_elapsed esta en time. Hay que ver que onda eso
}

int loop(int argc, char *argv[])
{
    if (argc != 1){
     printError("Cantidad de parametros incorrecto.\n");
     return -1;
 }
     //int pid = getPID();
     while (1)
    {
        waitSeconds(15);
        //print("%d\n", pid);
    }
return 1;
}