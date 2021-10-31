#include <pipeQueue.h>

typedef struct pipeQueueCDT
{
    int head, tail, size;
    char buff[PIPE_BUFFER_SIZE];
} pipeQueueCDT;

pipeQueueADT newPipeQueue(memoryManagerADT memoryManager)
{
    pipeQueueADT newPQ = allocMem(memoryManager,sizeof(struct pipeQueueCDT));
    newPQ->head = newPQ->tail = newPQ->size = 0;
    return newPQ;
}

int putS(pipeQueueADT pipeQueue, char *string)
{
    int i = 0;
    int count = strlen(string);

    while (pipeQueue->size < PIPE_BUFFER_SIZE - 1 && i < count && string[i] != 0)
    {
        pipeQueue->buff[pipeQueue->tail % PIPE_BUFFER_SIZE] = string[i++];
        pipeQueue->tail = (pipeQueue->tail + 1) % PIPE_BUFFER_SIZE;
        pipeQueue->size++;
    }

    pipeQueue->buff[pipeQueue->tail] = 0;
    pipeQueue->tail = (pipeQueue->tail + 1) % PIPE_BUFFER_SIZE;

    return 0;
}

int getS(pipeQueueADT pipeQueue, char *dest, int count)
{

    int i = 0;
    while ((pipeQueue->buff[pipeQueue->head] != 0 || i == 0) && i < count && pipeQueue->buff[pipeQueue->head] != EOF)
    {
        if (pipeQueue->size > 0 && pipeQueue->buff[pipeQueue->head] != 0)
        {
            dest[i] = pipeQueue->buff[pipeQueue->head];
            pipeQueue->head = (pipeQueue->head + 1) % PIPE_BUFFER_SIZE;
            i++;
            pipeQueue->size--;
        }
        else if (pipeQueue->size > 0 && pipeQueue->buff[pipeQueue->head] == 0)
        {
            pipeQueue->size--;
            break;
        }
    }

    if (pipeQueue->buff[pipeQueue->head] == EOF)
    {
        dest[0] = EOF;
        dest[1] = 0;
        pipeQueue->head = pipeQueue->tail = pipeQueue->size = 0;
        return EOF;
    }
    return 0;
}

int isEmpty(pipeQueueADT pipeQueue)
{
    return pipeQueue->size == 0;
}