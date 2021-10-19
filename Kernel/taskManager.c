#include <taskManager.h>

schedulerADT scheduler;

void initTaskManager(schedulerADT sch) 
{
    scheduler = sch;
}
uint64_t taskManager(uint64_t currentRsp)
{
    return nextProcess(scheduler, currentRsp);
}