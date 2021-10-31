#include <shell.h>

#define MAX_SIZE 100
#define ESC 1
#define TAB -12
#define BSPACE -10

char buffer[MAX_SIZE];
char parameter[MAX_SIZE];
char user[MAX_SIZE / 2];
char c = 0;
int bIndex;
int uIndex;
int exit = 0;
int exitUser;
void (*func_ptr[BUILTIN_SIZE])() = {help, clear};
static int getPipeIndex(int argc, char **argv);
static void runPipe(int argc, char **argv, int pipeIndex);
static uint64_t newProcess(int isCommand, int argc, char **argv, int fg, int *fd);
//static void waitPipe(uint64_t pipe, uint64_t pid1, uint64_t pid2);

void shell()
{

    clear();
    requestUser();
    printUser(user);
    printUser(":$ ");

    while (!exit)
    {
        c = getChar();

        if (c == ESC)
        {
            exit = 1;
        }
        else if (c == '\n')
        {
            putChar(c);
            int isCommand = checkCommandBuiltIn(buffer);
            if (isCommand >= 0)
            {
                func_ptr[isCommand]();
            }
            else if (isCommand == -1)
            {
                int argc = 0;
                char *argv[MAX_ARGS] = {0};
                int fg = 1;
                int fd[2];
                fd[0] = 0;
                fd[1] = 1;
                argc = prepareArgs(' ', argv, buffer);

                int pipeIndex = getPipeIndex(argc, argv);

                if (pipeIndex != -1)
                {
                    if (pipeIndex == 0 || pipeIndex == argc - 1)
                    {
                        printError("El pipe debe colocarse entre dos comandos\n");
                    }
                    else
                    {
                        runPipe(argc, argv, pipeIndex);
                    }
                }
                else
                {
                    isCommand = checkCommandUserApps(argv[0]);
                    if (isCommand >= 0 && argc > 0 && argv[argc - 1][0] == '&')
                    {
                        fg = 0;
                        argc--;
                    }
                    newProcess(isCommand, argc, argv, fg, fd);
                }
            }
            buffer[0] = 0;
            bIndex = 0;
            printUser(user);
            printUser(":$ ");
        }
        else if (c != 0)
        {
            if (c == BSPACE)
            {
                if (bIndex > 0)
                {
                    putChar(c);
                    buffer[--bIndex] = 0;
                }
            }
            else
            {
                if (bIndex < MAX_SIZE)
                {
                    putChar(c);
                    buffer[bIndex++] = c;
                    buffer[bIndex] = 0;
                }
            }
        }
    }
    clear();
    printf("Hasta luego %s.", user);
    return;
}

void requestUser()
{
    uIndex = 0;
    exitUser = 0;
    printf("Bienvenido, ingrese su usuario: ");
    while (!exitUser)
    {
        c = getChar();

        if (c == '\n' || c == ESC)
        {
            exitUser = 1;
            if (c == ESC)
            {
                exit = 1;
            }
        }
        else if (c == BSPACE)
        {
            if (uIndex > 0)
            {
                putChar(c);
                user[--uIndex] = 0;
            }
        }
        else if (c == TAB)
        {
            ; // no se permite el tab mientras se escribe usuario
        }
        else if (c != 0)
        {
            if (uIndex < MAX_SIZE / 2)
            {
                user[uIndex++] = c;
                user[uIndex] = 0;
                putChar(c);
            }
        }
    }
    clear();
    printf("Bienvenido %s, Puedes escribir el comando help para ver los diferentes comandos del sistema\n", user);
    printf("Para finalizar la shell oprima ESC\n");
}

static int getPipeIndex(int argc, char **argv)
{
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-") == 0)
            return i;
    }
    return -1;
}

static void runPipe(int argc, char **argv, int pipeIndex)
{
    if (argc != 3)
    {
        printError("mal\n");
    }
    else
    {
        printf("aca todo bien\n");
        int leftCommand = checkCommandUserApps(argv[0]);
        int rightCommand = checkCommandUserApps(argv[2]);
        if (leftCommand && rightCommand)
        {
            /*
            uint64_t pipeId = _syscall(CREATE_PIPE, 0, 0, 0, 0, 0);
            uint64_t newfd = _syscall(CREATE_FD, pipeId, 0, 0, 0, 0);
            int leftFd[2];
            leftFd[0] = 0;
            leftFd[1] = (int)newfd;
            int rightFd[2];
            rightFd[0] = (int)newfd;
            rightFd[1] = 1;
            char *newArgv[MAX_ARGS];
            int newArgc = 1;
            newArgv[0] = argv[0];

            uint64_t pid1 = newProcess(leftCommand, newArgc, newArgv, 0, leftFd);
            newArgv[0] = argv[2];
            uint64_t pid2 = newProcess(rightCommand, newArgc, newArgv, 0, rightFd);
            
            //waitPipe(pipeId, pid1, pid2);
            */
        }
        else
        {
            printf("comandos invalidos\n");
        }
    }
}

static uint64_t newProcess(int isCommand, int argc, char **argv, int fg, int *fd)
{
    switch (isCommand)
    {
    case CASE_LOOP:
        return _syscall(NEW_PROCESS, (uint64_t)&loop, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_CAT:
        return _syscall(NEW_PROCESS, (uint64_t)&cat, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_WC:
        return _syscall(NEW_PROCESS, (uint64_t)&wc, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_FILTER:
        return _syscall(NEW_PROCESS, (uint64_t)&filter, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_PHYLO:
        return _syscall(NEW_PROCESS, (uint64_t)&phylo, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_INFOREG:
        return _syscall(NEW_PROCESS, (uint64_t)&inforeg, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_PRINTMEM:
        return _syscall(NEW_PROCESS, (uint64_t)&getMem, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_CPUID:
        return _syscall(NEW_PROCESS, (uint64_t)&cpuid, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_TIME:
        return _syscall(NEW_PROCESS, (uint64_t)&getTime, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_MEM:
        return _syscall(NEW_PROCESS, (uint64_t)&mem, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_PS:
        return _syscall(NEW_PROCESS, (uint64_t)&ps, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_KILL:
        return _syscall(NEW_PROCESS, (uint64_t)&kill, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_NICE:
        return _syscall(NEW_PROCESS, (uint64_t)&nice, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_BLOCK:
        return _syscall(NEW_PROCESS, (uint64_t)&block, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_SEM:
        return _syscall(NEW_PROCESS, (uint64_t)&sem, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_PIPE:
        return _syscall(NEW_PROCESS, (uint64_t)&pipe, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_TEST_MM:
        return _syscall(NEW_PROCESS, (uint64_t)&test_mem, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_TEST_PROCESSES:
        return _syscall(NEW_PROCESS, (uint64_t)&test_processes, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    case CASE_TEST_PRIO:
        return _syscall(NEW_PROCESS, (uint64_t)&test_prio, (uint64_t)argv, (uint64_t)argc, fg, (uint64_t)fd);
        break;
    default:
        printError("El comando ingresado es invalido\n");
        return 0;
        break;
    }
}
/*
static void waitPipe(uint64_t pipe, uint64_t pid1, uint64_t pid2)
{
    char eof[2] = {-1, 0};
    _syscall(WAIT_PID, pid1,0,0,0,0);

    _syscall(WRTIE_PIPE, pipe,(uint64_t) eof,0,0,0);

    _syscall(WAIT_PID, pid2,0,0,0,0);

    _syscall(CLOSE_PIPE, pipe,0,0,0,0);
    _syscall(FREE_PIPE, pid1,0,0,0,0);
}
*/