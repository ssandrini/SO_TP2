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

void test_processes(int argc, char ** argv)
{
  
  printf("Test processes\n");
  p_rq p_rqs[MAX_PROCESSES];
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  int fd[2];
  fd[0]=0;
  fd[1]=1;

  // Create MAX_PROCESSES processes
  for (rq = 0; rq < MAX_PROCESSES; rq++)
  {
    char* argAux[] = {"endless_loop"};
    p_rqs[rq].pid = _syscall(NEW_PROCESS,(uint64_t) &endless_loop, (uint64_t) argAux,1,0,fd); // TODO: Port this call as required
   
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
          if (_syscall(KILL_PROCESS, (uint64_t) p_rqs[rq].pid,0,0,0,0) == -1)
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
          if (_syscall(BLOCK_PROCESS, (uint64_t) p_rqs[rq].pid,0,0,0,0) == -1)
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
        if (_syscall(UNBLOCK_PROCESS,(uint64_t) p_rqs[rq].pid,0,0,0,0) == -1)
        {                                       // TODO: Port this as required
          printf("Error unblocking process\n"); // TODO: Port this as required
          return;
        }
        p_rqs[rq].state = RUNNING;
      }
      
  }
  _syscall(PS,0,0,0,0,0);
  
  printf("termino\n");
  return ;
}
