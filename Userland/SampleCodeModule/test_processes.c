#include <tests.h>
//TO BE INCLUDED
void endless_loop(int argc, char ** argv)
{
  while (1)
  {
    printf("a");
    _hltuser();
  }
}

void endless_loop2(int argc, char ** argv)
{
  while (1)
  {
    printf("b");
    _hltuser();
  }
}

#define MAX_PROCESSES 10 //Should be around 80% of the the processes handled by the kernel

enum State
{
  ERROR,
  RUNNING,
  BLOCKED,
  KILLED
};

typedef struct P_rq
{
  uint32_t pid;
  enum State state;
} p_rq;

int test_processes()
{
  
  printf("Test processes\n");
  p_rq p_rqs[MAX_PROCESSES];
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;

  // Create MAX_PROCESSES processes
  for (rq = 0; rq < MAX_PROCESSES; rq++)
  {
    char* argAux[] = {"endless_loop"};
    p_rqs[rq].pid = _syscall(NEW_PROCESS,&endless_loop, argAux,1,0,0); // TODO: Port this call as required
   
    if (p_rqs[rq].pid == -1)
    {                                     // TODO: Port this as required
      printf("Error creating process\n"); // TODO: Port this as required
      return;
    }
    else
    {
      p_rqs[rq].state = RUNNING;
      alive++;
    }
  }

  _syscall(PS,0,0,0,0,0);

  
  // Randomly kills, blocks or unblocks processes until every one has been killed
  while (alive > 0)
  {

    for (rq = 0; rq < MAX_PROCESSES; rq++)
    {
      action = GetUniform(2) % 2;

      switch (action)
      {
      case 0:
        if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED)
        {
          if (_syscall(KILL_PROCESS,p_rqs[rq].pid,0,0,0,0) == -1)
          {                                    // TODO: Port this as required
            printf("Error killing process\n"); // TODO: Port this as required
            return;
          }
          p_rqs[rq].state = KILLED;
          alive--;
        }
        break;

      case 1:
        if (p_rqs[rq].state == RUNNING)
        {
          if (_syscall(BLOCK_PROCESS,p_rqs[rq].pid,0,0,0,0) == -1)
          {                                     // TODO: Port this as required
            printf("Error blocking process\n"); // TODO: Port this as required
            return;
          }
          p_rqs[rq].state = BLOCKED;
        }
        break;
      }
    }

    // Randomly unblocks processes
    for (rq = 0; rq < MAX_PROCESSES; rq++)
      if (p_rqs[rq].state == BLOCKED && GetUniform(2) % 2)
      {
        if (_syscall(UNBLOCK_PROCESS,p_rqs[rq].pid,0,0,0,0) == -1)
        {                                       // TODO: Port this as required
          printf("Error unblocking process\n"); // TODO: Port this as required
          return;
        }
        p_rqs[rq].state = RUNNING;
      }
      
  }
  _syscall(PS,0,0,0,0,0);
  
  printf("termino\n");
  
  return 0;
}

int test_kill()
{
  printf("Test kill\n");
  char* argAux[] = {"mytestkill"};
  _syscall(NEW_PROCESS,(uint64_t) &endless_loop, (uint64_t) argAux,1,0,0);
  _syscall(NEW_PROCESS,(uint64_t) &endless_loop2, (uint64_t) argAux,1,0,0);
  _syscall(PS,0,0,0,0,0);
  //_syscall(KILL_PROCESS,pid,0,0,0,0);
  //_syscall(PS,0,0,0,0,0);
  return 0;
}