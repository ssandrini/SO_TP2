#include <pipesManager.h>

static memoryManagerADT memoryManager;
static schedulerADT scheduler;

typedef struct pipeCDT
{
    int pipeId;
    int beingAccessed;

    char buffer[PIPE_BUFFER_SIZE];
    int readIndex;
    int writeIndex;
    uint64_t writeSemId;
    uint64_t readSemId;
} pipeCDT;

static pipeADT pipeList[MAX_PIPES] = {NULL};
static memoryManagerADT memoryManager;
static schedulerADT scheduler;
static int pipePutChar(pipeADT pipe, char c);
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
    newPipe->pipeId = i;
    newPipe->beingAccessed = 1;
    newPipe->readIndex = newPipe->writeIndex = 0;
    newPipe->writeSemId = semCreate(PIPE_BUFFER_SIZE);
    newPipe->readSemId = semCreate(0);

    pipeList[i] = newPipe;
    return newPipe->pipeId;
}

int pipeWrite(int pipeId, char *src)
{

    if (pipeId < 0 || pipeId > MAX_PIPES || pipeList[pipeId] == NULL)
        return -1;
    pipeADT pipe = pipeList[pipeId];

    while (*src != 0)
        pipePutChar(pipe, *src++);
    
    return 0;
}

static int pipePutChar(pipeADT pipe, char c)
{

    semWait(pipe->writeSemId);

    pipe->buffer[pipe->writeIndex] = c;
    pipe->writeIndex = (pipe->writeIndex + 1) % PIPE_BUFFER_SIZE;

    semPost(pipe->readSemId);

    return 0;
}

int pipeRead(int pipeId, char *dest, int count)
{
    if (pipeId < 0 || pipeId > MAX_PIPES || pipeList[pipeId] == NULL)
        return -1;
    pipeADT pipe = pipeList[pipeId];

    for(int i = 0; i < count; i++)
    {
        semWait(pipe->readSemId);
        dest[i] = pipe->buffer[pipe->readIndex];
        pipe->readIndex = (pipe->readIndex + 1) % PIPE_BUFFER_SIZE;
        semPost(pipe->writeSemId);
    }
    return count;
}

int closePipe(int pipeId)
{
    if (pipeId < 0 || pipeId > MAX_PIPES || pipeList[pipeId] == NULL)
    {
        return -1;
    }
    pipeADT pipe = pipeList[pipeId];

    semClose(pipe->writeSemId);
    semClose(pipe->readSemId);

    freeMem(memoryManager, pipe);
    pipeList[pipeId] = NULL;
    return 0;
}

void printPipes()
{
    char *message = "PIPE_ID  ESTADO    PROCESOS BLOQUEADOS:";
    ncPrint(message, 12);
    ncNewline();

    char *aux = allocMem(memoryManager, 10);
    int space = 0;

    for (int i = 0; i < MAX_PIPES; i++)
    {
        if (pipeList[i] != NULL)
        {
            uintToBase((uint64_t)pipeList[i]->pipeId, aux, 10);
            space = pipeList[i]->pipeId >= 10 ? 2 : 1;
            space = pipeList[i]->pipeId >= 100 ? 3 : space;
            ncPrint(aux, COLOR);
            while (space < 9)
            {
                ncPrint(" ", COLOR);
                space++;
            }

            if (pipeList[i]->beingAccessed)
            {
                ncPrint("En uso", 15);
                ncPrint("     ", 15);
            }   
            else
            {
                ncPrint("Libre", 15);
                ncPrint("      ", 15);
            }
            
            printBlockedPids(pipeList[i]->readSemId);
        }
    }
    ncNewline();
    freeMem(memoryManager, aux);
}