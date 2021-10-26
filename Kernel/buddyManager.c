#ifdef BUDDY
#include <memoryManager.h>
/*
    Idem los comentarios del otro MM, igual ahora estan
    hardcodeados los dos exactamente iguales.
*/
#define STRUCT_POS 0x0000000000600000
#define INITIAL_POS 0x0000000000700000
#define FINAL_POS 0x000000000FFFFFFFF

typedef struct memoryManagerCDT
{
    char *current;
} memoryManagerCDT;

void *allocMem(memoryManagerADT mm, size_t size)
{
    if (mm->current + size < (char *)FINAL_POS)
    {
        void *ans = (void *)mm->current;
        mm->current += size;
        return ans;
    }
    else
        return NULL;
}

memoryManagerADT newMemoryManager()
{
    memoryManagerADT mm = (void *) STRUCT_POS;
    mm->current = (char *)INITIAL_POS;
    return mm;
}

void freeMem(memoryManagerADT mm, void *p)
{
    return;
}

#endif