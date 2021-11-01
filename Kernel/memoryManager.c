#ifdef NO_BUDDY
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <memoryManager.h>
#define STRUCT_POS 0x0000000000600000

struct memoryManagerCDT
{
  size_t size;
} memoryManagerCDT;

typedef long Align;

typedef union header
{
  struct
  {
    union header *ptr;
    size_t size;
  } s;

  Align x;

} Header;

static Header *base;
static Header *free_node = NULL;
size_t total_units;

void initFreeManager(char *heap_base, size_t heap_size);
void* malloc(uint64_t nbytes);
void free(void *block);

memoryManagerADT newMemoryManager(void *startDir, size_t size)
{
  memoryManagerADT mm = (void* )STRUCT_POS;
  mm->size = size;
  initFreeManager((char *)startDir, size);
  return mm;
}

void initFreeManager(char *heap_base, size_t heap_size)
{
  if (heap_base == NULL)
    return;
  total_units = (heap_size + sizeof(Header) - 1) / sizeof(Header) + 1;
  free_node = base = (Header *)heap_base;
  free_node->s.size = total_units;
  free_node->s.ptr = free_node;
}

void *allocMem(memoryManagerADT mm, size_t size)
{
  return malloc(size);
}

void* malloc(uint64_t nbytes)
{

  if (nbytes == 0)
    return 0;

  Header *current_node, *prevptr;
  size_t nunits;
  void *result;
  bool is_allocating;

  nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;

  prevptr = free_node;

  is_allocating = true;
  for (current_node = prevptr->s.ptr; is_allocating; current_node = current_node->s.ptr)
  {
    if (current_node->s.size >= nunits)
    {
      if (current_node->s.size == nunits)
      {
        prevptr->s.ptr = current_node->s.ptr;
      }
      else
      {
        current_node->s.size -= nunits;
        current_node += current_node->s.size;
        current_node->s.size = nunits;
      }

      free_node = prevptr;
      result = current_node + 1;
      is_allocating = false;
    }

    if (current_node == free_node)
      return NULL;

    prevptr = current_node;
  } /* for */

  return result;
}
int freeMem(memoryManagerADT mm, void *p)
{
  free(p);
  return 1;
}
void free(void *block)
{
  if (block == NULL || (((long)block - (long)base) % sizeof(Header)) != 0)
  {
    return;
  }

  Header *free_block, *current_node;
  free_block = (Header *)block - 1;

  if (free_block < base || free_block >= (base + total_units * sizeof(Header)))
  {
    return;
  }

  block = NULL;

  bool external = false;

  for (current_node = free_node; !(free_block > current_node && free_block < current_node->s.ptr); current_node = current_node->s.ptr)
  {
    if (free_block == current_node || free_block == current_node->s.ptr)
    {
      return;
    }
    if (current_node >= current_node->s.ptr && (free_block > current_node || free_block < current_node->s.ptr))
    {
      external = true;
      break;
    }
  }

  if (!external && (current_node + current_node->s.size > free_block || free_block + free_block->s.size > current_node->s.ptr))
  {
    return;
  }

  if (free_block + free_block->s.size == current_node->s.ptr)
  {

    free_block->s.size += current_node->s.ptr->s.size;

    free_block->s.ptr = current_node->s.ptr->s.ptr;
  }
  else
  {

    free_block->s.ptr = current_node->s.ptr;
  }

  if (current_node + current_node->s.size == free_block)
  {

    current_node->s.size += free_block->s.size;

    current_node->s.ptr = free_block->s.ptr;
  }
  else
  {
    current_node->s.ptr = free_block;
  }

  free_node = current_node;
}
void printStatus(memoryManagerADT mm)
{
  char aux[24];
  ncPrint("Estado de la memoria:",12);
  ncNewline();
  ncPrint(" -Tipo de memoria usada: Free List.", 15);
  ncNewline();
  ncPrint(" -Tamanio de memoria: ", 15);
  uintToBase( (uint64_t) mm->size, aux, 10);
  ncPrint(aux, 15);
  ncNewline();
  ncPrint(" -Tamanio de memoria utilizada: ", 15);
  uintToBase( (uint64_t) total_units, aux, 10);
  ncPrint(aux, 15);
  ncNewline();
  ncPrint(" -Tamanio de memoria libre: ", 15);
  uintToBase((uint64_t) (mm->size - total_units), aux, 10);
  ncPrint(aux, 15);
  ncNewline();
}
#endif