#include <naiveConsole.h>
#define STD_OUT 0
#define STD_ERR 1
#define B_SPACE -10        // chequear si este valor está bien

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
