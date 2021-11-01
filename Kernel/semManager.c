#include <semManager.h>
#include <naiveConsole.h>
typedef struct Semaphore
{
    uint64_t id;
    uint64_t value;
    uint64_t blockedPIDs[MAX_BLOCKED_PIDS];
    uint64_t blockedSize;
    uint64_t attachedProcesses;
    int mutex;
    struct Semaphore *next;
} Semaphore;

typedef struct SemaphoreList
{
    Semaphore *first;
    Semaphore *last;
    uint64_t size;
} SemaphoreList;

static memoryManagerADT memMan;
static schedulerADT scheduler;
static SemaphoreList *semList;
static uint64_t idCounter;
static char *aux10;
static void addInList(Semaphore *newSem);
static Semaphore *findSem(uint64_t id);
static void removeFromList(uint64_t id);

void initSemManager(memoryManagerADT mm, schedulerADT sch)
{
    memMan = mm;
    scheduler = sch;
    idCounter = 1;
    semList = allocMem(memMan, sizeof(SemaphoreList));
    semList->first = semList->last = NULL;
    semList->size = 0;
    aux10 = allocMem(memMan, 100);
}

uint64_t semCreate(uint64_t initValue)
{
    Semaphore *sem = allocMem(memMan, sizeof(Semaphore));
    if (sem == NULL)
    {
        return (uint64_t)NULL;
    }

    sem->value = initValue;
    sem->blockedSize = 0;
    sem->id = idCounter++;
    sem->mutex = 0;
    sem->attachedProcesses = 0;
    sem->next = NULL;

    addInList(sem);
    return sem->id;
}

uint64_t semOpen(uint64_t id)
{
    Semaphore *sem = findSem(id);
    if (sem == NULL)
    {
        return (uint64_t)NULL;
    }
    sem->attachedProcesses++;
    return sem->id;
}

int semWait(uint64_t id)
{
    Semaphore *sem = findSem(id);
    if (sem == NULL)
    {
        return -1;
    }

    while (_xchg(&(sem->mutex), 1) != 0)
        ;

    if (sem->value > 0)
    {
        sem->value--;
        _xchg(&(sem->mutex), 0);
        return 0;
    }
    else
    {
        int currPid = getPid(scheduler);
        sem->blockedPIDs[sem->blockedSize++] = currPid;
        _xchg(&(sem->mutex), 0);
        blockProcess(scheduler, currPid);
        return 0;
    }
}

int semPost(uint64_t id)
{
    Semaphore *sem = findSem(id);
    if (sem == NULL)
    {
        return -1;
    }

    while (_xchg(&(sem->mutex), 1) != 0)
    {
        ;
    }

    if (sem->blockedSize > 0)
    {
        int first = sem->blockedPIDs[0];
        int i;
        for (i = 0; i < sem->blockedSize - 1; i++)
        {
            sem->blockedPIDs[i] = sem->blockedPIDs[i + 1];
        }
        sem->blockedSize--;
        unblockProcess(scheduler, first);
    }
    else
    {
        sem->value++;
    }
    _xchg(&(sem->mutex), 0);
    return 0;
}

int semClose(uint64_t id)
{
    Semaphore *sem = findSem(id);
    if (sem == NULL)
    {

        return -1;
    }

    if (sem->attachedProcesses > 0)
    {
        sem->attachedProcesses--;
        if (sem->attachedProcesses == 0)
        {
            removeFromList(id);
        }
        return 0;
    }
    removeFromList(id);
    return 0;
}

void printBlockedPids(uint64_t semId)
{
    Semaphore *current = findSem(semId);
    if (current->blockedSize == 0)
        ncPrint(" - ", 15);
    else
    {
        for (int i = 0; i < current->blockedSize; i++)
        {
            uintToBase(current->blockedPIDs[i], aux10, 10);
            ncPrint(aux10, 15);
            if (i + 1 != current->blockedSize)
                ncPrint(", ", 15);
        }
    }
}

void semPrint()
{
    char *message = "SEM_ID  VALOR  BLOCKED_PIDS";
    ncPrint(message, 12);
    ncNewline();

    Semaphore *current = semList->first;
    while (current != NULL)
    {
        int space;
        uintToBase(current->id, aux10, 10);
        space = current->id >= 10 ? 2 : 1;
        space = current->id >= 100 ? 3 : space;
        ncPrint(aux10, COLOR);
        while (space < 8)
        {
            ncPrint(" ", COLOR);
            space++;
        }

        uintToBase(current->value, aux10, 10);
        space = current->id >= 10 ? 2 : 1;
        space = current->id >= 100 ? 3 : space;
        ncPrint(aux10, COLOR);
        while (space < 6)
        {
            ncPrint(" ", COLOR);
            space++;
        }
        printBlockedPids(current->id);
        ncNewline();
        current = current->next;
    }
}
/////////////////// static functions /////////////////////////

static Semaphore *findSem(uint64_t id)
{
    Semaphore *current = semList->first;
    while (current != NULL && current->id != id)
        current = current->next;
    return current;
}

static void addInList(Semaphore *newSem)
{
    if (semList->size == 0)
    {
        semList->first = semList->last = newSem;
    }
    else
    {
        semList->last->next = newSem;
        semList->last = newSem;
    }
    semList->size++;
}

static void removeFromList(uint64_t id)
{
    Semaphore *current = semList->first;
    Semaphore *prev = NULL;
    while (current != NULL && current->id != id)
    {
        prev = current;
        current = current->next;
    }
    if (current == NULL)
        return;
    if (prev == NULL) // era el first
    {
        semList->first = current->next;
    }
    else
    {
        prev->next = current->next;
        if (prev->next == NULL)
            semList->last = prev;
    }
    semList->size--;
    freeMem(memMan, current);
}
