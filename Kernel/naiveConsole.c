#include <naiveConsole.h>
#define STD_OUT 0
#define STD_ERR 1
#define B_SPACE -10        // chequear si este valor está bien
#define CURSOR 222

void ncPrint(const char * string, int color)
{
	for (int i = 0; string[i] != 0; i++)
	{
		ncPrintChar(string[i], color);
	}
}

void ncPrintChar(char character, int color)
{
	if(character == '\n')
		ncNewline();
	else if(character == B_SPACE)
		clearLastAscii();
	else {
		writeAscii(character, color);
		writeAscii(CURSOR, 15);
	}
}

// provisto por la catedra
uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base)
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
	} while (value /= base);

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

void intToHexaStr(char *buff)
{
	char auxStr[9] = "0x000000";
	int dim = 0;
	for (int i = 0; buff[i] != 0; i++)
	{
		dim++;
	}
	for (int j = 7; j >= 0 && dim != 0; j--)
	{
		auxStr[j] = buff[dim - 1];
		dim--;
	}
	for (int k = 0; k < 8; k++)
	{
		buff[k] = auxStr[k];
	}
}


void ncNewline()
{
	newLine();
}

void ncClear()
{
	clearScreen();
}
