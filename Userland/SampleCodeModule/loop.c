// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "loop.h"
#include "tests.h"

void sleep(int seconds)
{
    int totalTime = (int) _syscall(GET_SEC,0,0,0,0,0) + seconds;
    while ((int) _syscall(GET_SEC,0,0,0,0,0) <= totalTime)
        ;
}

void loop(int argc, char *argv[])
{

    if (argc != 2)
    {
        printError("Cantidad de parametros incorrecto.\n");
        return;
    }
    int sec = strToInt(argv[1]);
    if( sec < 1 || sec > 20)
    {
        printError("El tiempo debe estar entre 1 y 20\n");
        return;
    }
    int pid = (int)_syscall(PID, 0, 0, 0, 0, 0);

    char * aux = myMalloc(1000);
    while (1)
    {
        sleep(sec);
        printf("Hola, soy el proceso de PID %d\n", pid);
    }
}