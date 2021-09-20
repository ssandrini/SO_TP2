#ifndef __MEMORYMANAGER_H_
#define __MEMORYMANAGER_H_
#include <stdlib.h>

#define INITIAL_POS 0x0000000000110000
#define FINAL_POS 0x00000000003FFFFFF

void * alloc(size_t size);
void initMemoryManager();
#endif