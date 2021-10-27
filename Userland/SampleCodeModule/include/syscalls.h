#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

#include <stdint.h>
#define READ 0
#define WRITE 1
#define GET_TIME 2
#define GET_REG 3
#define GET_MEM 4
#define MALLOC 5
#define CLEAR_SCREEN 6
#define CPU_INFO 7
#define FREE 8
#define NEW_PROCESS 9
#define KILL_PROCESS 10
#define BLOCK_PROCESS 11
#define UNBLOCK_PROCESS 12
#define NICE 13
#define PS 14

extern uint64_t _syscall(uint64_t sysNumber, uint64_t r1, uint64_t r2, uint64_t r3, uint64_t r4, uint64_t rsp);

#endif