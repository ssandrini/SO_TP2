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
    _syscall(WRITE, (uint64_t) toPrint, (uint64_t) 15,0,0,0);
}

char getChar()
{
    char k = 0;
    _syscall(READ,(uint64_t) &k,(uint64_t) 1,0,0,0);
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
    _syscall(WRITE,(uint64_t) buff,(uint64_t) 15,0,0,0);
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

void printUser(char *name)
{
    _syscall(WRITE,(uint64_t) name, (uint64_t) 13,0,0,0);
}

void printError(char *err)
{
    _syscall(WRITE,(uint64_t) err, (uint64_t)4,0,0,0);
}

void printTitle(char *title)
{
    _syscall(WRITE,(uint64_t)title,14,0,0,0);
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

void * myMalloc(size_t size) 
{
    return (void * ) _syscall(MALLOC, (uint64_t)size, 0, 0, 0,0);
}

void myFree(void * dir)
{
    _syscall(FREE, (uint64_t) dir, 0,0,0,0);
}

uint64_t my_sem_create(uint64_t initialValue)
{
    return _syscall(CREATE_SEM, initialValue, 0, 0, 0, 0);
}
uint64_t my_sem_open(uint64_t sem_id)
{
    return _syscall(OPEN_SEM, sem_id, 0, 0, 0, 0);
}

uint64_t my_sem_wait(uint64_t sem_id)
{
    return _syscall(WAIT_SEM, sem_id, 0, 0, 0, 0);
}

uint64_t my_sem_post(uint64_t sem_id)
{
    return _syscall(POST_SEM, sem_id, 0, 0, 0, 0);
}

uint64_t my_sem_close(uint64_t sem_id)
{
    return _syscall(CLOSE_SEM, sem_id, 0,0,0,0);
}

/*

uint64_t mySemOpen(uint64_t id){
    return _syscall(OPEN_SEM, id, 0,0,0,0);
}

uint64_t mySemCreate(uint64_t value){
    return _syscall(CREATE_SEM, value, 0,0,0,0);
}

void mySemClose(uint64_t id){
    _syscall(CLOSE_SEM, id, 0, 0, 0, 0);
}

void mySemWait(uint64_t id){
    _syscall(WAIT_SEM, id, 0, 0,0,0);
}

void mySemPost(uint64_t id){
    _syscall(POST_SEM, id, 0, 0,0,0);
}
*/