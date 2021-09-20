#include <memoryManager.h>

char * current;

void * alloc(size_t size)
{
    if(current + size < (char *) FINAL_POS)
    {
        void * ans = (void *) current;
        current+= size;
        return ans;
    }
    else
        return NULL;
}

void initMemoryManager()
{
    current = (char *) INITIAL_POS;
}