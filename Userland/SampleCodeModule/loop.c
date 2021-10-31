#include "loop.h"
#include "tests.h"
void waitSeconds(int seconds)
{
    // int time = ticks_elapsed() + seconds;
    // while (ticks_elapsed() < time)
    //       ;
    //me tira error pero ticks_elapsed esta en time. Hay que ver que onda eso
}

void loop(int argc, char *argv[])
{
    while(1)
    {
        printf("a");
    }
    /*
    if (argc != 1)
    {
        printError("Cantidad de parametros incorrecto.\n");
        return ;
    }
    //int pid = getPID();
    while (1)
    {
        waitSeconds(15);
        //print("%d\n", pid);
    }
    return ;
    */
}