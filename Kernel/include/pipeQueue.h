#ifndef PIPE_QUEUE_H
#define PIPE_QUEUE_H
#include <lib.h>
#include <memoryManager.h>

#define PIPE_BUFFER_SIZE 1024
#define EOF -1

typedef struct pipeQueueCDT * pipeQueueADT;

pipeQueueADT newPipeQueue(memoryManagerADT memoryManager);

int isEmpty(pipeQueueADT pipeQueue);

int putS(pipeQueueADT pipeQueue, char * string);

int getS(pipeQueueADT pipeQueue, char * dest, int count);


#endif