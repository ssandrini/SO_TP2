#ifdef NO_BUDDY
#include <stdio.h>

#include <memoryManager.h>
/*
    Dejo estos comentarios para que después revisemos bien las direcciones
    de los defines. Además tengo dudas si está bien lo de STRUCT POS
    y me gustaría ver si se puede armar algo tipo patron singleton.
*/
#define STRUCT_POS 0x0000000000600000
#define INITIAL_POS 0x0000000000700000
#define FINAL_POS 0x000000000FFFFFFFF

enum STATE
{
  FREE = 1,
  NOT_FREE = 0
};

typedef struct BNode
{
  struct BNode *next;
  void *memPtr;
  size_t size;
  enum STATE state;
} BNode;

struct memoryManagerCDT
{
  BNode *head;
  void *startDir;
  size_t memorySize;
  size_t usedSize;
} memoryManagerCDT; //es una lista en este caso

BNode *allocMemRec(memoryManagerADT mm, BNode *node, size_t size)
{
  if (node == NULL || node->size == 0)
  {
    return NULL;
  }
  if (node->state == FREE)
  {
    if (node->size == size)
    {
      node->state = NOT_FREE;
      mm->usedSize += node->size;
      return node;
    }
    else if (node->size > size)
    {
      int spaceLeft = node->size - size;
      
      if (spaceLeft > sizeof(BNode))
      {
        BNode *aux = (void *)((char *)node->memPtr + size); //creo el proximo nodo
        aux->next = node->next;
        aux->state = FREE;
        aux->memPtr = (void *)((char *)aux + sizeof(BNode));
        aux->size = spaceLeft - sizeof(BNode);
        node->size = size;
        node->next = aux;
        mm->usedSize += sizeof(BNode);
      }
      node->state = NOT_FREE;
      mm->usedSize += node->size;
      return node;
    }
  }
  return allocMemRec(mm, node->next, size);
}

void *allocMem(memoryManagerADT mm, size_t size)
{
  BNode *node = allocMemRec(mm, mm->head, size);
  if (node == NULL)
  {
    return NULL;
  }
  return node->memPtr;
}

int freeMemRec(memoryManagerADT mm, BNode *actual, BNode *previous, void *ptr)
{
  if (actual == NULL || ptr == NULL)
  {
    return 0;
  }
  if (actual->memPtr == ptr)
  {
    if ((previous == NULL || (previous != NULL && previous->state == NOT_FREE)) && ((actual->next != NULL && actual->next->state == NOT_FREE) || actual->next == NULL))
    { //el que le sigue es null
      actual->state = FREE;
      mm->usedSize -= actual->size;
      return 1;
    }
    else if (previous != NULL && previous->state == FREE && ((actual->next != NULL && actual->next->state == NOT_FREE) || actual->next == NULL))
    { //el que le sigue no es null
      previous->size = previous->size + actual->size + sizeof(BNode);
      previous->next = actual->next;
      mm->usedSize -= (actual->size + sizeof(BNode));
      return 1;
    }
    else if (previous != NULL && actual->next != NULL && previous->state == FREE && actual->next->state == FREE)
    { //cuando apuntan a algo pero son free
      previous->size = previous->size + actual->size + actual->next->size + (2 * sizeof(BNode));
      previous->next = actual->next->next;
      mm->usedSize -= (actual->size + 2 * sizeof(BNode));
      return 1;
    }
    else if ((previous == NULL || (previous->state == NOT_FREE)) && actual->next != NULL && actual->next->state == FREE)
    { //el siguiente apunta a algo pero es free
      mm->usedSize -= (actual->size + sizeof(BNode));
      actual->size = actual->size + actual->next->size + sizeof(BNode);
      actual->next = actual->next->next;
      actual->state = FREE;
      return 1;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    return freeMemRec(mm, actual->next, actual, ptr);
  }
}

int freeMem(memoryManagerADT mm, void *ptr)
{
  return freeMemRec(mm, mm->head, NULL, ptr);
}

memoryManagerADT newMemoryManager(void *startDir, size_t memorySize)
{
  memoryManagerADT memoryManager = startDir;

  memoryManager->memorySize = memorySize;
  memoryManager->startDir = startDir;
  memoryManager->head = (void *)((char *)startDir + sizeof(memoryManagerCDT));

  memoryManager->head->next = NULL;
  memoryManager->head->memPtr = (void *)((char *)memoryManager->head + sizeof(BNode));
  memoryManager->head->size = memorySize - sizeof(memoryManagerCDT) - sizeof(BNode);
  memoryManager->head->state = FREE;

   memoryManager->usedSize = sizeof(memoryManagerCDT) + sizeof(BNode);

  return memoryManager;
}

void printStatus(memoryManagerADT mm)
{
  char aux[24];
  ncPrint("Estado de la memoria:", 12);
  ncNewline();
  ncPrint(" -Tipo de memoria usada: first fit.", 15);
  ncNewline();
  ncPrint(" -Tamanio de memoria: ", 15);
  uintToBase((uint64_t)mm->memorySize, aux, 10);
  ncPrint(aux, 15);
  ncNewline();
  ncPrint(" -Tamanio de memoria utilizada: ", 15);
  uintToBase((uint64_t)mm->usedSize, aux, 10);
  ncPrint(aux, 15);
  ncNewline();
  ncPrint(" -Tamanio de memoria libre: ", 15);
  uintToBase((uint64_t)(mm->memorySize - mm->usedSize), aux, 10);
  ncPrint(aux, 15);
  ncNewline();
}

#endif