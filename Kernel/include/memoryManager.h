#ifndef __MEMORYMANAGER_H_
#define __MEMORYMANAGER_H_
#include <stdlib.h>

typedef struct memoryManagerCDT *memoryManagerADT;
memoryManagerADT newMemoryManager();
void *allocMem(memoryManagerADT mm, size_t size);
int freeMem (memoryManagerADT mm, void * p);

#endif