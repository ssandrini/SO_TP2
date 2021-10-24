#include <sysHandler.h>
#include <naiveConsole.h>

memoryManagerADT memoryMan;

void sysHandler(uint64_t sysNumber, uint64_t r1, uint64_t r2, uint64_t r3, uint64_t rsp)
{
    switch (sysNumber)
    {
    case 0: // sysRead  r1=buffer r2=bytes
        read((unsigned char *)r1, (unsigned int)r2);
        _hlt();
        break;
    case 1: // sysWrite
        ncPrint((const char *)r1, (int)r2);
        break;
    case 2:                 // sysGetTime
        getTimeRTC(r1, r2); // en r1 dia mes año y en r2 horas min seg
        break;
    case 3: // sysGetReg
        getReg((uint64_t *)r1, (uint64_t *)rsp);
        break;
    case 4: // sysGetMem
        getMem((uint8_t *)r1, (uint8_t *)r2);
        break;
    case 5: // sysMalloc 
        allocMem(memoryMan, (size_t) r1);
        break;
    case 6: // sysClearScreen
        ncClear();
        break;
    case 7: // sysGetCpuInfo (en este caso el tercer parametro representa el ID)
        getInfo((uint32_t *)r1, (uint32_t *)r2, (int *)r3);
        break;
    case 8:
        freeMem(memoryMan, (void *) r1);
        break;
    default:
        break;
    }
}

void read(unsigned char *r1, unsigned int r2)
{
    unsigned char *KeyBuffer = getBuffer();
    unsigned int i;
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

void initSysHandler(memoryManagerADT mm)
{
    memoryMan = mm;
}