#include <scheduler.h>

typedef enum
{
      READY,
      BLOCKED
} State;

typedef struct
{
      uint64_t pid;
      uint64_t ppid;
      unsigned int priority;

      char *name;
      int argc;
      char **argv;

      void *rsp; // aca tendria que usar el stackFrame
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
      PNode * current;
}PList;

typedef struct schedulerCDT
{
    memoryManagerADT memoryManager;
    PList * processesList;
    int pidCounter;
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

schedulerADT newScheduler(memoryManagerADT memoryManager) 
{
    schedulerADT scheduler = allocMem(memoryManager, sizeof(schedulerCDT));
    scheduler->processesList = allocMem(memoryManager, sizeof(PList));
    scheduler->pidCounter = 0;
    return scheduler;
}

int newProcess(schedulerADT scheduler, char * processName, unsigned int priority, uint64_t codeAddress) 
{
      // dejo este comentario:
      /*
            Creo que en el kernel.c apenas comienza y crea el mm y el scheduler
            para iniciar todo deberia forzar una int20 entonces el scheduler va 
            a tomar el primer proceso que creería que debería ser la shell

      */
    PCB * aux = (PCB *) allocMem(scheduler->memoryManager, sizeof(PCB));
    aux->pid = scheduler->pidCounter++;
    aux->ppid = scheduler->processesList->current->pcb.pid; 
    aux->priority = priority;
    aux->state = READY;
    // strcpy(aux->name, processName); necesito hacerme strcpy
    aux->rbp = allocMem(scheduler->memoryManager, STACK_SIZE);
    //aux->rsp = newStackFrame();

    // aca me faltaria agregarlo a la lista y seguir 

}

