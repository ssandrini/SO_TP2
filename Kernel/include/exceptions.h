#ifndef __EXCEPTIONS_H_
#define __EXCEPTIONS_H_

#include <stdint.h>

#define B_SPACE -10

void exceptionDispatcher(uint64_t exception, uint64_t * stackFrame);
void zero_division();
void invalid_operation();
void backAddresses(uint64_t * ip, uint64_t * rsp);
void registerPrint(uint64_t * stackFrame);
extern void _sti();
extern void _hlt();
#endif