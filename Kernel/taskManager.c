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