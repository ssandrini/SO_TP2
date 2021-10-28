#ifndef __FD_MANAGER_H_
#define __FD_MANAGER_H_

#include <memoryManager.h>
#include <scheduler.h>
#include <keyboard.h>
#define MAX_FD 128
#define STDIN 0
#define STDOUT 1
#define EOF {-1,0}

typedef struct fdCDT * fdADT;
int initFdManager(memoryManagerADT mm, schedulerADT sch);
int newFd(int pipeId);
int fdRead(int fd, char * dest, int count);
int fdWrite(int fd, char * src, int color);
void closeFd(int fd);
int freeFd(int fd);
#endif
