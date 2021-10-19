#include <scheduler.h>


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
} PCB;

typedef struct PNode
{
      PCB * pcb;
      struct PNode *next;
} PNode;

typedef struct PList
{
      uint64_t size;
      uint64_t qReady;
      PNode *first;
      PNode *last;
} PList;

typedef struct schedulerCDT
{
      memoryManagerADT memoryManager;
      PList *processesList;
      int pidCounter;
      PNode *currentProcess;
      int life;
      PCB *idle;
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

static int argsCopy(memoryManagerADT mm, char **buffer, char **argv, int argc)
{
      for (int i = 0; i < argc; i++)
      {
            buffer[i] = allocMem(mm, sizeof(char) * (strlen(argv[i]) + 1));
            strcpy(argv[i], buffer[i]);
      }
      return 1;
}

static void setNewSF(void (*entryPoint)(int, char **), int argc, char **argv, void *rbp)
{
      StackFrame *frame = (StackFrame *)rbp - 1;
      frame->gs = 0;
      frame->fs = 0;
      frame->r15 = 0;
      frame->r14 = 0;
      frame->r13 = 0;
      frame->r12 = 0;
      frame->r11 = 0;
      frame->r10 = 0;
      frame->r9 = 0;
      frame->r8 = 0;
      frame->rsi = (uint64_t)argc;
      frame->rdi = (uint64_t)entryPoint;
      frame->rbp = 0;
      frame->rdx = (uint64_t)argv;
      frame->rcx = 0;
      frame->rbx = 0;
      frame->rax = 0;
      frame->rip = (uint64_t)wrapper;
      frame->cs = 0x008;
      frame->eflags = 0x202;
      frame->rsp = (uint64_t)(&frame->base);
      frame->ss = 0x000;
      frame->base = 0x000;
}

static void exit()
{
      // aca iria un killprocess del current
      _int20();
}

static void wrapper(void (*entryPoint)(int, char **), int argc, char **argv)
{
      entryPoint(argc, argv);
      exit();
}

static void halt(int argc, char **argv)
{
      while (1)
      {
            _hlt();
      }
}


// ------------------FUNCIONES DEFINIDAS EN EL TAD------------------------------
schedulerADT newScheduler(memoryManagerADT mm)
{
      // faltan los chequeos del allocMem
      schedulerADT scheduler = (schedulerADT) allocMem(mm, sizeof(schedulerCDT));
      scheduler->memoryManager = mm;
      scheduler->processesList = (PList *) allocMem(scheduler->memoryManager, sizeof(PList));
      scheduler->pidCounter = 0;
      scheduler->currentProcess = NULL;
      scheduler->life = 0;
      scheduler->processesList->first = scheduler->processesList->last  = NULL;
      scheduler->processesList->size = 0;
      scheduler->processesList->qReady = 0;

      char *argv[] = {"Halt"};
      scheduler->idle = (PCB *) allocMem(scheduler->memoryManager, sizeof(PCB));
      scheduler->idle->pid = scheduler->pidCounter++;
      scheduler->idle->ppid = 0;
      scheduler->idle->name = allocMem(scheduler->memoryManager, strlen("Halt"));
      strcpy(scheduler->idle->name, "Halt");
      scheduler->idle->priority = 1;
      scheduler->idle->state = READY;
      scheduler->idle->rbp = allocMem(scheduler->memoryManager, STACK_SIZE);
      scheduler->idle->rbp = (void *)((char *)scheduler->idle->rbp + STACK_SIZE - 1);
      scheduler->idle->rsp = (void *)((StackFrame *)scheduler->idle->rbp - 1);
      char **argvAux = allocMem(scheduler->memoryManager, sizeof(char *));
      argsCopy(scheduler->memoryManager, argvAux, argv, 1);
      scheduler->idle->argc = 1;
      scheduler->idle->argv = argvAux;
      setNewSF(&halt, 1, argvAux, scheduler->idle->rbp);

      return scheduler;
}

int newProcess(schedulerADT scheduler, char *processName, unsigned int priority, void (*entryPoint)(int, char **), char **argv, int argc)
{
      // dejo este comentario:
      /*
            Creo que en el kernel.c apenas comienza y crea el mm y el scheduler
            para iniciar todo deberia forzar una int20 entonces el scheduler va 
            a tomar el primer proceso que creería que debería ser la shell
      */

      PCB *aux = (PCB *)allocMem(scheduler->memoryManager, sizeof(PCB));
      aux->pid = scheduler->pidCounter++;
      aux->ppid = scheduler->currentProcess->pcb->pid;
      char **argvAux = allocMem(scheduler->memoryManager, sizeof(char *) * argc);
      argsCopy(scheduler->memoryManager, argvAux, argv, argc);
      aux->argc = argc;
      aux->argv = argvAux;
      aux->name = allocMem(scheduler->memoryManager, strlen(processName));
      strcpy(aux->name, processName);
      aux->priority = priority;
      aux->state = READY;
      aux->rbp = allocMem(scheduler->memoryManager, STACK_SIZE);
      aux->rbp = (void *)((char *)aux->rbp + STACK_SIZE - 1);
      setNewSF(entryPoint, argc, argvAux, aux->rbp);

      PNode * auxNode = (PNode *)allocMem(scheduler->memoryManager, sizeof(PNode));
      auxNode->pcb = aux;
      enqueue(scheduler, auxNode);
      
      return aux->pid;
}

uint64_t nextProcess(schedulerADT scheduler, uint64_t currentRsp)
{
      if(scheduler->currentProcess)
      {
            if(scheduler->currentProcess->pcb->state == READY && scheduler->life > 1)
            {
                  scheduler->life--;
                  return currentRsp;
            }
            else
            {
                  if(scheduler->currentProcess->pcb->state == KILLED)
                  {
                        removeProcess(scheduler, scheduler->currentProcess);
                  }
                  else
                  {
                        enqueue(scheduler, scheduler->currentProcess);
                  }
                  scheduler->currentProcess = NULL;
                  return nextProcess(scheduler, currentRsp);
            }
      }
      else
      {
            if(scheduler->processesList->qReady > 0)
            {
                  scheduler->currentProcess = dequeue(scheduler);
                  while (scheduler->currentProcess->pcb->state != READY)
                  {
                        if (scheduler->currentProcess->pcb->state == KILLED)
                        {
                              removeProcess(scheduler, scheduler->currentProcess);
                        }
                        else 
                        {
                              enqueue(scheduler, scheduler->currentProcess);
                        }
                        scheduler->currentProcess = dequeue(scheduler);
                  }
                  scheduler->life = scheduler->currentProcess->pcb->priority;
                  return scheduler->currentProcess->pcb->rsp;
            }

            return scheduler->idle->rsp;
      }
}

int getPid(schedulerADT scheduler){
      return 0;
}

int killProcess(schedulerADT scheduler, int pid){
      return 0;
}

int setPriority(schedulerADT scheduler, int pid, int newPriority){
      return 0;
}

int setState(schedulerADT scheduler, int pid, State newState){
      return 0;
}

static void enqueue(schedulerADT scheduler, PNode * newProcess)
{
      if (isEmpty(scheduler->processesList))
      {
            scheduler->processesList->first = scheduler->processesList->last = newProcess;
      }
      else
      {
            scheduler->processesList->last->next = newProcess;
            scheduler->processesList->last = newProcess;
      }

      if (newProcess->pcb->state == READY)
            scheduler->processesList->qReady++;

      scheduler->processesList->size++;
}

static PCB *dequeue(schedulerADT scheduler)
{
      if (isEmpty(scheduler->processesList))
            return NULL;

      PCB * ans = scheduler->processesList->first;
      scheduler->processesList->first = scheduler->processesList->first->next;
      scheduler->processesList->size--;

      if (ans->state == READY)
            scheduler->processesList->qReady--;

      return ans;
}

static int isEmpty(PList * list) {
      return list->size == 0;
}

static void removeProcess(schedulerADT scheduler, PNode * node) 
{
      freeMem(scheduler->memoryManager, node->pcb->name);
      freeMem(scheduler->memoryManager, node->pcb->argv);
      freeMem(scheduler->memoryManager, node->pcb->rbp);
      freeMem(scheduler->memoryManager, node->pcb);

      PNode * iterator = scheduler->processesList->first;
      while(iterator->next != node)
            iterator = iterator->next;
      iterator->next = node->next;
      freeMem(scheduler->memoryManager, node);
}
/*
      NO OLVIDARME DE LIBERAR TODA LA MEMORIA USADA AL MATAR UN PROCESO */
