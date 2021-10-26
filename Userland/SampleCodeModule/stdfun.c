#include <stdfun.h>

#define BSIZE 10
#define BSPACE -10

char auxBuffer[BSIZE] = {0};
int currentIdx = 0;

void putChar(const char ascii)
{
    char toPrint[2];
    toPrint[0] = ascii;
    toPrint[1] = 0;
    _syscall(WRITE, (uint64_t) toPrint, (uint64_t) 15,0,0);
}

char getChar()
{
    char k = 0;
    _syscall(READ,(uint64_t) &k,(uint64_t) 1,0,0);
    return k;
}

int strlen(const char *string)
{
    int i = 0;
    while (*string++ != 0)
    {
        i++;
    }
    return i;
}
//-------------------------------------------------------------------------------------
// leves cambios, nos basamos en la implementacion del libro K&R de C y con esta fuente
// https://www.techiedelight.com/implement-itoa-function-in-c/
// Iterative function to implement itoa() function in C
int numToStr(int value, char *auxBuffer, int base)
{
    if (base < 2 || base > 32)
        return -1;
    int n = value;
    if (n < 0)
    {
        n = -n;
    }
    int i = 0;
    while (n)
    {
        int r = n % base;
        if (r >= 10)
            auxBuffer[i++] = 65 + (r - 10);
        else
            auxBuffer[i++] = 48 + r;

        n = n / base;
    }
    if (i == 0)
        auxBuffer[i++] = '0';
    if (value < 0 && base == 10)
        auxBuffer[i++] = '-';
    auxBuffer[i] = '\0'; 
    reverse(auxBuffer, 0, i - 1);
    return i;
}

char *reverse(char *auxBuffer, int i, int j)
{
    while (i < j)
        swap(&auxBuffer[i++], &auxBuffer[j--]);
    return auxBuffer;
}

void swap(char *x, char *y)
{
    char t = *x;
    *x = *y;
    *y = t;
}

//-------------------------------------------------------------------------------------

void strcpy(char *destination, const char *source)
{
    while (*source != '\0')
    {
        *destination = *source;
        destination++;
        source++;
    }
    *destination = '\0';
    return;
}

// nos basamos en este codigo aunque hicimos varias modificaciones
// https://opensource.apple.com/source/xnu/xnu-201/osfmk/kern/printf.c.auto.html
void printf(char *str, ...)
{
    va_list args;
    int i = 0, h, j = 0; // i lectura en str  - j pos en auxBuffer
    int forindex;
    char buff[100] = {0}, tmp[20], tmp2[8];
    char *str_arg;
    va_start(args, str);
    while (str && str[i])
    {
        if (str[i] == '%')
        {
            i++;
            switch (str[i])
            {
                case 'c':
                {
                    buff[j] = (char)va_arg(args, int);
                    j++;
                    break;
                }
                case 'd':
                {
                    numToStr(va_arg(args, int), tmp, 10); //base 10
                    strcpy(&buff[j], tmp);
                    j += strlen(tmp);
                    break;
                }
                case 's':
                {
                    str_arg = (char *)va_arg(args, char *);
                    strcpy(&buff[j], str_arg);
                    j += strlen(str_arg);
                    break;
                }
                case 'x': // imprime en hexa pero con 8 digitos
                {
                    h = uintToString(va_arg(args, unsigned int), tmp, 16); //base 16
                    for (forindex = 0; forindex < 8 - h; forindex++)
                    {
                        tmp2[forindex] = '0';
                    }
                    strcpy(&tmp2[forindex], tmp);
                    strcpy(&buff[j], tmp2);
                    j += strlen(tmp2);
                    
                    break;
                }
                case 'X': // imprime en hexa pero solo con 2 digitos
                {
                    h = uintToString(va_arg(args, unsigned int), tmp, 16); //base 16
                    // aca me sale una string de len = 1 o 2
                    if( h == 1 ) {
                        tmp2[0] = '0';
                        tmp2[1] = tmp[0];
                        strcpy(&buff[j], tmp2);
                    }
                    else 
                        strcpy(&buff[j], tmp);

                    j += 2;
                    break;
                }
            }
        }
        else
        {
            buff[j] = str[i];
            j++;
        }
        i++;
    }
    _syscall(WRITE,(uint64_t) buff,(uint64_t) 15,0,0);
    va_end(args);
}

int strcmp(char *s1, char *s2)
{
    int i = 0;
    while (s1[i] == s2[i])
    {
        if (s1[i] == '\0' && s2[i] == '\0')
            return 0; 
        i++;
    }
    return s1[i] - s2[i]; 
}

//----------------------------------------------------------------------
// con algunas modificaciones pero nos basamos en la fuente indicada:
//https://www.geeksforgeeks.org/write-your-own-atoi/
int strToInt(char *str)
{
    int len = strlen(str);
    int ans = 0;
    int j = 0;
    int minusFlag = 0;
    if (str[0] == '-')
        minusFlag = 1;
    for (int i = len - 1; i >= 0; i--, j++)
    {
        if (str[i] <= '9' && str[i] >= '0')
        {
            ans += (str[i] - '0') * pow(10, j);
        }
    }
    if (minusFlag)
        ans = -ans;
    return ans;
}
//-------------------------------------------------------------------------------


// los caracteres ya tienen que ser validos cuando se llama a esta funcion
uint64_t hexaStrToDir(char *hexaStr)
{ 
    int len = strlen(hexaStr);
    uint64_t ans = 0;
    int j = 0;
    for (int i = len - 1; i >= 0; i--, j++)
    {
        if (hexaStr[i] <= '9' && hexaStr[i] >= '0')
        {
            ans += (hexaStr[i] - '0') * pow(16, j);
        }
        else if (hexaStr[i] <= 'F' && hexaStr[i] >= 'A')
        {
            ans += (hexaStr[i] - 'A' + 10) * pow(16, j);
        }
    }
    return ans;
}

int pow(int base, int e)
{
    int ans = 1;
    for (int i = 0; i < e; i++)
    {
        ans *= base;
    }
    return ans;
}

void doubleToString(long double result, char *auxBuffer)
{
    
    int isNegative = (result < 0) ? 1 : 0;
    int integer_part = (int)result;
    int i;
    result = (result - integer_part); 
    for(i=0; (int) result == 0 && i <= 8; i++){
        result *= 10;
    }
    i--;
    result *= pow(10,8-i);
    int decimal_part = (int)result; 

    char aux[40];
    if (isNegative == 1)
    {
        decimal_part = -decimal_part;
        if (integer_part == 0)
        {
            *aux = '-';
            int len = numToStr(integer_part, aux + 1, 10);
            aux[++len] = '.';
            for(int j = 0; j < i; j++){
                aux[++len] = '0';
            }
            numToStr(decimal_part, aux + len + 1, 10);
            strcpy(auxBuffer, aux);
            return;
        }
    }
    int len = numToStr(integer_part, aux, 10);
    aux[len] = '.';
    for(int j = 0; j < i; j++){
        aux[++len] = '0';
    }
    numToStr(decimal_part, aux + len + 1, 10);
    strcpy(auxBuffer, aux);
}

int readNumFromLine(char *dest)
{
    auxBuffer[0] = 0;
    currentIdx = 0;
    int c;
    int pointFlag = 0;
    int errorFlag = 0;
    while ((c = getChar()) != '\n')
    {
        if (c == BSPACE)
        {
            if (currentIdx > 0)
            {   
                currentIdx--;
                putChar(BSPACE);
                errorFlag = 0;
            }
        }
        else if (c != 0)
        {
            if (currentIdx < BSIZE - 1)
            {
                if ((c == '-' && currentIdx != 0) || (pointFlag && c == '.'))
                {
                    putChar(c);
                    auxBuffer[currentIdx++] = c;
                    errorFlag = 1;
                }
                if ((c <= '9' && c >= '0') || c == '.' || c == '-')
                {
                    putChar(c);
                    if (c == '.')
                        pointFlag = 1;
                    auxBuffer[currentIdx++] = c;
                }
                else
                {
                    putChar(c);
                    auxBuffer[currentIdx++] = c;
                    errorFlag = 1;
                }
            }
        }
    }
    if (errorFlag == 1)
        return -1;
    if (currentIdx == 0)
        return -1;
    putChar('\n');
    auxBuffer[currentIdx++] = '\0';
    strcpy(dest, auxBuffer);
    return currentIdx;
}

//Copia del destino HASTA el punto
int strcpyTilPoint(char *destination, const char *source)
{
    int i = 0;
    while (*source != '\0' && *source != '.')
    {
        *destination = *source;
        destination++;
        source++;
        i++;
    }
    *destination = '\0';
    return i;
}

void stringToDouble(char *string, long double *num)
{
    char intPart[20];
    char doublePart[20] = {0};

    int idxTilPoint = strcpyTilPoint(intPart, string);
    if (idxTilPoint != strlen(string))
    {
        strcpy(doublePart, string + idxTilPoint + 1);
    }

    int ipart, dpart;

    ipart = strToInt(intPart);
    dpart = strToInt(doublePart);
    if(ipart < 0)
        *num = ipart - (long double)dpart / pow(10, strlen(doublePart));
    else {
        *num = ipart + (long double)dpart / pow(10, strlen(doublePart));
    }
}

void printUser(char *name)
{
    _syscall(WRITE,(uint64_t) name, (uint64_t) 13,0,0);
}

void printError(char *err)
{
    _syscall(WRITE,(uint64_t) err, (uint64_t)4,0,0);
}

void printTitle(char *title)
{
    _syscall(WRITE,(uint64_t)title,14,0,0);
}

uint32_t uintToString(uint64_t value, char * buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

void * malloc(size_t size) 
{
    return (void * ) _syscall(MALLOC, (uint64_t)size, 0, 0, 0);
}

void free(void * dir)
{
    _syscall(FREE, (uint64_t) dir, 0,0,0);
}