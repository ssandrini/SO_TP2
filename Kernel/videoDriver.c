#include <videoDriver.h>
#include <font8x16.h>

// TODAS LAS FUNCIONES HECHAS EN ESTE DRIVER ESTAN BASADAS EN LA WIKI QUE NOS RECOMENDO LA CATEDRA
// FUENTES : https://wiki.osdev.org/VESA_Video_Modes
// 			 https://wiki.osdev.org/User:Omarrx024/VESA_Tutorial
//			 https://wiki.osdev.org/Drawing_In_Protected_Mode#Drawing_Text
struct infoStructure
{
	uint16_t attributes;  // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;	  // deprecated
	uint8_t window_b;	  // deprecated
	uint16_t granularity; // deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr; // deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;		   // number of bytes per horizontal line
	uint16_t width;		   // width in pixels
	uint16_t height;	   // height in pixels
	uint8_t w_char;		   // unused...
	uint8_t y_char;		   // ...
	uint8_t planes;
	uint8_t bpp;   // bits per pixel in this mode
	uint8_t banks; // deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size; // deprecated; size of a bank, almost always 64 KB but may be LETTER_HEIGHT KB...
	uint8_t image_pages;
	uint8_t reserved0;

	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;

	uint32_t framebuffer; // physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size; // size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__((packed));

struct infoStructure *screenInfo = (struct infoStructure *)0x5C00;

char *getDataPosition(int x, int y)
{
	return (char *)screenInfo->framebuffer + (x + WIDTH * y) * 3;
}

int getPixelColor(int x, int y)
{
	char *pos = getDataPosition(x, y);
	return ((pos[2] & 0xff) << 16) + ((pos[1] & 0xff) << 8) + (pos[0] & 0xff);
}

void writePixel(int x, int y, int rgb_hexa)
{
	char *pos = getDataPosition(x, y);
	pos[0] = rgb_hexa & 0xFF;		  // Blue
	pos[1] = (rgb_hexa >> 8) & 0xFF;  // Green
	pos[2] = (rgb_hexa >> 16) & 0xFF; // Red
}

void writeLetter(int key, int posX, int posY, int letter_color)
{
	char *bitmap = font8x16[key];
	int x, y;
	int set1, set2;
	for (y = 0; y < LETTER_WIDTH; y++)
	{
		for (x = 0; x < LETTER_WIDTH; x++)
		{
			set1 = bitmap[x] & 1 << y;
			set2 = bitmap[x + LETTER_WIDTH] & 1 << y;
			if (set1)
			{
				writePixel(posX + x, posY + y, letter_color);
			}
			else
			{
				writePixel(posX + x, posY + y, BACKGROUND_COLOR);
			}
			if (set2)
			{
				writePixel(posX + x, posY + y + LETTER_WIDTH, letter_color);
			}
			else
			{
				writePixel(posX + x, posY + y + LETTER_WIDTH, BACKGROUND_COLOR);
			}
		}
	}
}

void drawLine(int y)
{
	for (int i = 0; i < WIDTH; i++)
	{
		writePixel(i, y, 0xFF0080);
	}
}

// pinta una linea de negro
void clearLine(int y_initial)
{
	for (int j = 0; j < WIDTH; j++)
	{
		writePixel(j, y_initial, BACKGROUND_COLOR);
	}
}