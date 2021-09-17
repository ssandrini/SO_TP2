#ifndef __IRQDISPATCHER_H__
#define __IRQDISPATCHER_H__
#include <keyboard.h>
#include <stdint.h>
void int_20();
void int_21();
void irqDispatcher(uint64_t irq);

#endif