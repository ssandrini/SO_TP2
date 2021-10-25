#ifndef __COMMANDS_H__
#define __COMMANDS_H__
#include <stdfun.h>
#include <stdint.h>
#include <syscalls.h>
#define COMMANDS_SIZE 12
#define CASE_GETMEM 3
#define LENGTH_PRINTMEM 8
#define SPECIAL_FEATURES_ID 7
#define FEATURES_ID 1

// --------------------------------------COMMANDS------------------------------------------//
void help(); 
void inforeg();
void getMem(char * param);
void getTime();
void exc0Trigger();
void exc6Trigger();
void clear();
void quadratic();
void cpuid();
void celsius();
void fahrenheit();
void polar();
//-------------------------------------------------------------------------------------------
//---------------------------------------AUX FUNCTIONS---------------------------------------
int checkCommand(char * buffer, char * parameter);
//-------------------------------------------------------------------------------------------

//---------------------------------------ASM FUNCTIONS----------------------------------------
extern int _quadratic(long double * a, long double * b, long double * c, long double * r1, long double * r2);
extern void _exc6Trigger();
extern void _getReg(uint64_t registers);
extern void _getMem(uint8_t * dir, uint8_t * memory);
extern void _getTime(int * date, int * hours);
extern void _clearScreen();
extern int _getCpuInfo(uint32_t * r1, uint32_t * r2, int * features_id_and_ret);
extern void _FtoCelcius(long double * deg_f, long double * c1, long double * c2 , long double * res);
extern void _CtoFahren(long double * deg_c, long double * c1, long double * c2 , long double * res);
extern void _rectToPolar(long double * x, long double * y, long double * mod, long double * angle, long double * c);
//---------------------------------------------------------------------------------------------

#endif
    