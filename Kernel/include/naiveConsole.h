#ifndef NAIVE_CONSOLE_H
#define NAIVE_CONSOLE_H

#include <videoDriver.h>

void changeScreen(int screen);
void ncPrint(const char * string, int letter_color);
void ncNewLine();
void ncClear(int cS);
void scroll();
void middleLine();
uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);
void intToHexaStr(char * buff);
void printCursor();
void deleteCursor();
#endif