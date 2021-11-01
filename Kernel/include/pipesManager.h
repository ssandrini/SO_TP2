#ifndef __PIPES_MANAGER_H_
#define __PIPES_MANAGER_H_

#include <scheduler.h>
#include <semManager.h>
#define MAX_PIPES 128
#define PIPE_BUFFER_SIZE 512
typedef struct pipeCDT *pipeADT;

void initPipeManager(memoryManagerADT mm, schedulerADT sch);
int newPipe();
int pipeWrite(int pipeId, char *src);
int pipeRead(int pipeId, char *dest, int count);
void printPipes();
int closePipe(int pipeId);

#endif