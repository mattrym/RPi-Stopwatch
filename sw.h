#ifndef _SW_H
#define _SW_H

#define SW_LNUM				4
#define SW_BNUM				3
#define SW_PRESSED			'0'

#define SW_LED_START	 	GPIO_LED_3
#define SW_LED_STOP	 	GPIO_LED_1
#define SW_LED_LAP		GPIO_LED_2
#define SW_LED_RESET 		GPIO_LED_4

#define SW_BTN_RESET 		GPIO_BTN_TOP
#define SW_BTN_ONOFF 		GPIO_BTN_RIGHT
#define SW_BTN_LAP 		GPIO_BTN_LEFT

void sw_init();
void sw_tick();
void sw_fin();

void sw_onoff();
void sw_lap();
void sw_reset(); 

#endif
