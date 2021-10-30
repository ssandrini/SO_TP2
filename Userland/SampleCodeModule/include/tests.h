#ifndef __TESTS_H__
#define __TESTS_H__
#include "test_util.h"
#include <syscalls.h>
#include <stdfun.h>
void endless_loop(int argc, char ** argv);
void test_processes(); 
void test_kill();
void test_mm();
extern void _hltuser();
#endif