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
static int pipeIdCounter;

void initPipeManager(memoryManagerADT mm, schedulerADT sch)
{
    memoryManager = mm;
    scheduler = sch;
    pipeIdCounter = 0;
}

int newPipe()
{
    int i;
    while (i < MAX_PIPES && pipeList[i] != NULL)
        i++;

    pipeADT newPipe = allocMem(memoryManager, sizeof(pipeCDT));
    newPipe->beingAccessed = newPipe->waitingProcess = 0;
    newPipe->pipeQueue = newPipeQueue(memoryManager);
    newPipe->pipeId = pipeIdCounter++;
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
        blockProcess(scheduler,pid);

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
        return -1;
    pipeADT pipe = pipeList[pipeId];
    return putS(pipe->pipeQueue, EOF);
}
