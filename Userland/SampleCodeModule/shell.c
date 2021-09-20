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

void (*func_ptr[COMMANDS_SIZE])() = {help, getTime, inforeg, getMem, cpuid, exc0Trigger, exc6Trigger, quadratic, clear, celsius, fahrenheit, polar};

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
        else if (c == TAB)
        {
            ;//ya no tiene ninguna funcion
        }
        else if (c == '\n')
        {
            putChar(c);
            int isCommand = checkCommand(buffer, parameter); //el uno por default
            buffer[0] = 0;
            bIndex = 0;
            if (isCommand >= 0)
            {
                if (isCommand == 3)
                    func_ptr[isCommand](parameter);
                else
                    func_ptr[isCommand]();
                parameter[0] = 0;
            }
            else
            {
                printError("El comando ingresado es invalido\n");
            }
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
