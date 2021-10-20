#include <taskManager.h>

schedulerADT scheduler;

void initTaskManager(schedulerADT sched) 
{
    scheduler = sched;
}
void * taskManager(void * currentRsp)
{
    return nextProcess(scheduler, currentRsp);
}