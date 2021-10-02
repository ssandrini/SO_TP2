#include <scheduler.h>

/*
    ojo que al crear el MM se crean en una posicion fija
    entonces al crear otro se van a pisar
*/
typedef enum
{
      READY,
      BLOCKED
} State;

typedef struct
{
      uint64_t pid;
      uint64_t ppid;

      char *name;
      int argc;
      char **argv;

      void *rsp;
      void *rbp;

      State state; 
}PCB;

typedef struct PNode
{
      PCB pcb;
      struct PNode *next;
}PNode;

typedef struct PList
{
      uint64_t size;
      PNode *first;
}PList;

typedef struct schedulerCDT
{
    memoryManagerADT memoryManager;
    PList * processesList;
} schedulerCDT;

typedef struct
{
      uint64_t gs;
      uint64_t fs;
      uint64_t r15;
      uint64_t r14;
      uint64_t r13;
      uint64_t r12;
      uint64_t r11;
      uint64_t r10;
      uint64_t r9;
      uint64_t r8;
      uint64_t rsi;
      uint64_t rdi;
      uint64_t rbp;
      uint64_t rdx;
      uint64_t rcx;
      uint64_t rbx;
      uint64_t rax;
      uint64_t rip;
      uint64_t cs;
      uint64_t eflags;
      uint64_t rsp;
      uint64_t ss;
      uint64_t base;
} StackFrame;

schedulerADT newScheduler(memoryManagerADT memoryManager) {
    schedulerADT scheduler = allocMem(memoryManager, sizeof(schedulerCDT));
    scheduler->processesList = allocMem(memoryManager, sizeof(PList));
    return scheduler;
}


