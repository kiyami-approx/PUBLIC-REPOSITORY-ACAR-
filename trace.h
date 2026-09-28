#ifndef __TRACE_H__
#define __TRACE_H__

#include "gpio.h"
#include "speed.h"
#include "delay_us.h"

extern volatile uint8_t g_force_stop;

void Track_Follow_Oval(void);
void Track_Follow_Square(void);

#endif
