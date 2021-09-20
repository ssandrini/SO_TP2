#ifndef NAIVE_CONSOLE_H
#define NAIVE_CONSOLE_H

#include <videoDriver.h>

void ncPrint(const char * string, int color);
void ncPrintChar(char character, int color);
void intToHexaStr(char *buff);
uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base);
void ncNewline();
void ncClear();
#endif