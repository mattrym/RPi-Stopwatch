#include <time.h>
#include "gpio.h"

#ifndef _SW_H
#define _SW_H

#define SW_LNUM				4
#define SW_BNUM				3

#define SW_PRESSED			'0'

unsigned int sw_state;

void sw_init();
void sw_tick();
void sw_fin();

void sw_onoff();
void sw_lap();
void sw_reset(); 

#endif
