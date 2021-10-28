#ifndef __TESTS_H__
#define __TESTS_H__
#include "test_util.h"
#include <syscalls.h>

void endless_loop(int argc, char ** argv);
int test_processes(); 
int test_kill();
extern void _hltuser();
#endif