#ifndef __SYSHANDLER_H__
#define __SYSHANDLER_H__
#include <stdint.h>
#include <naiveConsole.h>
#include <keyboard.h>
#include <memoryManager.h>
extern int _RTC(int num);
extern void _getRegisters(uint64_t * regs);
extern void _cpuid(uint32_t * r1,uint32_t * r2, int * id);
extern int _cpuidSupport();
extern void _hlt();
void sysHandler(uint64_t sysNumber, uint64_t r1, uint64_t r2, uint64_t r3, uint64_t rsp) ;
void read(unsigned char * r1, unsigned int r2);
void getTimeRTC(uint64_t r1, uint64_t r2);
void getReg(uint64_t * r1, uint64_t * stackFrame);
void getMem(uint8_t * dir, uint8_t * vec);
void getInfo(uint32_t * r1, uint32_t * r2, int * id);
void initSysHandler(memoryManagerADT mm);
#endif