//#ifdef BUDDY
#include <memoryManager.h>
#define STRUCT_POS 0x0000000000600000
#define MIN_BLOCK 64

typedef enum
{
    FREE,
    USED,
    DIVIDED
} BState;

typedef struct BNode
{
    struct BNode *left;
    struct BNode *right;
    void *memDir;
    size_t size;
    size_t level;
    BState state;
} BNode;

typedef struct memoryManagerCDT
{
    BNode *root;
    void *memoryDir; // primera posicion de memoria
    void *nextBNodePos;
    size_t memorySize;
    size_t usedSize;
} memoryManagerCDT;

static BNode *newBNode(memoryManagerADT mm, size_t size, void *memDir, size_t level)
{
    BNode *aux = mm->nextBNodePos;
    mm->nextBNodePos = (void *)((char *)mm->nextBNodePos + sizeof(BNode));
    aux->size = size; //node->size / 2;
    aux->left = aux->right = NULL;
    aux->memDir = memDir; //node->memDir;
    aux->state = FREE;
    aux->level = level; //node->level + 1;
    return aux;
}

memoryManagerADT newMemoryManager(void *startDir, size_t size)
{
    if (size < MIN_BLOCK || size % 2 != 0)
    {
        return NULL;
    }
    memoryManagerADT buddyManager = (void *)STRUCT_POS;
    buddyManager->usedSize = 0;
    buddyManager->memorySize = size;
    buddyManager->memoryDir = startDir;
    buddyManager->root = (void *)((char *)buddyManager + sizeof(memoryManagerCDT));
    buddyManager->nextBNodePos = (void *)((char *)buddyManager->root + sizeof(BNode));

    buddyManager->root->left = NULL;
    buddyManager->root->right = NULL;
    buddyManager->root->state = FREE;
    buddyManager->root->memDir = buddyManager->memoryDir;
    buddyManager->root->level = 0;
    buddyManager->root->size = size;

    return buddyManager;
}

static void *allocRecursive(BNode *current, size_t size, memoryManagerADT mm)
{
    void *toReturn = NULL;

    if (current->state == USED)
    {
        return toReturn;
    }

    else if (current->state == DIVIDED)
    {
        toReturn = allocRecursive(current->left, size, mm);
        if (toReturn == NULL)
        {
            return allocRecursive(current->right, size, mm);
        }
        return toReturn;
    }

    else if (current->state == FREE && size <= current->size)
    {
        if (current->size == MIN_BLOCK)
        {
            current->state = USED;
            return current->memDir;
        }

        else if (size <= current->size / 2)
        {
            if (current->left == NULL)
            {
                current->left = newBNode(mm, current->size / 2, current->memDir, current->level + 1);
            }
            if (current->right == NULL)
            {
                current->right = newBNode(mm, current->size / 2, (void *)((char *)current->memDir + current->size / 2), current->level + 1);
            }
            current->state = DIVIDED;
            return allocRecursive(current->left, size, mm);
        }

        else
        {
            current->state = USED;
            return current->memDir;
        }
    }

    return NULL;
}

void *allocMem(memoryManagerADT mm, size_t size)
{
    void *toReturn = allocRecursive(mm->root, size, mm);
    if (toReturn != NULL)
    {
        mm->usedSize += size;
    }
    return toReturn;
}

static int buddyFreeMemoryRec(memoryManagerADT mm, void *p, BNode *node)
{
    int toReturn;
    if (node == NULL)
    {
        return 0;
    }
    if (node->state == FREE)
    {
        return 0;
    }
    else if (node->state == DIVIDED)
    {
        toReturn = buddyFreeMemoryRec(mm, p, node->left);
        if (toReturn == 0)
        {
            toReturn = buddyFreeMemoryRec(mm, p, node->right);
        }
        if (node->left->state == FREE && node->right->state == FREE)
        {
            node->state = FREE;
        }
        return toReturn;
    }
    else
    {
        if (node->memDir == p)
        {
            node->state = FREE;
            mm->usedSize -= node->size;
            return 1;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}

int freeMem(memoryManagerADT mm, void *p)
{
    return buddyFreeMemoryRec(mm, p, mm->root);
}

void printStatus(memoryManagerADT mm)
{
    char aux[24];
    ncPrint("Estado de la memoria:",12);
    ncNewline();
    ncPrint(" -Tipo de memoria usada: buddy.", 15);
    ncNewline();
    ncPrint(" -Tamanio de memoria: ", 15);
    uintToBase( (uint64_t) mm->memorySize, aux, 10);
    ncPrint(aux, 15);
    ncNewline();
    ncPrint(" -Tamanio de memoria utilizada: ", 15);
    uintToBase( (uint64_t) mm->usedSize, aux, 10);
    ncPrint(aux, 15);
    ncNewline();
    ncPrint(" -Tamanio de memoria libre: ", 15);
    uintToBase((uint64_t) (mm->memorySize - mm->usedSize), aux, 10);
    ncPrint(aux, 15);
    ncNewline();
    ncPrint(" -Tamanio minimo del bloque: ",15);
    uintToBase((uint64_t) MIN_BLOCK, aux, 10);
    ncPrint(aux, 15);
    ncNewline();
}

//#endif