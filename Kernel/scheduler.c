#include <scheduler.h>
#include <naiveConsole.h>
typedef struct
{
      int pid;
      int ppid;
      unsigned int priority;
      int fg;
      char *name;
      int argc;
      char **argv;

      void *rsp; // aca tendria que usar el stackFrame
      void *rbp;

      State state;
} PCB;

typedef struct PNode
{
      PCB *pcb;
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
      PNode *idle;
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

static schedulerADT sch;
static PNode *dequeue(schedulerADT scheduler);
static void enqueue(schedulerADT scheduler, PNode *newProcess);
static int argsCpy(memoryManagerADT mm, char **buffer, char **argv, int argc);
static void setNewSF(void (*entryPoint)(int, char **), int argc, char **argv, void *rbp);
static void processExit();
static void wrapper(void (*entryPoint)(int, char **), int argc, char **argv);
static void halt(int argc, char **argv);
static int isEmpty(PList *list);
static void removeProcess(schedulerADT scheduler, PNode *node);
static void printProcessInfo(PNode *n);
char *aux2;
static int argsCpy(memoryManagerADT mm, char **buffer, char **argv, int argc)
{
      for (int i = 0; i < argc; i++)
      {
            buffer[i] = allocMem(mm, sizeof(char) * (strlen(argv[i]) + 1));
            if (buffer[i] == NULL)
                  return -1;
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

static void processExit()
{
      killProcess(sch, sch->currentProcess->pcb->pid);
}

static void wrapper(void (*entryPoint)(int, char **), int argc, char **argv)
{
      entryPoint(argc, argv);
      processExit();
}

static void halt(int argc, char **argv)
{
      while (1)
      {
            //ncPrint("H",10);
            _hlt();
      }
}

// ------------------FUNCIONES DEFINIDAS EN EL TAD------------------------------
schedulerADT newScheduler(memoryManagerADT mm)
{
      // faltan los chequeos del allocMem
      schedulerADT scheduler = allocMem(mm, sizeof(schedulerCDT));
      sch = scheduler; // lo necesito para el wrapper
      scheduler->memoryManager = mm;
      scheduler->processesList = allocMem(scheduler->memoryManager, sizeof(PList));
      scheduler->pidCounter = 0;
      scheduler->currentProcess = NULL;
      scheduler->life = 1;
      scheduler->processesList->first = scheduler->processesList->last = NULL;
      scheduler->processesList->size = 0;
      scheduler->processesList->qReady = 0;

      aux2 = allocMem(scheduler->memoryManager, 10);
      char *argv[] = {"Halt"};
      scheduler->idle = allocMem(scheduler->memoryManager, sizeof(PNode));
      scheduler->idle->pcb = allocMem(scheduler->memoryManager, sizeof(PCB));
      scheduler->idle->pcb->pid = scheduler->pidCounter++;
      scheduler->idle->pcb->ppid = 0;
      scheduler->idle->pcb->name = allocMem(scheduler->memoryManager, strlen(argv[0]));
      strcpy(argv[0], scheduler->idle->pcb->name);
      scheduler->idle->pcb->fg = 1;
      scheduler->idle->pcb->priority = 1;
      scheduler->idle->pcb->state = READY;
      scheduler->idle->pcb->rbp = allocMem(scheduler->memoryManager, STACK_SIZE);
      scheduler->idle->pcb->rbp = (void *)((char *)scheduler->idle->pcb->rbp + STACK_SIZE - 1);
      scheduler->idle->pcb->rsp = (void *)((StackFrame *)scheduler->idle->pcb->rbp - 1);
      char **argvAux = allocMem(scheduler->memoryManager, sizeof(char *));
      argsCpy(scheduler->memoryManager, argvAux, argv, 1);
      scheduler->idle->pcb->argc = 1;
      scheduler->idle->pcb->argv = argvAux;
      setNewSF(&halt, 1, argvAux, scheduler->idle->pcb->rbp);

      return scheduler;
}

int newProcess(schedulerADT scheduler, unsigned int priority, void (*entryPoint)(int, char **), char **argv, int argc, int fg)
{
      if (scheduler->currentProcess != NULL)
      {
            // Solo un foreground puede crear a otro foreground
            if (fg == 1 && scheduler->currentProcess->pcb->fg == 0)
            {
                  return -1;
            }
      }
      PCB *aux = allocMem(scheduler->memoryManager, sizeof(PCB));
      aux->pid = scheduler->pidCounter++;
      if (scheduler->currentProcess != NULL)
            aux->ppid = scheduler->currentProcess->pcb->pid;
      else
            aux->ppid = 0;

      char **argvAux = allocMem(scheduler->memoryManager, sizeof(char *) * argc);
      argsCpy(scheduler->memoryManager, argvAux, argv, argc);
      aux->argc = argc;
      aux->argv = argvAux;
      aux->name = allocMem(scheduler->memoryManager, strlen(argv[0]));
      strcpy(argv[0], aux->name);
      aux->priority = priority;
      aux->state = READY;
      aux->rbp = allocMem(scheduler->memoryManager, STACK_SIZE);
      aux->rbp = (void *)((char *)aux->rbp + STACK_SIZE - 1);
      aux->rsp = (void *)((StackFrame *)aux->rbp - 1);
      aux->fg = fg;
      setNewSF(entryPoint, aux->argc, aux->argv, aux->rbp);

      PNode *auxNode = allocMem(scheduler->memoryManager, sizeof(PNode));
      auxNode->pcb = aux;
      auxNode->next = NULL;
      enqueue(scheduler, auxNode);
      scheduler->processesList->qReady++;

      // bloqueo al padre (la shell deberia bloquearse mientras se corre un ls por ejemplo)
      if (fg == 1 && scheduler->currentProcess != NULL && scheduler->currentProcess != scheduler->idle && scheduler->currentProcess->pcb->fg == 1)
      {
            //ncPrint("aca",10);
            blockProcess(scheduler, scheduler->currentProcess->pcb->pid);
      }
      return aux->pid;
}

void *nextProcess(schedulerADT scheduler, void *currentRsp)
{

      if (scheduler->currentProcess == NULL)
      {
            if (!isEmpty(scheduler->processesList))
                  scheduler->currentProcess = dequeue(scheduler);
            else
                  scheduler->currentProcess = scheduler->idle;
            scheduler->life = scheduler->currentProcess->pcb->priority;
      }
      else
      {
            scheduler->currentProcess->pcb->rsp = currentRsp;
            if (scheduler->life < 1) 
            {
                  if (scheduler->currentProcess != scheduler->idle)
                  {
                        enqueue(scheduler, scheduler->currentProcess);
                  }
                  if (scheduler->processesList->qReady > 0)
                  {
                        do
                        {
                              scheduler->currentProcess = dequeue(scheduler);
                              if (scheduler->currentProcess->pcb->state == KILLED)
                              {
                                    removeProcess(scheduler, scheduler->currentProcess);
                              }
                              else if (scheduler->currentProcess->pcb->state == BLOCKED)
                              {     
                                    enqueue(scheduler, scheduler->currentProcess);
                              }
                        } while (scheduler->processesList->qReady > 0 && scheduler->currentProcess->pcb->state != READY);
                  }
                  else
                  {
                        scheduler->currentProcess = scheduler->idle;
                  }
                  scheduler->life = scheduler->currentProcess->pcb->priority;
            }
      }
      scheduler->life--;
      return scheduler->currentProcess->pcb->rsp;
}

int getPid(schedulerADT scheduler)
{
      return scheduler->currentProcess->pcb->pid;
}

int killProcess(schedulerADT scheduler, int pid)
{
      if (scheduler->currentProcess != NULL && scheduler->currentProcess->pcb->pid == pid)
      {
            scheduler->currentProcess->pcb->state = KILLED;
            scheduler->life = 0;
            scheduler->processesList->qReady--;
            if (scheduler->currentProcess->pcb->fg == 1 && scheduler->currentProcess->pcb->ppid != 0)
            {
                  unblockProcess(scheduler, scheduler->currentProcess->pcb->ppid);
            }
            _int20();
            return 0;
      }

      PNode *current = scheduler->processesList->first;
      while (current != NULL && current->pcb->pid != pid)
      {
            current = current->next;
      }
      if (current == NULL)
            return -1;

      if (current->pcb->state == READY)
            scheduler->processesList->qReady--;
      current->pcb->state = KILLED;

      if (current->pcb->fg == 1 && current->pcb->ppid != 0)
      {
            unblockProcess(scheduler, current->pcb->ppid);
      }

      return 0;
}

int setPriority(schedulerADT scheduler, int pid, int newPriority)
{
      if (newPriority < 1)
      {
            return -1;
      }
      if (scheduler->currentProcess != NULL && scheduler->currentProcess->pcb->pid == pid)
      {
            scheduler->currentProcess->pcb->priority = newPriority;
            scheduler->life = newPriority; // reinicio los ciclos restantes
            return 0;
      }

      PNode *current = scheduler->processesList->first;
      while (current != NULL && current->pcb->pid != pid)
      {
            current = current->next;
      }
      if (current == NULL)
            return -1;

      scheduler->currentProcess->pcb->priority = newPriority;
      return 0;
}

int blockProcess(schedulerADT scheduler, int pid)
{
      if (scheduler->currentProcess->pcb->pid == pid)
      {
            
            scheduler->currentProcess->pcb->state = BLOCKED;
            scheduler->processesList->qReady--;
            scheduler->life = 0;
            //enqueue(scheduler, scheduler->currentProcess);
            _int20();
            return pid;
      }
      else
      {
            PNode *aux = scheduler->processesList->first;

            while (aux != NULL && aux->pcb->pid != pid)
            {
                  aux = aux->next;
            }
            if (aux == NULL)
            {
                  return -1;
            }
            aux->pcb->state = BLOCKED;
            scheduler->processesList->qReady--;
            return pid;
      }
}

int unblockProcess(schedulerADT scheduler, int pid)
{
      PNode *aux = scheduler->processesList->first;

      while (aux != NULL && aux->pcb->pid != pid)
      {
            aux = aux->next;
      }
      if (aux == NULL)
      {
            return -1;
      }
      if (aux->pcb->state == BLOCKED)
      {
            aux->pcb->state = READY;
            scheduler->processesList->qReady++;
      }
      return 0;
}

void yield(schedulerADT scheduler)
{
      scheduler->life = 0;
      _int20();
}

void printProcesses(schedulerADT scheduler)
{
      PNode *node = scheduler->processesList->first;
      char *message = "NAME               PID:   PPID:   PRIORITY:    RSP:     RBP:   GROUND:  STATE:";
      ncPrint(message, 12);
      newLine();
      int i = 0;
      for (i= 0; i < scheduler->processesList->size; node = node->next, i++)
      {
            printProcessInfo(node);
            newLine();
      }


      if (scheduler->currentProcess != NULL)
            printProcessInfo(scheduler->currentProcess);

      newLine();
}

static void printProcessInfo(PNode *n)
{

      char *buff = allocMem(sch->memoryManager, 10);

      ncPrint(n->pcb->name, COLOR);
      int space = strlen(n->pcb->name);
      while (space < 19)
      {
            ncPrint(" ", COLOR);
            space++;
      }

      uintToBase((uint64_t) n->pcb->pid, buff, 10);
      space = n->pcb->pid >= 10 ? 2 : 1;
      space = n->pcb->pid >= 100 ? 3 : space;
      ncPrint(buff, COLOR);
      while (space < 7)
      {
            ncPrint(" ", COLOR);
            space++;
      }

      uintToBase((uint64_t) n->pcb->ppid, buff, 10);
      ncPrint(buff, COLOR);
      ncPrint("   ", COLOR);

      uintToBase((uint64_t) n->pcb->priority, buff, 10);
      ncPrint(buff, COLOR);
      ncPrint("   ", COLOR);

      uintToBase((uint64_t) n->pcb->rsp, buff, 10);
      ncPrint(buff, COLOR);
      ncPrint("   ", COLOR);

      uintToBase((uint64_t) n->pcb->rbp, buff, 16);
      ncPrint(buff, COLOR);
      ncPrint("   ", COLOR);

      uintToBase((uint64_t) n->pcb->fg, buff, 10);
      ncPrint(buff, COLOR);
      ncPrint("   ", COLOR);

      if (n->pcb->state == READY)
      {
            ncPrint("Ready", COLOR);
      }
      else if (n->pcb->state == BLOCKED)
      {
            ncPrint("Blocked", COLOR);
      }
      else
      {
            ncPrint("Killed", COLOR);
      }
}

static void enqueue(schedulerADT scheduler, PNode *newProcess)
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

      scheduler->processesList->size++;
}

static PNode *dequeue(schedulerADT scheduler)
{
      if (isEmpty(scheduler->processesList))
      {
            return NULL;
      }

      PNode *ans = scheduler->processesList->first;
      scheduler->processesList->first = scheduler->processesList->first->next;
      scheduler->processesList->size--;

      return ans;
}

static int isEmpty(PList *list)
{
      return list->size == 0;
}

static void removeProcess(schedulerADT scheduler, PNode *node)
{
      
      freeMem(scheduler->memoryManager, node->pcb->name);
      freeMem(scheduler->memoryManager, node->pcb->argv);
      freeMem(scheduler->memoryManager, node->pcb->rbp);
      freeMem(scheduler->memoryManager, node->pcb);
      freeMem(scheduler->memoryManager, node);
      
}
/*
      NO OLVIDARME DE LIBERAR TODA LA MEMORIA USADA AL MATAR UN PROCESO */
