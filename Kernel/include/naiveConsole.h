#ifndef NAIVE_CONSOLE_H
#define NAIVE_CONSOLE_H

#include <videoDriver.h>

void ncPrint(const char * string, int color);
void ncPrintChar(char character, int color);
void ncNewline();
void ncClear();
#endif