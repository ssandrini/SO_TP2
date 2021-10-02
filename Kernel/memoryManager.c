#ifdef NO_BUDDY

#include <memoryManager.h>
/*
    Dejo estos comentarios para que después revisemos bien las direcciones
    de los defines. Además tengo dudas si está bien lo de STRUCT POS
    y me gustaría ver si se puede armar algo tipo patron singleton.
*/
#define STRUCT_POS 0x0000000000600000
#define INITIAL_POS 0x0000000000700000
#define FINAL_POS 0x000000000FFFFFFFF

typedef struct memoryManagerCDT
{
    char *current;
}memoryManagerCDT;

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

#endif