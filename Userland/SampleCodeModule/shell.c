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
                argc = prepareArgs(' ', argv, buffer);
                isCommand = checkCommandUserApps(argv[0]);
                if (isCommand >= 0 && argc > 0 && argv[argc - 1][0] == '&')
                {
                    fg = 0;
                    argc--;
                }
                // {"loop", "cat", "wc", "filter", "phylo", "inforeg", "printmem", "cpuid", "trigger0", "trigger6", "time", "mem", "ps", "kill", "nice", "block", "sem", "pipe", "test_mm", "test_processes"};
                switch (isCommand)
                {
                case CASE_LOOP:
                    _syscall(NEW_PROCESS, (uint64_t)&loop, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_CAT:
                    _syscall(NEW_PROCESS, (uint64_t)&cat, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_WC:
                    _syscall(NEW_PROCESS, (uint64_t)&wc, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_FILTER:
                    _syscall(NEW_PROCESS, (uint64_t)&filter, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_PHYLO:
                    _syscall(NEW_PROCESS, (uint64_t)&phylo, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_INFOREG:
                    _syscall(NEW_PROCESS, (uint64_t)&inforeg, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_PRINTMEM:
                    _syscall(NEW_PROCESS, (uint64_t)&getMem, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_CPUID:
                    _syscall(NEW_PROCESS, (uint64_t)&cpuid, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_TRIGGER0:
                    _syscall(NEW_PROCESS, (uint64_t)&exc0Trigger, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_TRIGGER6:
                    _syscall(NEW_PROCESS, (uint64_t)&exc6Trigger, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_TIME:
                    _syscall(NEW_PROCESS, (uint64_t)&getTime, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_MEM:
                    _syscall(NEW_PROCESS, (uint64_t)&mem, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_PS:
                    _syscall(NEW_PROCESS, (uint64_t)&ps, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_KILL:
                    _syscall(NEW_PROCESS, (uint64_t)&kill, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_NICE:
                    _syscall(NEW_PROCESS, (uint64_t)&nice, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_BLOCK:
                    _syscall(NEW_PROCESS, (uint64_t)&block, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_SEM:
                    _syscall(NEW_PROCESS, (uint64_t)&sem, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_PIPE:
                    _syscall(NEW_PROCESS, (uint64_t)&pipe, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_TEST_MM:
                    _syscall(NEW_PROCESS, (uint64_t)&test_mem, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                case CASE_TEST_PROCESSES:
                    _syscall(NEW_PROCESS, (uint64_t)&test_processes, (uint64_t)argv, (uint64_t)argc, fg, 0);
                    break;
                default:
                    printError("El comando ingresado es invalido\n");
                    break;
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
