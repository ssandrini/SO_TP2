#include <tests.h>
#include <commands.h>
#define MINOR_WAIT 1000000 // TODO: To prevent a process from flooding the screen
#define WAIT 10000000      // TODO: Long enough to see theese processes beeing run at least twice

uint64_t my_getpid()
{
  return _syscall(PID,0, 0,0,0,0);
}

uint64_t my_create_process(char *name)
{
  int argc = 0;
  char *argv[1] = {0};
  argc = prepareArgs(' ', argv, name);
  int fd[2];
  fd[0] = 0;
  fd[1] = 1;
  return _syscall(NEW_PROCESS, (uint64_t) &endless_loop3, (uint64_t) argv, argc, 0, fd);
}

uint64_t my_nice(uint64_t pid, uint64_t newPrio)
{
  return _syscall(NICE, pid, newPrio,0,0,0);
}

uint64_t my_kill(uint64_t pid)
{
  return _syscall(KILL_PROCESS, pid, 0, 0, 0, 0);
}

uint64_t my_block(uint64_t pid)
{
  return  _syscall(BLOCK_PROCESS,pid, 0, 0, 0, 0);
}

uint64_t my_unblock(uint64_t pid)
{
  return _syscall(UNBLOCK_PROCESS,pid,0,0,0,0);
}

void bussy_wait(uint64_t n)
{
  uint64_t i;
  for (i = 0; i < n; i++)
    ;
}

void endless_loop3()
{
  uint64_t pid = my_getpid();

  while (1)
  {
    printf("%d ", pid);
    bussy_wait(MINOR_WAIT);
  }
}

#define TOTAL_PROCESSES 3

void test_prio(int argc, char **argv)
{
  uint64_t pids[TOTAL_PROCESSES];
  uint64_t i;

  for (i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = my_create_process("endless_loop");

  bussy_wait(WAIT);
  printf("\nCHANGING PRIORITIES...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
  {
    switch (i % 3)
    {
    case 0:
      my_nice(pids[i], 0); // lowest priority
      break;
    case 1:
      my_nice(pids[i], 1); // medium priority
      break;
    case 2:
      my_nice(pids[i], 2); // highest priority
      break;
    }
  }

  bussy_wait(WAIT);
  printf("\nBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    my_block(pids[i]);

  printf("CHANGING PRIORITIES WHILE BLOCKED...\n");
  for (i = 0; i < TOTAL_PROCESSES; i++)
  {
    switch (i % 3)
    {
    case 0:
      my_nice(pids[i], 1); // medium priority
      break;
    case 1:
      my_nice(pids[i], 1); // medium priority
      break;
    case 2:
      my_nice(pids[i], 1); // medium priority
      break;
    }
  }

  printf("UNBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    my_unblock(pids[i]);

  bussy_wait(WAIT);
  printf("\nKILLING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    my_kill(pids[i]);
}