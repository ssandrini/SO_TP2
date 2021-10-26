#ifndef _SEMAPHORES_H
#define _SEMAPHORES_H

#include <memoryManager.h>
#include <lib.h>
#include <scheduler.h>
#include <stdint.h>

#define MAX_BLOCKED_PIDS 32
#define MAX_SEMAPHORES 64

void initSemManager(memoryManagerADT mm);
uint64_t semOpen(uint64_t id, uint64_t initValue);
int semWait(uint64_t id);
int semPost(uint64_t id);
int semClose(uint64_t id);
void semPrint();

#endif