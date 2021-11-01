#include <tests.h>

void lector(int argc, char ** argv)
{
    char * aux = myMalloc(10);
    _syscall(READ_PIPE,(uint64_t) strToInt(argv[1]),aux,1,0,0);
    _syscall(PRINT_PIPE, 0,0,0,0,0);
    myFree(aux);
}

void test_pipes(int argc, char ** argv)
{
    printf("Test de pipes\n");

    uint64_t pipeId = _syscall(CREATE_PIPE, 0, 0, 0, 0, 0);
    uint64_t newfd = _syscall(CREATE_FD, pipeId, 0, 0, 0, 0);
    int rightFd[2];
    rightFd[0] = (int)newfd;
    rightFd[1] = 1;
    char * aux = myMalloc(10);
    uintToString(pipeId,aux,10);
    char *newArgv[1] = {"Lector", aux};
    int newArgc = 2;

    _syscall(NEW_PROCESS, (uint64_t) &(lector), (uint64_t) newArgv, 1,0, (uint64_t)rightFd);
    sleep(1);
    _syscall(PRINT_PIPE, 0,0,0,0,0);
    sleep(2);
    _syscall(WRTIE_PIPE, pipeId, (uint64_t) "a",0,0,0);
    sleep(1);
    myFree(aux);
    _syscall(CLOSE_FD,newfd,0,0,0,0);
}