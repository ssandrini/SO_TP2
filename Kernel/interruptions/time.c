#include <time.h>

static unsigned long ticks = 0;

void timer_handler()
{
	ticks++;
}

int seconds_elapsed()
{
	return ticks / 18;
}