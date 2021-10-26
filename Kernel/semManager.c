#include <semManager.h>

#define NULL 0

memoryManagerADT memMan;
typedef struct Semaphore
{
    uint64_t id;
    uint64_t value;
    uint64_t blockedPIDs[MAX_BLOCKED_PIDS];
    uint64_t blockedSize;
    int mutex;
} Semaphore;

static Semaphore * semList[MAX_SEMAPHORES];

void initSemManager(memoryManagerADT mm)
{
    for (int i = 0; i < MAX_SEMAPHORES; i++)
    {
        semList[i] = NULL;
    }
    memMan = mm;
}
// static void dumpBlockedPIDs(uint64_t *blockedPIDs, uint64_t blockedPIDsSize);

static Semaphore * findSem(uint64_t id)
{
    int i;
    for(i = 0; i < MAX_SEMAPHORES; i++)
    {
        if(semList[i]->id == id)
        {
            return semList[i];
        }
    }
    return NULL;
}

uint64_t semOpen(uint64_t id, uint64_t initValue)
{
    Semaphore * sem = findSem(id);
    if (sem == NULL)
    {
        sem = allocMem(memMan, sizeof(Semaphore));
        if (sem == NULL)
            return -1;

        sem->value = initValue;
        sem->blockedSize = 0;
        sem->id = id;
        sem->mutex = 0;
    }
    // habria que chequear blockedPIDSize si no es null?
    return id;
}
