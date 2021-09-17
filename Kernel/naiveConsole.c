#include <naiveConsole.h>

#define S1_START 0
#define S1_END 382
#define S2_START 386
#define S2_END HEIGHT
#define SCREEN_HEIGHT 382 // (HEIGHT / 2) - LINE_HEIGHT
#define LINE_START S1_END + 1
#define LINE_HEIGHT 4

#define STD_OUT 0
#define STD_ERR 1
#define STD_COLOR 0xFFFFFF
#define ERR_COLOR 0xFF0000
#define B_SPACE -10

static int currX[2] = {0, 0};
static int currY[2] = {S1_START, S2_START};
int cS = 0;

void changeScreen(int screen)
{
	deleteCursor();
	cS = screen;
	printCursor();
}

void middleLine()
{
	for (int i = 0; i < LINE_HEIGHT; i++)
	{
		drawLine(LINE_START + 1 + i);
	}
}

void ncPrint(const char *string, int letter_color)
{
	for (int i = 0; string[i] != 0; i++)
	{

		if (string[i] == '\n')
		{
			ncNewLine();
		}
		else if (string[i] == B_SPACE)
		{
			int start = (cS == 0) ? S1_START : S2_START;
			writeLetter(' ', currX[cS], currY[cS], letter_color); // borra el cursor
			writeLetter(' ', currX[cS] - LETTER_WIDTH, currY[cS], letter_color);
			currX[cS] -= LETTER_WIDTH;

			if (currX[cS] == 0 && currY[cS] >= start)
			{
				currX[cS] = WIDTH - LETTER_WIDTH;
				currY[cS] -= LETTER_HEIGHT;
			}
		}
		else
		{
			writeLetter(string[i], currX[cS], currY[cS], letter_color);
			currX[cS] += LETTER_WIDTH;
		}

		if (currX[cS] == WIDTH)
		{
			currX[cS] = 0;
			currY[cS] += LETTER_HEIGHT;
			int end = (cS == 0) ? S1_END : S2_END;
			if (currY[cS] >= end)
			{
				scroll();
				currY[cS] -= LETTER_HEIGHT;
			}
		}
	}
	printCursor();
}

void ncNewLine()
{
	do
	{
		writeLetter(' ', currX[cS], currY[cS], STD_COLOR);
		currX[cS] += LETTER_WIDTH;
	} while (currX[cS] == WIDTH);

	currX[cS] = 0;
	currY[cS] += LETTER_HEIGHT;
	int end = (cS == 0) ? S1_END : S2_END;
	if (currY[cS] >= end)
	{
		scroll();
		currY[cS] -= LETTER_HEIGHT;
	}
}

void scroll()
{
	int start = (cS == 0) ? S1_START : S2_START;
	int end = (cS == 0) ? S1_END : S2_END;

	for (int y = start; y < end - LETTER_HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			int color = getPixelColor(x, y);
			writePixel(x, y, getPixelColor(x, y + LETTER_HEIGHT));
			writePixel(x, y + LETTER_HEIGHT, color);
		}
	}

	for (int i = end - LETTER_HEIGHT; i < end; i++)
	{
		clearLine(i);
	}
}

void ncClear(int cS)
{
	int start = (cS == 0) ? S1_START : S2_START;
	int end = (cS == 0) ? S1_END : S2_END;
	for (int y = start; y < end; y++)
	{
		clearLine(y);
	}
	currX[cS] = 0;
	currY[cS] = start;
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

void printCursor()
{
	for (int i = currX[cS]; i < currX[cS] + LETTER_WIDTH; i++)
	{
		for (int j = currY[cS]; j < currY[cS] + LETTER_HEIGHT; j++)
		{
			writePixel(i, j, 0xFFFFFF);
		}
	}
}

void deleteCursor()
{
	writeLetter(' ', currX[cS], currY[cS], 0); // borra el cursor
}