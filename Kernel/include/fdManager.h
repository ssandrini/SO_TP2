#ifndef __FD_MANAGER_H_
#define __FD_MANAGER_H_

#include <memoryManager.h>
#include <scheduler.h>
#include <pipesManager.h>
#include <keyboard.h>
#include <naiveConsole.h>

#define MAX_FD 128
#define STDIN 0
#define STDOUT 1
#define NO_PIPE -1


typedef struct fdCDT * fdADT;
int initFdManager(memoryManagerADT mm, schedulerADT sch);
int newFd(int pipeId);
int fdRead(int fd, char * dest, int count);
int fdWrite(int fd, char * src);
void closeFd(int fd);
int freeFd(int fd);

#endif
