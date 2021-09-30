#ifndef __SCHEDULER_H_
#define __SCHEDULER_H_
#include <stdint.h>
#include <memoryManager.h>

typedef struct schedulerCDT *schedulerADT;

schedulerADT newScheduler(memoryManagerADT memoryManager);

#endif