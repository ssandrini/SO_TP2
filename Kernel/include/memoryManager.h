#ifndef __MEMORYMANAGER_H_
#define __MEMORYMANAGER_H_
#include <stdlib.h>
#include <naiveConsole.h>
typedef struct memoryManagerCDT *memoryManagerADT;

memoryManagerADT newMemoryManager(void *startDir, size_t size);
void *allocMem(memoryManagerADT mm, size_t size);
int freeMem(memoryManagerADT mm, void *p);
void printStatus(memoryManagerADT mm);
#endif