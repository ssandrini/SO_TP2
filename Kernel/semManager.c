#include <semManager.h>

#define NULL 0

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

memoryManagerADT memMan;
schedulerADT schedul;
static SemaphoreList *semList;
static uint64_t idCounter;

static void addInList(Semaphore *newSem);
static Semaphore *findSem(uint64_t id);
static void removeFromList(uint64_t id);

void initSemManager(memoryManagerADT mm, schedulerADT scheduler)
{
    memMan = mm;
    schedul = scheduler;
    idCounter = 1;
    semList = allocMem(memMan, sizeof(SemaphoreList));
    semList->first = NULL;
    semList->size = 0;
}

uint64_t semCreate(uint64_t initValue)
{
    Semaphore *sem = allocMem(memMan, sizeof(Semaphore));
    if (sem == NULL)
        return NULL;

    sem->value = initValue;
    sem->blockedSize = 0;
    sem->id = idCounter++;
    sem->mutex = 0;
    sem->attachedProcesses = 1;

    addInList(sem);
    return sem->id;
}

uint64_t semOpen(uint64_t id)
{
    Semaphore *sem = findSem(id);
    // hay que chequear si el semaforo ya está lleno?
    if (sem == NULL)
        return NULL;
    sem->attachedProcesses++;
    return sem->id;
}

int semWait(uint64_t id)
{
    Semaphore *sem = findSem(id);
    if (sem == NULL)
        return -1;

    while (_xchg(&(sem->mutex), 1) != 0)
        ;

    if (sem->value <= 0 || sem->blockedSize > 0)
    {
        int currPid = getPid(schedul);
        sem->blockedPIDs[sem->blockedSize++] = currPid;
        blockProcess(schedul, currPid);
    }
    else
    {
        sem->value--;
    }

    _xchg(&(sem->mutex), 0);
    return 0;
}

int semPost(uint64_t id)
{
    Semaphore *sem = findSem(id);
    if (sem == NULL)
        return -1;

    while (_xchg(&(sem->mutex), 1) != 0)
        ;

    if (sem->blockedSize > 0)
    {
        int first = sem->blockedPIDs[0];
        int i;
        for (i = 0; i < sem->blockedSize - 1; i++)
        {
            sem->blockedPIDs[i] = sem->blockedPIDs[i + 1];
        }
        sem->blockedSize--;
        unblockProcess(first);
    }
    else
        sem->value++;

    _xchg(&(sem->mutex), 0);
    return 0;
}

int semClose(uint64_t id)
{
    Semaphore *sem = findSem(id);
    if (sem == NULL)
        return -1;

    if (sem->attachedProcesses > 0)
    {
        sem->attachedProcesses--;
        return 0;
    }

    freeMem(memMan, sem);
    return 0;
}

void semPrint()
{
    // FALTA SEM_PRINT
    return;
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
    if (semList->size == 0)
        return;

    Semaphore *current = semList->first;
    while (current->next != NULL && current->next->id != id)
        current = current->next;
    if (current->next == NULL)
        return;
    Semaphore *aux = current->next;
    current->next = current->next->next;
    freeMem(memMan, aux);
}