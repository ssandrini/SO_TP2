#ifndef __COMMANDS_H__
#define __COMMANDS_H__
#include <stdfun.h>
#include <stdint.h>
#include <syscalls.h>
#include "filter.h"
#include "loop.h"
#include "wc.h"
#include "phylo.h"
#include "cat.h"
#include <tests.h>
#define NAME_MAX 20
#define INFO_MAX 150
#define BUILTIN_SIZE 2
#define APPS_SIZE 22
#define MAX_ARGS 5

#define CASE_LOOP 0
#define CASE_CAT 1
#define CASE_WC 2
#define CASE_FILTER 3
#define CASE_PHYLO 4
#define CASE_INFOREG 5
#define CASE_PRINTMEM 6
#define CASE_CPUID 7
#define CASE_TIME 8
#define CASE_MEM 9
#define CASE_PS 10
#define CASE_KILL 11
#define CASE_NICE 12
#define CASE_BLOCK 13
#define CASE_SEM 14
#define CASE_PIPE 15
#define CASE_TEST_MM 16
#define CASE_TEST_PROCESSES 17
#define CASE_TEST_PRIO 18
#define CASE_TEST_SYNC 19
#define CASE_TEST_NOSYNC 20
#define CASE_TEST_PIPES 21

#define SPECIAL_FEATURES_ID 7
#define FEATURES_ID 1

// --------------------------------------COMMANDS------------------------------------------//
void help(); 
void inforeg(int argc, char ** argv);
void getMem(int argc, char ** argv);
void getTime(int argc, char ** argv);
void clear();
void cpuid(int argc, char ** argv);
void mem(int argc, char ** argv);
void ps(int argc, char ** argv);
void sem(int argc, char ** argv);
void pipe(int argc, char ** argv);
void kill(int argc, char ** argv);
void nice(int argc, char ** argv);
void block(int argc, char ** argv);
//---------------------------------------AUX FUNCTIONS---------------------------------------
int checkCommandBuiltIn(char * buffer);
int checkCommandUserApps(char * buffer);
int prepareArgs(char token, char **argv, char *input);
int checkTests(char *buffer, char *parameter);
#endif
    