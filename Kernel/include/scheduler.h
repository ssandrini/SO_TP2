#ifndef __SCHEDULER_H_
#define __SCHEDULER_H_
#include <stdint.h>
#include <memoryManager.h>
#include <lib.h>
#include <interrupts.h>

#define STACK_SIZE 4096
#define COLOR 15    

typedef enum
{
    READY,
    BLOCKED,
    KILLED
} State;

typedef struct schedulerCDT *schedulerADT;

schedulerADT newScheduler(memoryManagerADT memoryManager);

int getCurrentFdRead(schedulerADT scheduler);

int getCurrentFdWrite(schedulerADT scheduler);

int newProcess(schedulerADT scheduler, unsigned int priority, void (*entryPoint)(int, char **), char **argv, int argc, int fg, int fd[2]);

int getPid(schedulerADT scheduler);

int killProcess(schedulerADT scheduler, int pid);

int killFgProcess(schedulerADT scheduler);

int setPriority(schedulerADT scheduler, int pid, int newPriority);

int blockProcess(schedulerADT scheduler, int pid);
int unblockProcess(schedulerADT scheduler, int pid);

void * nextProcess(schedulerADT scheduler, void * currentRsp);

void printProcesses(schedulerADT scheduler);

void yield(schedulerADT scheduler);

int isBlocked(schedulerADT scheduler, int pid);

int wait(schedulerADT scheduler, int pid);

void saveDir(schedulerADT scheduler, int pid, void * toSave);
#endif