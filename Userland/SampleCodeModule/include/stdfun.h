#ifndef __STDFUN_H__
#define __STDFUN_H__
#include <stdint.h>
#include <sys/types.h>
#include <stdarg.h>
#include <syscalls.h>

void printf(char *str, ...); 
void putChar(const char ascii);
char getChar();
int strlen(const char * string);
void strcpy(char *destination, const char *source);
int numToStr(int value, char *buffer, int base);
char *reverse(char *buffer, int i, int j);
void swap(char *x, char *y);
int strcmp(char * s1, char *s2);
int strToInt(char *str);
uint64_t hexaStrToDir(char *hexaStr);
int pow(int base, int e);
void printUser(char * name);
void printError(char * err);
void printTitle(char * title);
uint32_t uintToString(uint64_t value, char * buffer, uint32_t base);
void * myMalloc(size_t size);
void myFree(void * dir);
uint64_t my_sem_open(uint64_t id);
uint64_t my_sem_create(uint64_t value);
uint64_t my_sem_close(uint64_t id);
uint64_t my_sem_wait(uint64_t id);
uint64_t my_sem_post(uint64_t id);
#endif