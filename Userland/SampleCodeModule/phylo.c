// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "phylo.h"

/*
*    La siguiente implementación se basa en la solución propuesta
*    en el libro Sistemas Operativos Modernos 3era edición de 
*    Andrew Tanenbaum.
*/

typedef enum
{
    THINKING = 1,
    EATING = 2,
    HUNGRY = 3,
} State;

typedef struct Philosopher
{
    uint64_t pid;
    uint64_t sem;
    State state;
} Philosopher;

static Philosopher *philos[MAX_PHILOS];
static int count = 0;
static uint64_t mutexId;
static int problemRunning;

static int right(int num);
static int left(int num);
static void takeForks(int i);
static void placeForks(int i);
static void look(int i);
static int addPhilosopher();
static int removePhilosopher();
static void printTable();

static int right(int num)
{
    return (num + 1) % count;
}

void philo(int argc, char **argv)
{
    int idx = strToInt(argv[1]);
    while (problemRunning)
    {
        takeForks(idx);
        sleep(1);
        placeForks(idx);
        sleep(1);
    }
}

static int left(int num)
{
    return (num + count - 1) % count;
}

static void takeForks(int i)
{
    my_sem_wait(mutexId);
    philos[i]->state = HUNGRY;
    look(i);
    my_sem_post(mutexId);
    my_sem_wait(philos[i]->sem);
}

static void placeForks(int i)
{
    my_sem_wait(mutexId);
    philos[i]->state = THINKING;
    look(left(i));
    look(right(i));
    my_sem_post(mutexId);
}

static void look(int i)
{
    if (philos[i]->state == HUNGRY && philos[left(i)]->state != EATING && philos[right(i)]->state != EATING)
    {
        philos[i]->state = EATING;
        my_sem_post(philos[i]->sem);
    }
}

static int addPhilosopher()
{
    if (count == MAX_PHILOS)
        return -1;

    my_sem_wait(mutexId);
    Philosopher *newPhilo = myMalloc(sizeof(Philosopher));
    if (newPhilo == (void *)0)
        return -1;
    newPhilo->state = THINKING;
    uint64_t semId = my_sem_create(1);
    char * buffAux = myMalloc(10);
    uintToString((uint64_t) count, buffAux,10);
    char * argv[] = {"Philo", buffAux};
    int fd[2];
    fd[0]=0;
    fd[1]=1;
    newPhilo->sem = my_sem_open(semId);
    newPhilo->pid =_syscall(NEW_PROCESS, (uint64_t) &philo, (uint64_t) argv, 2,0,(uint64_t) fd);
    philos[count++] = newPhilo;
    my_sem_post(mutexId);
    myFree(buffAux);
    return 0;
}

static int removePhilosopher()
{
    if (count == PHILO_COUNT)
    {
        return -1;
    }
    count--;
    Philosopher *toRemove = philos[count];
    my_sem_close(toRemove->sem);
    _syscall(KILL_PROCESS,toRemove->pid,0,0,0,0);
    myFree(toRemove);
    my_sem_post(mutexId);

    return 0;
}

static void printTable(int argc, char *argv[])
{
    while (problemRunning)
    {
        my_sem_wait(mutexId);
        for (int i = 0; i < count; i++)
        {
            philos[i]->state == EATING ? putChar('E') : putChar('.');
            putChar(' ');
        }
        putChar('\n');
        my_sem_post(mutexId);
        yield();
    }
}

void phylo(int argc, char **argv)
{
    problemRunning = 1;
    mutexId = my_sem_create(1);
    if (mutexId == 0)
    {
        printError("Error al abrir un semáforo\n");
        return;
    }
    printf("Comenzando el problema de los filosofos\n");
    printf("Para agregar un filosofo presione la tecla 'a'\n");
    printf("Para eliminar un filosofo presione la tecla 'r'\n");
    printf("Para terminar el programa presione la tecla 'q'\n");
    printf("La letra 'E' simboliza a un filosofo comiendo\n");
    printf("El punto '.' simboliza a un filosofo esperando\n\n");
    printf("Cargando filosofos...\n");
    sleep(10);

    for (int i = 0; i < PHILO_COUNT; i++)
        addPhilosopher();
    char *argvAux[] = {"PrintTable"};
    int fd[2];
    fd[0] = 0;
    fd[1] = 1;
    uint64_t printTablePid = _syscall(NEW_PROCESS, (uint64_t)&printTable, (uint64_t)argvAux, 1, 0, (uint64_t) fd);
    while (problemRunning)
    {
        char key = getChar();
        switch (key)
        {
        case 'a':
            if (addPhilosopher() == -1)
                printf("Maximo 8 filosofos\n");
            else
                printf("Se agrego un filosofo\n");
            break;
        case 'r':
            if (removePhilosopher() == -1)
                printf("No se pudo eliminar, minimo 4 filosofos\n");
            else
                printf("Se ha eliminado un filosofo\n");
            break;
        case 'q':
            printf("Programa terminado\n");
            problemRunning = 0;
            break;
        default:
            break;
        }
    }

    for (int i = 0; i < count; i++)
    {
        my_sem_close(philos[i]->sem);
        _syscall(KILL_PROCESS, philos[i]->pid,0,0,0,0);
        myFree(philos[i]);
    }

    count = 0;
    _syscall(KILL_PROCESS, printTablePid,0,0,0,0);
    my_sem_close(mutexId);
}
