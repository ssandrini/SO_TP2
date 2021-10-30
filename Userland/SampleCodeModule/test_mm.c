#include <stdlib.h>
#include <tests.h>

#define MAX_BLOCKS 128
#define MAX_MEMORY 1024 //Should be around 80% of memory managed by the MM

typedef struct MM_rq{
  void *address;
  uint32_t size;
}mm_rq;

void freeMem(void *memory)
{
    _syscall(FREE, (uint64_t)memory, 0, 0, 0, 0);
}

void test_mem(){
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;

  while (1){
    rq = 0;
    total = 0;

    // Request as many blocks as we can
    while(rq < MAX_BLOCKS && total < MAX_MEMORY){
      mm_rqs[rq].size = GetUniform(MAX_MEMORY - total - 1) + 1;
      mm_rqs[rq].address = (void *)malloc(mm_rqs[rq].size); // TODO: Port this call as required
      if (mm_rqs[rq].address == NULL)
      {
        printf("No hay memoria!\n");
        return;
      }
      total += mm_rqs[rq].size;
      rq++;
    }

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL)
        memoryset(mm_rqs[i].address, i, mm_rqs[i].size); // TODO: Port this call as required

    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL)
        if(!memcheck(mm_rqs[i].address, i, mm_rqs[i].size))
          printf("ERROR!\n"); // TODO: Port this call as required

    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL)
        freeMem(mm_rqs[i].address);  // TODO: Port this call as required
  } 
}

void test_mm(){
  printf("Test memory manager\n");
  char* argAux[] = {"testMemoryManager"};
  _syscall(NEW_PROCESS,(uint64_t) &test_mem, (uint64_t) argAux,1,0,0);
  _syscall(PS,0,0,0,0,0);
  //_syscall(KILL_PROCESS,pid,0,0,0,0);
  //_syscall(PS,0,0,0,0,0);
  return;
}