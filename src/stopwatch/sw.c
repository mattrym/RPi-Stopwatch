#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "gpio.h"
#include "sw.h"

#define ERR(source) 		(perror(source),\
                     		 fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     		 exit(EXIT_FAILURE))

#define SW_LED_START	 	GPIO_LED_3
#define SW_LED_STOP		 	GPIO_LED_1
#define SW_LED_LAP			GPIO_LED_2
#define SW_LED_RESET 		GPIO_LED_4

#define SW_BTN_RESET 		GPIO_BTN_TOP
#define SW_BTN_ONOFF 		GPIO_BTN_RIGHT
#define SW_BTN_LAP 			GPIO_BTN_LEFT

#define SW_STOPPED			0
#define SW_STARTED			1

#define SW_BLINK_SEC		0
#define SW_BLINK_NSEC 		100000000

static const int SW_LEDS[SW_LNUM] = { 
	SW_LED_START, 
	SW_LED_STOP, 
	SW_LED_LAP, 
	SW_LED_RESET 
};
static const int SW_BTNS[SW_BNUM] = { 
	SW_BTN_ONOFF, 
	SW_BTN_LAP, 
	SW_BTN_RESET 
};

unsigned int sw_state;
struct timespec sw_reftime;						// reference time - used for time measurement
unsigned long int sw_totaltime, sw_laptime;		// measured total time and current lap time

void sw_init();
void sw_tick();
void sw_fin();

void sw_onoff();
void sw_lap();
void sw_reset(); 

void sw_update();
void sw_blink(unsigned pin);

/*  SW_INIT
 *	allocates necessarry resources and initializing crucial variables
 */
void sw_init(int *sw_btnfds)
{
	int i;

	for (i = 0; i < SW_LNUM; ++i) {
		gpio_export(SW_LEDS[i]);
		gpio_set_dir(SW_LEDS[i], GPIO_DIR_OUT);
	}
	for (i = 0; i < SW_BNUM; ++i) {
		gpio_export(SW_BTNS[i]);
		gpio_set_dir(SW_BTNS[i], GPIO_DIR_IN);
		gpio_set_edge(SW_BTNS[i], GPIO_EDGE_FALLING);
		gpio_open_value(SW_BTNS[i], &sw_btnfds[i]);
	}

	sw_totaltime = sw_laptime = 0;
	sw_state = SW_STOPPED;
}

/*	SW_TICK
 *	updates the time and caches the last measurement time
 */
void sw_tick()
{
	long time_diff;
	struct timespec sw_ticktime;

	if (sw_state != SW_STARTED) {
		return;
	}

	if (clock_gettime(CLOCK_REALTIME, &sw_ticktime) == -1) {
		ERR("sw_tick");
	}

	time_diff = (sw_ticktime.tv_sec  - sw_reftime.tv_sec)  * 1000 +
				(sw_ticktime.tv_nsec - sw_reftime.tv_nsec) / 1000000;

	sw_totaltime += time_diff;
	sw_laptime 	 += time_diff;

	memcpy(&sw_reftime, &sw_ticktime, sizeof(struct timespec));
	printf("\rtotal time: %02lu:%02lu:%03lu \t lap time: %02lu:%02lu:%03lu", 
			sw_totaltime / 60000, (sw_totaltime / 1000) % 60, sw_totaltime % 1000,
			sw_laptime   / 60000, (sw_laptime   / 1000) % 60, sw_laptime   % 1000);
	fflush(stdout);
}

/*	SW_FIN
 *	disposes resources after finishing the application
 */
void sw_fin(int *sw_btnfds)
{
	int i;

	for (i = 0; i < SW_LNUM; ++i) {
		gpio_unexport(SW_LEDS[i]);
	}
	for (i = 0; i < SW_BNUM; ++i) {
		gpio_close_value(sw_btnfds[i]);
		gpio_unexport(SW_BTNS[i]);
	}
}

/*	SW_ONOFF
 *	toggles the state of stopwatch (ON/OFF) with proper LED setting
 */
void sw_onoff()
{
	if (sw_state == SW_STARTED) {
		sw_tick();
		sw_state = SW_STOPPED;

		gpio_set_value(SW_LED_START, GPIO_VAL_LOW);
		gpio_set_value(SW_LED_STOP, GPIO_VAL_HIGH);
	} else {
		sw_update();
		sw_state = SW_STARTED;

		gpio_set_value(SW_LED_STOP, GPIO_VAL_LOW);
		gpio_set_value(SW_LED_START, GPIO_VAL_HIGH);
	}
}

/*	SW_LAP
 *	measures time for the certain laps (starts stopwatch, if stopped)
 */
void sw_lap()
{
	if (sw_state == SW_STARTED) {
		sw_tick();
	}
	else { 
		sw_onoff();
	}

	printf("\rtotal time: %02lu:%02lu:%03lu \t lap time: %02lu:%02lu:%03lu\n", 
			sw_totaltime / 60000, (sw_totaltime / 1000) % 60, sw_totaltime % 1000,
			sw_laptime   / 60000, (sw_laptime   / 1000) % 60, sw_laptime   % 1000);
	
	sw_laptime = 0;
	sw_update();
	sw_blink(SW_LED_LAP);
}

/*	SW_RESET
 *	resets the stopwatch and starts a new measurement session
 */
void sw_reset()
{
	if (sw_state == SW_STARTED) {
		sw_tick();
	}

	printf("\rtotal time: %02lu:%02lu:%03lu \t lap time: %02lu:%02lu:%03lu\n\n", 
			sw_totaltime / 60000, (sw_totaltime / 1000) % 60, sw_totaltime % 1000,
			sw_laptime   / 60000, (sw_laptime   / 1000) % 60, sw_laptime   % 1000);

	sw_totaltime = 0;
	sw_laptime = 0;

	sw_update();
	sw_blink(SW_LED_RESET);
}

/*	SW_UPDATE
 *	caches the last event (update) time - crucial for handling stopped state
 */
void sw_update()
{
	struct timespec sw_updatetime;

	if (sw_state == SW_STARTED) {
		return;
	}

	if (clock_gettime(CLOCK_REALTIME, &sw_updatetime) == -1) {
		ERR("sw_start");
	}

	memcpy(&sw_reftime, &sw_updatetime, sizeof(struct timespec));
	printf("\rtotal time: %02lu:%02lu:%03lu \t lap time: %02lu:%02lu:%03lu", 
			sw_totaltime / 60000, (sw_totaltime / 1000) % 60, sw_totaltime % 1000,
			sw_laptime   / 60000, (sw_laptime   / 1000) % 60, sw_laptime   % 1000);
	fflush(stdout);
}

/*	SW_BLINK
 *	blinks the LED in a synchronous way
 */
void sw_blink(unsigned pin)
{
	struct timespec stime, rtime;
	stime.tv_sec = SW_BLINK_SEC;
	stime.tv_nsec = SW_BLINK_NSEC;

	gpio_set_value(pin, GPIO_VAL_HIGH);

	while (-1 == nanosleep(&stime, &rtime)) {
		if (EINTR == errno) {
			memcpy(&stime, &rtime, sizeof(struct timespec));
		} else {
			ERR("sw_blink");
		}
	}

	gpio_set_value(pin, GPIO_VAL_LOW);
}
