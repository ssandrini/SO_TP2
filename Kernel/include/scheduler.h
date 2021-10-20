#ifndef __SCHEDULER_H_
#define __SCHEDULER_H_
#include <stdint.h>
#include <memoryManager.h>
#include <lib.h>
#include <interrupts.h>

#define STACK_SIZE 4096

typedef enum
{
    READY,
    BLOCKED,
    KILLED
} State;

typedef struct schedulerCDT *schedulerADT;

schedulerADT newScheduler(memoryManagerADT memoryManager);

int newProcess(schedulerADT scheduler, char * processName, unsigned int priority, void (*entryPoint)(int, char **), char **argv, int argc);

int getPid(schedulerADT scheduler);

int killProcess(schedulerADT scheduler, int pid);

int setPriority(schedulerADT scheduler, int pid, int newPriority);

int setState(schedulerADT scheduler, int pid, State newState);

void * nextProcess(schedulerADT scheduler, void * currentRsp);
// Me falta la de listar todos los procesos todavia

#endif