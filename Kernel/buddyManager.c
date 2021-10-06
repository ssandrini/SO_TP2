#ifdef BUDDY
#include <memoryManager.h>

/*


*/

#define STRUCT_POS 0x0000000000600000
#define MIN_BLOCK 2
/* este DEFINE va en kernel.c y es la dir que recibe por param al crear
#define INITIAL_POS 0x0000000000700000
*/

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
    size_t treeSize;
    size_t memorySize;
    size_t usedSize;
    size_t maxLevel;
} memoryManagerCDT;

static BNode *newBNode(memoryManagerADT mm, BNode *node)
{
    BNode *aux = mm->nextBNodePos;
    mm->nextBNodePos = (void *)((char *)mm->nextBNodePos + sizeof(BNode));
    aux->size = node->size / 2;
    aux->left = aux->right = NULL;
    aux->memDir = node->memDir;
    aux->state = FREE;
    aux->level = node->level + 1;
    return aux;
}

memoryManagerADT newMemoryManager(void *startDir, size_t size)
{
    if (size < MIN_BLOCK || size % 2 != 0)
    {
        return NULL;
    }
    memoryManagerADT buddyManager = (void *) STRUCT_POS;
    buddyManager->maxLevel = 0; // hay que calcularlo
    buddyManager->treeSize = 0; // hay que calcularlo
    buddyManager->usedSize = 0;
    buddyManager->memoryDir = startDir;
    buddyManager->root = (void *)((char *)buddyManager + sizeof(memoryManagerCDT));
    buddyManager->nextBNodePos = (void *)((char *)buddyManager->root + sizeof(BNode));
    
    buddyManager->root->left = NULL;
    buddyManager->root->right = NULL;
    buddyManager->root->state = FREE;
    buddyManager->root->memDir = buddyManager->memoryDir;
    buddyManager->root->level = 0;

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
                current->left = newBNode(mm, current);
            }
            if (current->right == NULL)
            {
                current->right = newBNode(mm, current);
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

#endif