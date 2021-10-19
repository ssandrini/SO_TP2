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
} PCB;

typedef struct PNode
{
      PCB pcb;
      struct PNode *next;
} PNode;

typedef struct PList
{
      uint64_t size;
      PNode *first;
      PNode *last;
} PList;

typedef struct schedulerCDT
{
      memoryManagerADT memoryManager;
      PList *processesList;
      int pidCounter;
      PNode *currentProcess;
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
      scheduler->processesList->first = scheduler->processesList->last  = NULL;
      scheduler->processesList->size = 0;

      char *argv[] = {"Halt"};
      scheduler->idle = (PCB *) allocMem(scheduler->memoryManager, sizeof(PCB));
      scheduler->idle->pid = scheduler->pidCounter++;
      scheduler->idle->ppid = 0;
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
      aux->ppid = scheduler->currentProcess->pcb.pid;
      char **argvAux = allocMem(scheduler->memoryManager, sizeof(char *) * argc);
      argsCopy(scheduler->memoryManager, argvAux, argv, argc);
      scheduler->idle->argc = argc;
      scheduler->idle->argv = argvAux;
      strcpy(aux->name, processName);
      aux->priority = priority;
      aux->state = READY;
      aux->rbp = allocMem(scheduler->memoryManager, STACK_SIZE);
      aux->rbp = (void *)((char *)aux->rbp + STACK_SIZE - 1);
      setNewSF(entryPoint, argc, argvAux, aux->rbp);

      // aca faltaria meterlo en la lista circular 
      // y retornar pid en caso de que salga todo bien
      // y podria retornar -1 si algun alloc falló
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

/*
      NO OLVIDARME DE LIBERAR TODA LA MEMORIA USADA AL MATAR UN PROCESO */