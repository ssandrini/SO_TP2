#ifndef __VIDEO_DRIVER_H_
#define __VIDEO_DRIVER_H_
#include <stdint.h>
/*
aca irian los defines de colores, posiciones, ancho, etc
*/

void writeAscii(char character, int color);

void clearScreen();

void newLine();

void clearLastAscii();


#endif