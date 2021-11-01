// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <tests.h>
#define NULL (void *)0
#define MAX_BLOCKS 12
#define MAX_MEMORY 1024*1024 //Should be around 80% of memory managed by the MM

typedef struct MM_rq
{
  void *address;
  uint32_t size;
} mm_rq;

void test_mem(int argc, char **argv)
{
  mm_rq mm_rqs[MAX_BLOCKS];

  printf("Comienza el test de memoria:\n");
  while(1)
  {
    uint8_t rq = 0;
    uint32_t total = 0;
  
    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < MAX_MEMORY)
    {
      mm_rqs[rq].size = GetUniform(MAX_MEMORY - total - 1) + 1;

      mm_rqs[rq].address = myMalloc(mm_rqs[rq].size);
      if (mm_rqs[rq].address == NULL)
      {
        printf("No memory!\n");
        return;
      }
      total += mm_rqs[rq].size;
      rq++;
    }

    
    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL)
        memoryset(mm_rqs[i].address, i, mm_rqs[i].size);

    
    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size))
          printf("error");
    
    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL)
        myFree(mm_rqs[i].address);
  }
  
}