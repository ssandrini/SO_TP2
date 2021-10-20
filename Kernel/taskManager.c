#include <taskManager.h>

schedulerADT scheduler;

void initTaskManager(schedulerADT sch) 
{
    scheduler = sch;
}
uint64_t taskManager(void * currentRsp)
{
    return nextProcess(scheduler, currentRsp);
}