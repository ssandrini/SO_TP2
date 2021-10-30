#include <pipesManager.h>

static memoryManagerADT memoryManager;
static schedulerADT scheduler;

typedef struct pipeCDT
{
    int pipeId;
    int beingAccessed;
    int waitingProcess;
    pipeQueueADT pipeQueue;
} pipeCDT;

static pipeADT pipeList[MAX_PIPES] = {NULL};
static memoryManagerADT memoryManager;
static schedulerADT scheduler;

void initPipeManager(memoryManagerADT mm, schedulerADT sch)
{
    memoryManager = mm;
    scheduler = sch;
}

int newPipe()
{
    int i;
    while (i < MAX_PIPES && pipeList[i] != NULL)
        i++;

    pipeADT newPipe = allocMem(memoryManager, sizeof(pipeCDT));
    newPipe->beingAccessed = newPipe->waitingProcess = 0;
    newPipe->pipeQueue = newPipeQueue(memoryManager);
    newPipe->pipeId = i;
    pipeList[i] = newPipe;

    return newPipe->pipeId;
}

int pipeWrite(int pipeId, char *src)
{
    if (pipeId < 0 || pipeId > MAX_PIPES || pipeList[pipeId] == NULL)
        return -1;

    pipeADT pipe = pipeList[pipeId];

    if (pipe->beingAccessed)
    {
        int pid = getPid(scheduler);
        pipe->waitingProcess = pid;
        blockProcess(scheduler, pid);

        pipe->waitingProcess = 0;
    }
    pipe->beingAccessed = 1;

    putS(pipe->pipeQueue, src);

    pipe->beingAccessed = 0;

    if (pipe->waitingProcess)
    {
        unblockProcess(scheduler, pipe->waitingProcess);
    }

    return 0;
}

int pipeRead(int pipeId, char *dest, int count)
{
    if (pipeId < 0 || pipeId > MAX_PIPES || pipeList[pipeId] == NULL)
        return -1;
    pipeADT pipe = pipeList[pipeId];

    if (pipe->beingAccessed || isEmpty(pipe->pipeQueue))
    {
        int pid = getPid(scheduler);
        pipe->waitingProcess = pid;
        blockProcess(scheduler, pid);

        pipe->waitingProcess = 0;
    }

    pipe->beingAccessed = 1;

    getS(pipe->pipeQueue, dest, count);
    pipe->beingAccessed = 0;

    if (pipe->waitingProcess)
    {
        unblockProcess(scheduler, pipe->waitingProcess);
    }

    return 0;
}

int closePipe(int pipeId)
{
    if (pipeId < 0 || pipeId > MAX_PIPES || pipeList[pipeId] == NULL)
    {
        return -1;
    }
    pipeADT pipe = pipeList[pipeId];
    char eof[2] = {-1,0};
    return putS(pipe->pipeQueue, eof);
}

int freePipe(int pipeId)
{
    if (pipeId < 0 || pipeId > MAX_PIPES || pipeList[pipeId] == NULL)
        return -1;

    pipeADT pipe = pipeList[pipeId];
    freeMem(memoryManager, pipe->pipeQueue);
    freeMem(memoryManager, pipe);
    pipeList[pipeId] = NULL;

    return 0;
}

void printPipes()
{
    char *message = "PIPE_ID    ESTADO:     PID DEL PROCESO EN ESPERA:    ";
    ncPrint(message, 12);
    ncNewline();

    char * aux = allocMem(memoryManager, 10);
    
    for(int i = 0; i < MAX_PIPES; i++)
    {
        if(pipeList[i] != NULL)
        {
            uintToBase((uint64_t) pipeList[i]->pipeId, aux, 10);
            ncPrint(aux,15);

            ncPrint("   ",15);
            if(pipeList[i]->beingAccessed)
                ncPrint("En uso",15);
            else
                ncPrint("Libre",15);

            ncPrint("   ",15);

            uintToBase((uint64_t) pipeList[i]->waitingProcess, aux, 10);
            ncPrint(aux,15);
        }
    }
    ncNewline();
    freeMem(memoryManager,aux);
    
}