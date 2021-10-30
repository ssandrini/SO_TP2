#include <commands.h>

char commandsNames[COMMANDS_SIZE][NAME_MAX] = {"help", "time", "inforeg", "printmem", "cpuid", "trigger0", "trigger6", "clear", "mem", "ps", "kill", "nice", "block", "sem", "pipe"};
char info[COMMANDS_SIZE - 1][INFO_MAX] = {"desplega el dia y la hora del sistema\n", "imprime en pantalla el valor de todos los registros\n",
                                          "realiza un volcado de memoria de 32 bytes a partir de la direccion recibida como argumento \n",
                                          "despliega los features del procesador\n", "demuestra la excepcion de division por cero\n",
                                          "demuestra la excepcion de operacion invalida\n", "borra toda la pantalla\n", "imprime el estado de la memoria\n",
                                          "imprime la lista de todos los procesos con sus propiedades\n", "mata un proceso dado su ID\n",
                                          "cambia la prioridad de un proceso dado su ID y la nueva prioridad\n", "cambia el estado de un proceso entre bloqueado y listo dado su ID\n",
                                          "imprime la lista de todos los semaforos con sus propiedades\n", "imprime la lista de todos los pipes con sus propiedades\n"};

char userAppsNames[APPS_SIZE][NAME_MAX] = {"loop", "cat", "wc", "filter", "phylo"};
char userAppsInfo[APPS_SIZE][INFO_MAX] = {"imprime su ID con un saludo cada 15 segundos\n", "imprime el stdin tal como lo recibe\n", "cuenta la cantidad de lineas del input\n", "filtra las vocales del input\n", "implementa el problema de los filosofos comensales\n"};

char testsAppsNames[TESTS_SIZE][NAME_MAX] = {};
char testsAppsInfo[TESTS_SIZE][INFO_MAX] = {};

void help()
{
    for (int i = 0; i < COMMANDS_SIZE -1; i++)
    {
        printTitle(commandsNames[i + 1]);
        printf(": %s", info[i]);
    }
    for (int i = 0; i < APPS_SIZE; i++)
    {
        printTitle(userAppsNames[i]);
        printf(": %s", userAppsInfo[i]);
    }
    for (int i = 0; i < TESTS_SIZE; i++)
    {
        printTitle(testsAppsNames[i]);
        printf(": %s", testsAppsInfo[i]);
    }
}

void clear()
{
    _syscall(CLEAR_SCREEN, 0, 0, 0, 0, 0);
}

void getTime()
{
    int date[3];
    int hour[3];
    _syscall(GET_TIME, (uint64_t)date, (uint64_t)hour, 0, 0, 0);
    printf("fecha: ");
    printf(" %d / %d / %d \n", date[2], date[1], date[0]);
    printf("hora: ");
    printf(" %d : %d : %d \n", hour[0], hour[1], hour[2]);
}

void cpuid()
{
    //para poner en el run.sh y ver si cambian los features
    //-cpu qemu64,mmx=on,sse2=on,f16c=on,sse=off,vaes=on

    uint32_t r1;
    uint32_t r2;

    uint32_t masks1[8] = {0x1, 0x2, 0x1000, 0x80000, 0x100000, 0x2000000, 0x10000000, 0x20000000};
    char namesr1[8][20] = {"sse3", "pclmulqdq", "fma", "sse41", "sse42", "aesni", "avx", "f16c"};
    //              bit =        0       1            12      19       20       25        28           29

    uint32_t masks2[3] = {0x800000, 0x2000000, 0x4000000};
    char namesr2[3][20] = {"mx", "sse", "sse2"};
    //              bit =       23         25          26

    uint32_t sMasks1[2] = {0x200, 0x400};
    char snamesr1[2][20] = {"vaesni", "vpclmulqdq"};
    //              bit =           9         10

    uint32_t sMasks2 = 0x20;
    char snamesr2[13] = "avx2";
    //              bit =      5

    int ok = FEATURES_ID;
    uint32_t aux = 0;

    _syscall(CPU_INFO, (uint64_t)&r1, (uint64_t)&r2, (uint64_t)&ok, 0, 0);

    if (ok)
    {
        printf("El procesador soporta la instruccion CPUID\n");
    }
    else
    {
        printf("El procesador no soporta la instruccion CPUID \n");
        return;
    }

    printf("Ademas soporta las siguientes features: \n");
    for (int i = 0; i < 8; i++)
    {
        if (i > 0 && i % 4 == 0)
            printf("\n");
        aux = r1 & masks1[i];
        if (aux != 0)
            aux = 1;
        printTitle(namesr1[i]);
        printf(" : %s  ", aux == 1 ? "SI" : "NO");
    }

    printf("\n");
    for (int i = 0; i < 3; i++)
    {
        aux = r2 & masks2[i];
        if (aux != 0)
            aux = 1;
        printTitle(namesr2[i]);
        printf(" : %s  ", aux == 1 ? "SI" : "NO");
    }

    printf("\n");

    ok = SPECIAL_FEATURES_ID;
    _syscall(CPU_INFO, (uint64_t)&r1, (uint64_t)&r2, (uint64_t)&ok, 0, 0);
    for (int i = 0; i < 2; i++)
    {
        aux = r1 & sMasks1[i];
        if (aux != 0)
            aux = 1;
        if (i == 1)
            printf("\n");
        printTitle(snamesr1[i]);
        printf(" : %s  ", aux == 1 ? "SI" : "NO");
    }
    printf("\n");

    aux = r2 & sMasks2;
    if (aux != 0)
        aux = 1;
    printTitle(snamesr2);
    printf(" : %s  \n", aux == 1 ? "SI" : "NO");
}

void inforeg()
{
    uint64_t registers[19];
    static char *registersName[] = {"R15    ", "R14    ", "R13    ", "R12    ", "R11    ", "R10    ", "R9     ", "R8     ", "RSI    ", "RDI    ", "RBP    ", "RDX    ", "RCX    ", "RBX    ", "RAX    ", "RIP    ", "CS     ", "FLAGS  ", "RSP    "};
    _syscall(GET_REG, (uint64_t)registers, 0, 0, 0, 0);
    for (int i = 0; i < 19; i++)
    {
        if (i > 0 && i % 3 == 0)
            printf("\n");
        printTitle(registersName[i]);
        printf(": %xh    ", registers[i]);
    }
    printf("\n");
}

void getMem(char *param)
{
    uint8_t *dir = (uint8_t *)hexaStrToDir(param);
    uint8_t vec[32]; // 32 "registros" de 1 byte
    _syscall(GET_MEM, (uint64_t)dir, (uint64_t)vec, 0, 0, 0);
    printTitle("Volcado de memoria byte a byte a partir de la direccion solicitada: \n");
    for (int i = 0; i < 32; i++)
    {
        if (i > 0 && i % 16 == 0)
            printf("\n");
        printf("%Xh  ", vec[i]);
    }
    printf("\n");
}

void exc0Trigger()
{
    int a = 5;
    int b = 0;
    int c = a / b;
    printf("%d \n", c);
}

void exc6Trigger()
{
    _exc6Trigger();
}

int checkCommandBuiltIn(char *buffer, char *parameter)
{
    char aux[LENGTH_PRINTMEM];
    for (int i = 0; i < COMMANDS_SIZE; i++)
    {
        if (i == CASE_GETMEM)
        {
            int j = 0;
            for (; j < LENGTH_PRINTMEM; j++)
            {
                aux[j] = buffer[j];
            }
            aux[j] = 0;
            if (strcmp(aux, commandsNames[i]) == 0 && buffer[LENGTH_PRINTMEM] == ' ')
            {
                int len = strlen(buffer);
                if (len == LENGTH_PRINTMEM + 1)
                {
                    printError("Indique la direccion como parametro de la forma printmem DIR (hexa) \n");
                    return -1;
                }
                j = LENGTH_PRINTMEM + 1;
                while (buffer[j] != 0)
                {
                    if ((buffer[j] >= '0' && buffer[j] <= '9') || (buffer[j] >= 'A' && buffer[j] <= 'F'))
                    {
                        *parameter++ = buffer[j++];
                    }
                    else
                        return -1;
                }
                if (j >= LENGTH_PRINTMEM + 1 + 8)
                {
                    printError("La direccion debe estar entre 00000000 y FFFFFFF\n");
                    return -1;
                }
                *parameter = 0;
                return i;
            }
        }
        else if (i == CASE_KILL || i == CASE_NICE)
        {
            int j = 0;
            for (; j < LENGTH_KILL; j++)
            {
                aux[j] = buffer[j];
            }
            aux[j] = 0;
            if (strcmp(aux, commandsNames[i]) == 0 && buffer[LENGTH_KILL] == ' ')
            {
                int len = strlen(buffer);
                if (len == LENGTH_KILL + 1)
                {
                    printError("Indique el PID como parametro \n");
                    return -1;
                }
                j = LENGTH_KILL + 1;
                while (buffer[j] != 0)
                {
                    if (buffer[j] >= '0' && buffer[j] <= '9')
                    {
                        *parameter++ = buffer[j++];
                    }
                    else
                        return -1;
                }
                *parameter = 0;
                return i;
            }
        }
        else if (i == CASE_BLOCK)
        {
            int j = 0;
            for (; j < LENGTH_BLOCK; j++)
            {
                aux[j] = buffer[j];
            }
            aux[j] = 0;
            if (strcmp(aux, commandsNames[i]) == 0 && buffer[LENGTH_BLOCK] == ' ')
            {
                int len = strlen(buffer);
                if (len == LENGTH_BLOCK + 1)
                {
                    printError("Indique el PID como parametro \n");
                    return -1;
                }
                j = LENGTH_BLOCK;
                while (buffer[j] != 0)
                {
                    if (buffer[j] >= '0' && buffer[j] <= '9')
                    {
                        *parameter++ = buffer[j++];
                    }
                    else
                        return -1;
                }
                *parameter = 0;
                return i;
            }
        }
        else if (strcmp(buffer, commandsNames[i]) == 0)
            return i;
    }
    return -1;
}

int checkCommandUserApps(char *buffer, char *parameter)
{

    /*
      int pipeIdx = findPipe(argc, argv);
      if (pipeIdx != -1)
      {
            if (pipeIdx == 0 || pipeIdx == argc - 1)
            {
                  print("Pipe should be between two commands\n");
                  return;
            }
            if (runPipe(pipeIdx, argv, argc, fg) == -1)
            {
                  print("One of the pipe commands was not valid \n");
                  return;
            }
            return;
      }
      */

    int index = -1;
    char aux[LENGTH_CAT];
    for (int i = 0; i < APPS_SIZE; i++)
    {
        /*
        if (i == CASE_CAT)
        {
            int j = 0;
            for (; j < LENGTH_CAT; j++)
            {
                aux[j] = buffer[j];
            }
            aux[j] = 0;
            if (strcmp(aux, userAppsNames[i]) == 0 && buffer[LENGTH_CAT] == ' ')
            {
                int len = strlen(buffer);
                if(len == LENGTH_CAT + 1) {
                    printError("Escriba el texto en el formato cat TEXTO \n");
                    return -1;
                }
                j = LENGTH_CAT + 1;
                while (buffer[j] != 0)
                {
                        *parameter++ = buffer[j++];
                }
                *parameter = 0;
                return i;
            }
        }
        else 
        */
        if (strcmp(buffer, userAppsNames[i]) == 0)
        {
            return i;
        }
    }
    return index;

    // createProcess(userAppsNames[i], , , , );  Hay que ver que recibe bien
}

void mem()
{
    _syscall(PRINT_STATUS_MEM, 0, 0, 0, 0, 0);
    /* char *argv[] = {"Pepe"};
    _syscall(NEW_PROCESS, (uint64_t) pepe3, (uint64_t) argv, 1, 1, 0);
    */
    //test_processes();
}
void ps()
{
    _syscall(PS, 0, 0, 0, 0, 0);
}

void kill(int pid)
{
    printf("%d\n",pid);
    _syscall(KILL_PROCESS, (uint64_t)pid, 0, 0, 0, 0);
}

void nice(int pid)
{
    _syscall(NICE, (uint64_t)pid, (uint64_t)5, 0, 0, 0);
}

void block(int pid)
{
    _syscall(BLOCK_PROCESS, (uint64_t)pid, 0, 0, 0, 0);
}

void pipe()
{
}

void sem()
{
}