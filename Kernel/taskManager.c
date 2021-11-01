// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <taskManager.h>

static schedulerADT scheduler;

void initTaskManager(schedulerADT sched) 
{
    scheduler = sched;
}
void * taskManager(void * currentRsp)
{
    return nextProcess(scheduler, currentRsp);
}