#include <sysHandler.h>

static schedulerADT scheduler;
static memoryManagerADT memoryManager;

uint64_t sysHandler(uint64_t sysNumber, uint64_t r1, uint64_t r2, uint64_t r3, uint64_t r4, uint64_t rsp)
{
    switch (sysNumber)
    {
    case 0: // sysRead  r1=buffer r2=bytes
        fdRead(getCurrentFdRead(scheduler), (char *) r1, (int) r2);
        //read((unsigned char *)r1, (unsigned int)r2);
        return 0;
        break;
    case 1: // sysWrite
        fdWrite(getCurrentFdWrite(scheduler), (char *) r1, (int) r2);
        //ncPrint((const char *)r1, (int)r2);
        return 0;
        break;
    case 2:                 // sysGetTime
        getTimeRTC(r1, r2); // en r1 dia mes año y en r2 horas min seg
        return 0;
        break;
    case 3: // sysGetReg
        getReg((uint64_t *)r1, (uint64_t *)rsp);
        return 0;
        break;
    case 4: // sysGetMem
        getMem((uint8_t *)r1, (uint8_t *)r2);
        return 0;
        break;
    case 5: // sysMalloc 
        return (uint64_t) allocMem(memoryManager, (size_t) r1);
        break;
    case 6: // sysClearScreen
        ncClear();
        return 0;
        break;
    case 7: // sysGetCpuInfo (en este caso el tercer parametro representa el ID)
        getInfo((uint32_t *)r1, (uint32_t *)r2, (int *)r3);
        return 0;
        break;
    case 8:
        return freeMem(memoryManager, (void *) r1);
        break;
    case 9: // sysCreateProcess(entryPoint, argv, argc, fg)
        return (uint64_t) newProcess(scheduler, 1, (void (*)(int, char **)) r1, (char **) r2, (int) r3, (int) r4, (int*) rsp);
        break;
    case 10: //sysKill(pid);
        if((int)r1 == 1)
        {
            ncPrint("No puedes matar a la shell",9);
            return 1;
        }
        return (uint64_t) killProcess(scheduler, (int) r1);
        break;
    case 11: //sysBLock(pid)
        if((int)r1 == getPid(scheduler))
        {
            ncPrint("No puedes bloquearte a ti mismo",9);
            return 1;
        }
        if(isBlocked(scheduler, (int) r1))
        {
            return (uint64_t) unblockProcess(scheduler, (int) r1);
        }
        else
        {
            return (uint64_t) blockProcess(scheduler, (int) r1);
        }
        break;
    case 12: // POR AHORA ESTA VACIA
        return 0;
        break;
    case 13: //sysNice(pid, priorioty)
        return (uint64_t) setPriority(scheduler, (int) r1, (int) r2);
        break;
    case 14:
        printProcesses(scheduler);
        return 0;
        break;
    case 15:
        yield(scheduler);
        return 0;
        break;
    case 16:
        printStatus(memoryManager);
        return 0;
        break;
    case 17:
        return (uint64_t) getPid(scheduler);
        break;
    case 18: // semCreate(UINT64 value)
        return  semCreate(r1);
        break;
    case 19: // semOPEN(UINT64 ID)
        return  semOpen(r1);
        break;
    case 20: // semClose(UINT64 ID)
        return (uint64_t) semClose(r1);
    case 21: //semWait(uint64_t id)
        return (uint64_t) semWait(r1);
        break;
    case 22: //semPost(uint64_t id)
        return (uint64_t) semPost(r1);
        break;
    case 23:
        semPrint();
        return 0;
        break;
    case 24:
        return (uint64_t) newPipe();
        break;
    case 25:
        return (uint64_t) closePipe((int) r1);
        break;
    case 26:
        return (uint64_t) freePipe((int) r1);
    case 27:
        return (uint64_t) pipeWrite((int) r1, (char *) r2);
        break;
    case 28:
        return (uint64_t) pipeRead((int) r1, (char *) r2, (int) r3);
        break;
    case 29:
        printPipes();
        return 0;
        break;
    default:
        return 0;
        break;
    }
}

void read(unsigned char *r1, unsigned int r2)
{
    unsigned char *KeyBuffer = getBuffer();
    unsigned int i;
    
    /*
    if(KeyBuffer[0] == '\0')
    {
        blockProcess(scheduler,1);
    }
    */
    
    
    r1[0] = 0;
    for (i = 0; KeyBuffer[i] != 0 && i < r2; i++)
    {
        r1[i] = KeyBuffer[i];
    }
    removeBuffer();
    
}

// la manipulacion del formato la sacamos de la fuente que nos dio la catedra:
// https://wiki.osdev.org/CMOS#Format_of_Bytes
void getTimeRTC(uint64_t r1, uint64_t r2)
{
    int *buffer1 = (int *)r1;
    int *buffer2 = (int *)r2;
    buffer1[0] = _RTC(9); //año
    buffer1[1] = _RTC(8); //mes
    buffer1[2] = _RTC(7); //dia
    buffer2[0] = _RTC(4); //hora
    buffer2[1] = _RTC(2); //minuto
    buffer2[2] = _RTC(0); //segundo

    buffer2[2] = (buffer2[2] & 0x0F) + ((buffer2[2] / 16) * 10);
    buffer2[1] = (buffer2[1] & 0x0F) + ((buffer2[1] / 16) * 10);
    buffer2[0] = ((buffer2[0] & 0x0F) + (((buffer2[0] & 0x70) / 16) * 10)) | (buffer2[0] & 0x80);
    buffer1[2] = (buffer1[2] & 0x0F) + ((buffer1[2] / 16) * 10);
    buffer1[1] = (buffer1[1] & 0x0F) + ((buffer1[1] / 16) * 10);
    buffer1[0] = (buffer1[0] & 0x0F) + ((buffer1[0] / 16) * 10);
}

void getReg(uint64_t *registers, uint64_t *stackFrame)
{
    for (int i = 0; i < 19; i++)
    {
        registers[i] = stackFrame[i];
    }
}

void getMem(uint8_t *dir, uint8_t *vec)
{
    for (int i = 0; i < 32; i++)
    {
        vec[i] = dir[i];
    }
}

void getInfo(uint32_t *r1, uint32_t *r2, int *id)
{
    if (_cpuidSupport())
    {
        _cpuid(r1, r2, id);
        *id = 1;
    }
    else
        *id = 0;
}

void initSysHandler(memoryManagerADT mm, schedulerADT sch)
{
    scheduler = sch;
    memoryManager = mm;
}