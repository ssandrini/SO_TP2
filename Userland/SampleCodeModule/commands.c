#include <commands.h>

char commandsNames[BUILTIN_SIZE][NAME_MAX] = {"help", "clear"};
char info[BUILTIN_SIZE + APPS_SIZE][INFO_MAX] = {"muestra todos los comandos con sus explicaciones\n", "borra toda la pantalla\n", "imprime su ID con un saludo cada 15 segundos\n", "imprime el stdin tal como lo recibe\n", "cuenta la cantidad de lineas del input\n",
                                                 "filtra las vocales del input\n", "implementa el problema de los filosofos comensales\n", "imprime en pantalla el valor de todos los registros\n",
                                                 "realiza un volcado de memoria de 32 bytes a partir de la direccion recibida como argumento \n",
                                                 "despliega los features del procesador\n", "demuestra la excepcion de division por cero\n",
                                                 "demuestra la excepcion de operacion invalida\n", "desplega el dia y la hora del sistema\n", "imprime el estado de la memoria\n",
                                                 "imprime la lista de todos los procesos con sus propiedades\n", "mata un proceso dado su ID\n",
                                                 "cambia la prioridad de un proceso dado su ID y la nueva prioridad\n", "cambia el estado de un proceso entre bloqueado y listo dado su ID\n",
                                                 "imprime la lista de todos los semaforos con sus propiedades\n", "imprime la lista de todos los pipes con sus propiedades\n",
                                                 "chequea el correcto funcionamiento del manejo de memoria\n", "chequea el correcto funcionamiento del manejo de procesos\n"};
char userAppsNames[APPS_SIZE][NAME_MAX] = {"loop", "cat", "wc", "filter", "phylo", "inforeg", "printmem", "cpuid", "trigger0", "trigger6", "time", "mem", "ps", "kill", "nice", "block", "sem", "pipe", "test_mm", "test_processes"};

void help()
{
    int i=0;
    for (; i < BUILTIN_SIZE; i++)
    {
        printTitle(commandsNames[i]);
        printf(": %s", info[i]);
    }
    for (i= 0; i < APPS_SIZE; i++)
    {
        printTitle(userAppsNames[i]);
        printf(": %s", info[i+2]);
    }
}

void clear()
{
    _syscall(CLEAR_SCREEN, 0, 0, 0, 0, 0);
}

void getTime(int argc, char **argv)
{
    if (argc != 1)
    {
        printError("Cantidad de argumentos incorrecta");
        return;
    }
    int date[3];
    int hour[3];
    _syscall(GET_TIME, (uint64_t)date, (uint64_t)hour, 0, 0, 0);
    printf("fecha: ");
    printf(" %d / %d / %d \n", date[2], date[1], date[0]);
    printf("hora: ");
    printf(" %d : %d : %d \n", hour[0], hour[1], hour[2]);
}

void cpuid(int argc, char **argv)
{
    if (argc != 1)
    {
        printError("Cantidad de argumentos incorrecta");
        return;
    }
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

void inforeg(int argc, char **argv)
{
    if (argc != 1)
    {
        printError("Cantidad de argumentos incorrecta");
        return;
    }
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

void getMem(int argc, char **argv)
{
    if (argc != 2)
    {
        printError("Cantidad de argumentos incorrecta");
        return;
    }
    int j = 0;
    while (argv[1][j] != 0)
    {
        if (!((argv[1][j] >= '0' && argv[1][j] <= '9') || (argv[1][j] >= 'A' && argv[1][j] <= 'F')))
        {
            printError("La direccion debe ser hexadecimal\n");
            return;
        }
        else
            j++;
    }
    if (j >= 8)
    {
        printError("La direccion debe estar entre 00000000 y FFFFFFF\n");
        return;
    }
    uint8_t *dir = (uint8_t *)hexaStrToDir(argv[1]);
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

void exc0Trigger(int argc, char **argv)
{
    int a = 5;
    int b = 0;
    int c = a / b;
    printf("%d \n", c);
}

void exc6Trigger(int argc, char **argv)
{
    _exc6Trigger();
}

int checkCommandBuiltIn(char *buffer)
{
    for (int i = 0; i < BUILTIN_SIZE; i++)
    {
        if (strcmp(buffer, commandsNames[i]) == 0)
            return i;
    }
    return -1;
}

int checkCommandUserApps(char *buffer)
{
    for (int i = 0; i < APPS_SIZE; i++)
    {
        if (strcmp(buffer, userAppsNames[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

void mem(int argc, char **argv)
{
    if (argc != 1)
    {
        printError("Cantidad de argumentos incorrecta");
        return;
    }
    _syscall(PRINT_STATUS_MEM, 0, 0, 0, 0, 0);
}

void ps(int argc, char **argv)
{
    if (argc != 1)
    {
        printError("Cantidad de argumentos incorrecta");
        return;
    }
    _syscall(PS, 0, 0, 0, 0, 0);
}

void kill(int argc, char **argv)
{
    if (argc != 2)
    {
        printError("Cantidad de argumentos incorrecta");
        return;
    }
    _syscall(KILL_PROCESS, (uint64_t)strToInt(argv[1]), 0, 0, 0, 0);
}

void nice(int argc, char **argv)
{
    if (argc != 3)
    {
        printError("Cantidad de argumentos incorrecta");
        return;
    }
    if (strToInt(argv[2]) < 1 || strToInt(argv[2]) > 99)
    {
        printError("La prioridad debe estar entre 1 y 99");
        return;
    }
    printf("pid: %d, newPriority: %d\n", strToInt(argv[1]), strToInt(argv[2]));

    _syscall(NICE, (uint64_t)strToInt(argv[1]), (uint64_t)strToInt(argv[2]), 0, 0, 0);
}

void block(int argc, char **argv)
{
    if (argc != 2)
    {
        printError("Cantidad de argumentos incorrecta");
        return;
    }
    
    _syscall(BLOCK_PROCESS, (uint64_t)strToInt(argv[1]), 0, 0, 0, 0);
}

void pipe(int argc, char **argv)
{
    if (argc != 1)
    {
        printError("Cantidad de argumentos incorrecta");
        return;
    }
    printf("Todavia no lo implementamos\n");
}

void sem(int argc, char **argv)
{
    if (argc != 1)
    {
        printError("Cantidad de argumentos incorrecta");
        return;
    }
    printf("Todavia no lo implementamos\n");
}

int prepareArgs(char token, char **argv, char *input)
{
    int index = 0;

    if (*input != token && *input != '\0')
        argv[index++] = input;

    while (*input != '\0')
    {
        if (*input == token)
        {
            *input = 0;
            if (*(input + 1) != token && (*(input + 1) != '\0'))
            {
                if (index >= MAX_ARGS)
                    return index;
                argv[index++] = input + 1;
            }
        }
        input++;
    }
    return index;
}
// loop 1


/*
char info[COMMANDS_SIZE][INFO_MAX] = {"desplega el dia y la hora del sistema\n", "imprime en pantalla el valor de todos los registros\n",
                                          "realiza un volcado de memoria de 32 bytes a partir de la direccion recibida como argumento \n",
                                          "despliega los features del procesador\n", "demuestra la excepcion de division por cero\n",
                                          "demuestra la excepcion de operacion invalida\n", "borra toda la pantalla\n", "imprime el estado de la memoria\n",
                                          "imprime la lista de todos los procesos con sus propiedades\n", "mata un proceso dado su ID\n",
                                          "cambia la prioridad de un proceso dado su ID y la nueva prioridad\n", "cambia el estado de un proceso entre bloqueado y listo dado su ID\n",
                                          "imprime la lista de todos los semaforos con sus propiedades\n", "imprime la lista de todos los pipes con sus propiedades\n"};

*/