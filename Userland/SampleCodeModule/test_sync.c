
#include <stdint.h>
#include <tests.h>
#define TOTAL_PAIR_PROCESSES 2
int64_t global; //shared memory
uint64_t sem_id;

void inc(int argc, char **argv);

static uint64_t my_create_process2(char **argv)
{
    int argc = 4;
    int fd[2];
    fd[0] = 0;
    fd[1] = 1;
    return _syscall(NEW_PROCESS, (uint64_t)&inc, (uint64_t)argv, argc, 0, (uint64_t)fd);
}

static uint64_t my_sem_create(uint64_t initialValue)
{
    return _syscall(CREATE_SEM, initialValue, 0, 0, 0, 0);
}
static uint64_t my_sem_open(uint64_t sem_id)
{
    return _syscall(OPEN_SEM, sem_id, 0, 0, 0, 0);
}

static uint64_t my_sem_wait(uint64_t sem_id)
{
    return _syscall(WAIT_SEM, sem_id, 0, 0, 0, 0);
}

static uint64_t my_sem_post(uint64_t sem_id)
{
    return _syscall(POST_SEM, sem_id, 0, 0, 0, 0);
}

static uint64_t my_sem_close(uint64_t sem_id)
{
    return _syscall(CLOSE_SEM, sem_id, 0,0,0,0);
}

void yield()
{
    _syscall(YIELD, 0, 0, 0, 0, 0);
}

void slowInc(int64_t *p, int64_t inc)
{
    int64_t aux = *p;
    aux += inc;
    yield();
    *p = aux;
}

//void inc(uint64_t sem, int64_t value, uint64_t N)
void inc(int argc, char **argv)
{
    int sem = strToInt(argv[1]);
    int value = strToInt(argv[2]);
    int N = strToInt(argv[3]);
    void *aux;
    uint64_t i;

    printf("voy a abrir con el id %d",(int)sem_id);
    if (sem && !my_sem_open(sem_id))
    {
        printf("ERROR OPENING SEM\n");
        return;
    }

    for (i = 0; i < N; i++)
    {
        if (sem)
            my_sem_wait(sem_id);
        slowInc(&global, value);
        if (sem)
            my_sem_post(sem_id);
    }

    if (sem)
        my_sem_close(sem_id);

    printf("Final value: %d\n", global);
}

void test_sync(int argc, char **argv)
{
    uint64_t i;

    global = 0;

    printf("CREATING PROCESSES...(WITH SEM)\n");
    char *argv1[] = {"inc", "1", "1", "100000"};
    char *argv2[] = {"inc", "1", "-1", "100000"};

    sem_id = my_sem_create(1);

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
    {
        my_create_process2(argv1);
        my_create_process2(argv2);
    }
}

void test_no_sync(int argc, char **argv)
{
    uint64_t i;

    global = 0;

    printf("CREATING PROCESSES...(WITHOUT SEM)\n");

    char *argv1[] = {"inc", "0", "1", "100000"};
    char *argv2[] = {"inc", "0", "-1", "100000"};

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
    {
        my_create_process2(argv1);
        my_create_process2(argv2);
    }
}
