#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <scheduler.h>
#include <semManager.h>
#include <fdManager.h>
extern char _keyPressed();
extern int _getKey();
void keyboard_handler();
char getAscii(unsigned int key);
void removeBuffer();
unsigned char * getBuffer();
void initKeyboard(memoryManagerADT memMan, schedulerADT sch);
#endif
