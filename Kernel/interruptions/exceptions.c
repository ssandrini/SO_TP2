// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <naiveConsole.h>
#include <exceptions.h>
#include <time.h>

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPERATION_ID 6

// valores de retorno de la excepcion
uint64_t *ipReturn;
uint64_t *rspReturn;

void exceptionDispatcher(uint64_t exception, uint64_t *stackFrame)
{
	// por ahora manejamos estas dos excepciones
	if (exception == (uint64_t)0)
	{
		zero_division();
	}
	else if (exception == INVALID_OPERATION_ID)
		invalid_operation();
	// en todas se imprimen los registros y se resetea
	registerPrint(stackFrame);
	// para resetear cambiamos aca los valoress de los registros
	stackFrame[0xF] = (uint64_t)ipReturn;
	stackFrame[0x12] = (uint64_t)rspReturn;
	return;
}

// setea los registros de retorno
void backAddresses(uint64_t *ip, uint64_t *rsp)
{
	ipReturn = ip;
	rspReturn = rsp;
}

void invalid_operation()
{
	ncPrint("Invalid code of operation", 4);
	ncNewline();
}

void zero_division()
{
	ncPrint("Zero division error ", 4);
	ncNewline();
}

void registerPrint(uint64_t *stackFrame)
{
	// TUVE QUE COMENTAR TODO ESTO PORQUE NO TENEMOS DELETECURSOR Y ESO
	
	char buffer[9];
	static const char *registersName[] = {"R15:   ", "R14:   ", "R13:   ", "R12:   ", "R11:   ", "R10:   ", "R9:    ", "R8:    ", "RSI:   ", "RDI:   ", "RBP:   ", "RDX:   ", "RCX:   ", "RBX:   ", "RAX:   ", "RIP:   ", "CS:    ", "FLAGS: ", "RSP:   "};
	for (int i = 0; i < 19; i++)
	{
		ncPrint(registersName[i], 4);
		uintToBase(stackFrame[i], buffer, 16);
		intToHexaStr(buffer);
		ncPrint(buffer,4);
		if (i > 0 && i % 3 == 0)
		{
			ncNewline();
		}
		else
		{
			ncPrint("  ", 4);
		}
	}
	_sti();
	ncPrint("La pantalla se reiniciara en ", 4);
	char buff[3] = {0};
	_hlt(); // aca lo cortamos hasta que el timer tick nos interrumpa
	int init_time = seconds_elapsed();
	int i = 10;
	uintToBase(i, buff, 10);
	ncPrint(buff, 4);
	//deleteCursor();
	while (i > 0)
	{
		int aux;
		_hlt();
		aux = 10 - (seconds_elapsed() - init_time);
		if (i != aux)
		{
			char bspace[2] = {B_SPACE, 0};
			ncPrint(bspace, 4);
			if (i == 10)
			{
				ncPrint(bspace, 4);
			}
			i = aux;
			uintToBase(i, buff, 10);
			ncPrint(buff, 4);
			//deleteCursor();
		}
	}
	ncNewline();
	
	
}
