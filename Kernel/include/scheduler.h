#ifndef __SCHEDULER_H_
#define __SCHEDULER_H_
#include <stdint.h>
#include <memoryManager.h>

typedef struct schedulerCDT *schedulerADT;

schedulerADT newScheduler(memoryManagerADT memoryManager);

int newProcess(schedulerADT scheduler, char * processName, unsigned int priority, uint64_t codeAddress);

int getPid(schedulerADT scheduler);

int killProcess(schedulerADT scheduler, int pid);

int setPriority(schedulerADT scheduler, int pid, int newPriority);

int setState(schedulerADT scheduler, int pid, State newState);

// Me falta la de listar todos los procesos todavia

#endif