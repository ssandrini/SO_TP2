#include <commands.h>
#define EPSILON 0.00000001
char commandsNames[12][20] = {"help", "time", "inforeg", "printmem", "cpuid", "trigger0", "trigger6", "quadratic", "clear", "celsius", "fahrenheit", "polar"};
char info[11][150] = { "desplega el dia y la hora del sistema\n", "imprime en pantalla el valor de todos los registros\n", 
                    "realiza un volcado de memoria de 32 bytes a partir de la direccion recibida como argumento \n",
                    "despliega los features del procesador\n","demuestra la excepcion de division por cero\n", 
                    "demuestra la excepcion de operacion invalida\n", "obtiene las raices de la funcion quadratica deseada\n",
                    "borra toda la pantalla\n", "realiza la conversion de grados celsius a fahrenheit \n", 
                    "realiza la conversion de grados fahrenheit a celsius \n", "convierte coordenadas rectangulares en polares \n"};

void help()
{
    test_kill();
    /*
    for (int i = 0; i < 11; i++)
    {
        printTitle(commandsNames[i+1]); 
        printf(": %s", info[i]);
    }
    */
}

void clear()
{
    _syscall(CLEAR_SCREEN, 0, 0, 0, 0,0);
}

void getTime()
{
    int date[3];
    int hour[3];
    _syscall(GET_TIME,(uint64_t) date,(uint64_t) hour,0,0,0);
    printf("fecha: ");
    printf(" %d / %d / %d \n", date[2], date[1], date[0]);
    printf("hora: ");
    printf(" %d : %d : %d \n", hour[0], hour[1], hour[2]);
}

void inforeg()
{
    uint64_t registers[19];
    static char *registersName[] = {"R15    ", "R14    ", "R13    ", "R12    ", "R11    ", "R10    ", "R9     ", "R8     ", "RSI    ", "RDI    ", "RBP    ", "RDX    ", "RCX    ", "RBX    ", "RAX    ", "RIP    ", "CS     ", "FLAGS  ", "RSP    "};
    _syscall(GET_REG, (uint64_t) registers,0,0,0,0);
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
    uint8_t * dir = (uint8_t * ) hexaStrToDir(param);
    uint8_t vec[32]; // 32 "registros" de 1 byte 
    _syscall(GET_MEM, (uint64_t) dir, (uint64_t) vec, 0 ,0,0);
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
    int c = a/b;
    printf("%d \n", c);
}

void exc6Trigger()
{
    _exc6Trigger();
}

int checkCommand(char *buffer, char *parameter)
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
                if(len == LENGTH_PRINTMEM + 1) {
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
                if( j >= LENGTH_PRINTMEM + 1 + 8 ) {
                    printError("La direccion debe estar entre 00000000 y FFFFFFF\n");
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

void quadratic()
{
    long double r1, r2;
    char buffs[3][15];
    char abc[3] = {'a', 'b', 'c'};
    long double abcNum[3];
    printf("Ingrese los valores de los coeficientes de la funcion cuadratica\n");
    for (int i = 0; i < 3; i++)
    {
         if (abcNum[0] >= 0 - EPSILON && abcNum[0] <= 0 + EPSILON)
         {
            printf("Valor incorrecto. El coeficiente 'a' debe ser distinto de 0 \n");
            return;
        }
        printf("%c = ", abc[i]);
        int length = readNumFromLine(buffs[i]);
        if (length <= -1)
        {
            printf("\nValor incorrecto. Debe ser un numero.\n");
            return;
        }
        stringToDouble(buffs[i], &abcNum[i]);
    }
    int aux = _quadratic(&abcNum[0], &abcNum[1], &abcNum[2], &r1, &r2);
    if (aux == 0)
    {
        printf("Las raices no son reales \n");
    }
    else
    {
        char root1[60];
        char root2[60];
        doubleToString(r1, root1);
        printTitle("root1");
        printf(" = %s \n", root1);
        doubleToString(r2, root2);
        printTitle("root2");
        printf(" = %s \n", root2);
    }
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

    _syscall(CPU_INFO, (uint64_t) &r1, (uint64_t) &r2, (uint64_t) &ok, 0,0);
    
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
    _syscall(CPU_INFO, (uint64_t) &r1, (uint64_t) &r2, (uint64_t) &ok, 0,0);
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

void celsius() {
    long double deg_c, res;
    long double c1 = (double) 9/5;
    long double c2 = 32;
    char buff[60];
    printf("Ingrese el valor de temperatura en grados celcius para convertirlo a grados fahrenheit\n");
    printf("C = ");
    int length = readNumFromLine(buff);
    if (length <= -1)
    {
        printf("\nValor incorrecto. Debe ser un numero.\n");
        return;
    }
    
    stringToDouble(buff, &deg_c);
    _CtoFahren(&deg_c, &c1, &c2, &res);

    char ans[60];
    doubleToString(res, ans);
    printf("Fahrenheit = %s F\n",ans);
}

void fahrenheit() {
    long double deg_c, res;
    long double c1 = (double) 5/9;
    long double c2 = 32;
    char buff[60];
    printf("Ingrese el valor de temperatura en grados fahrenheit para convertirlo a grados celsius\n");
    printf("F = ");
    int length = readNumFromLine(buff);
    if (length <= -1)
    {
        printf("\nValor incorrecto. Debe ser un numero.\n");
        return;
    }
    
    stringToDouble(buff, &deg_c);
    _FtoCelcius(&deg_c, &c1, &c2, &res);

    char ans[60];
    doubleToString(res, ans);
    printf("Celsius = %s C\n",ans);
}

void polar() {
    //extern void _rectToPolar(long double * x, long double * y, long double * mod, long double * angle, long double * c);
    long double rects[2];
    long double c = 180;
    long double ans[2];
    char buffs[2][60];
    char xy[2]={'X','Y'};
    printf("Ingrese las coordenadas X e Y que desee convertir a polares\n");
    for (int i = 0; i < 2; i++)
    {
        printf("%c = ", xy[i]);
        int length = readNumFromLine(buffs[i]);
        if (length <= -1)
        {
            printf("\nValor incorrecto. Debe ser un numero.\n");
            return;
        }
        stringToDouble(buffs[i], &rects[i]);
    }
   
    _rectToPolar(&rects[0], &rects[1], &ans[0], &ans[1], &c);

    char stringAns[60];
    doubleToString(ans[0], stringAns);
    printf("Modulo = %s \n",stringAns);
    doubleToString(ans[1], stringAns);
    printf("Angulo = %s grados\n",stringAns);
}