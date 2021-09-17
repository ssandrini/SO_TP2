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
	ncPrint("Invalid code of operation", 0xFF0000);
	ncNewLine();
}

void zero_division()
{
	ncPrint("Zero division error ", 0xFF0000);
	ncNewLine();
}

void registerPrint(uint64_t *stackFrame)
{
	// TUVE QUE COMENTAR TODO ESTO PORQUE NO TENEMOS DELETECURSOR Y ESO
	/*
	char buffer[9];
	static const char *registersName[] = {"R15:   ", "R14:   ", "R13:   ", "R12:   ", "R11:   ", "R10:   ", "R9:    ", "R8:    ", "RSI:   ", "RDI:   ", "RBP:   ", "RDX:   ", "RCX:   ", "RBX:   ", "RAX:   ", "RIP:   ", "CS:    ", "FLAGS: ", "RSP:   "};
	for (int i = 0; i < 19; i++)
	{
		ncPrint(registersName[i], 0xFF0000);
		uintToBase(stackFrame[i], buffer, 16);
		intToHexaStr(buffer);
		ncPrint(buffer, 0xFF0000);
		if (i > 0 && i % 3 == 0)
		{
			ncNewLine();
		}
		else
		{
			ncPrint("  ", 0xFF0000);
		}
	}
	_sti();
	ncPrint("La pantalla se reiniciara en ", 0xFF0000);
	char buff[3] = {0};
	_hlt(); // aca lo cortamos hasta que el timer tick nos interrumpa
	int init_time = seconds_elapsed();
	int aux = 10;
	int i = 10;
	uintToBase(i, buff, 10);
	ncPrint(buff, 0xFF0000);
	deleteCursor();
	while (i > 0)
	{
		_hlt();
		aux = 10 - (seconds_elapsed() - init_time);
		if (i != aux)
		{
			char bspace[2] = {B_SPACE, 0};
			ncPrint(bspace, 0xFF0000);
			if (i == 10)
			{
				ncPrint(bspace, 0xFF0000);
			}
			i = aux;
			uintToBase(i, buff, 10);
			ncPrint(buff, 0xFF0000);
			deleteCursor();
		}
	}
	*/
	ncNewLine();
}
