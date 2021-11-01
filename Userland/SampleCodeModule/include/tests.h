#ifndef __TESTS_H__
#define __TESTS_H__
#include "test_util.h"
#include <syscalls.h>
#include <stdfun.h>
void endless_loop(int argc, char ** argv);
void test_processes(int argc, char ** argv); 
void test_mem(int argc, char ** argv);
void test_prio(int argc, char ** argv);
void test_no_sync(int argc, char **argv);
void test_sync(int argc, char **argv);
void endless_loop3();
#endif