// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <videoDriver.h>

#define DEL 127
#define CURSOR 222

static uint8_t * const video = (uint8_t*)0xB8000;
static uint8_t * currentVideo = (uint8_t*)0xB8000;
static const uint32_t width = 80;
static const uint32_t height = 25 ;

static int getInformation(uint8_t * dir)
{
	return *(dir+1);
}

static void clearLine(uint8_t * p) 
{
	for(int i = 0; i < width; i++)
		p[i * 2] = ' ';
}

static void scroll() 
{
	currentVideo = video;
	uint8_t * aux = video + width * 2;
	while( aux < video + width * 2 * height) 
	{
		writeAscii(*aux, getInformation(aux));
		aux+=2;
	}
	aux -= width * 2;
	currentVideo = aux;
	clearLine(currentVideo);
}

void writeAscii(char character, int color)
{
	if(currentVideo ==  video + width * 2 * height) {
		scroll();
	}
	*currentVideo = character;
	*(currentVideo + 1) = color;
	if(character == (char) DEL)
	{
		currentVideo-=2;
		*currentVideo = character;
		*(currentVideo + 1) = color;
	}
	else if(character != (char) CURSOR)
	{
		currentVideo+=2;
	}
} 

void clearScreen()
{ 
	int i;
	for (i = 0; i < height * width; i++)
		video[i * 2] = ' ';
	currentVideo = video;
}

void newLine()
{
	do
	{
		writeAscii(' ', 0);
	} while((uint64_t)(currentVideo - video) % (width * 2) != 0);
}

void clearLastAscii()
{
	writeAscii(127, 0);
	writeAscii(222,15);
}   
