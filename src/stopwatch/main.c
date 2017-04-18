#define _GNU_SOURCE
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "gpio.h"
#include "sw.h"

#define ERR(source) (perror(source),\
                     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     exit(EXIT_FAILURE))
#define MAX_BUF		16

#define SW_TIMEOUT_SEC 		0
#define SW_TIMEOUT_NSEC		5000000
#define SW_DEBOUNCE_TIMEOUT 25

volatile sig_atomic_t running = 1;

void sigint_handler(int sig)
{
	running = 0;
}

void sethandler( void (*f)(int), int sigNo) {
	struct sigaction act;
	
	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = f;
	if (sigaction(sigNo, &act, NULL) < 0) {
		ERR("sigaction");
	}
}

int main(int argc, char **argv)
{
	int i, rfd;
	int sw_btnfds[SW_BNUM];				/* GPIO buttons file descriptors */
	char val_buf, buf[MAX_BUF];
	sigset_t mask;
	struct timespec timeout;
	struct pollfd fdset[SW_BNUM], poll_fdset[SW_BNUM], dbnc_fdset[SW_BNUM];
	void (*sw_btnaction[SW_BNUM])(void) = {	sw_onoff, sw_lap, sw_reset };
		/* callbacks for every GPIO button */

	sw_init(sw_btnfds);
	memset((void*)fdset, 0, sizeof(fdset));
	for (i = 0; i < SW_BNUM; ++i) {
		fdset[i].fd = sw_btnfds[i];
		fdset[i].events = POLLPRI;
	}

	sethandler(sigint_handler, SIGINT);
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);

	timeout.tv_sec = SW_TIMEOUT_SEC;
	timeout.tv_nsec = SW_TIMEOUT_NSEC;

	while (running) {
		memcpy(&poll_fdset, &fdset, sizeof(fdset));
		memcpy(&dbnc_fdset, &fdset, sizeof(fdset));

		/* first ppoll - waits until a input is ready or signal is delivered
		 * allows to handle smooth interrupt from keyboard
		 */
		if ((rfd = ppoll(poll_fdset, SW_BNUM, &timeout, &mask)) > 0) {

			/* second poll - debouncing loop, which clears the descriptors
			 * (reads from them till the end) and polls until a timeout
			 * (DEBOUNCE_TIMEOUT) is reached
			 * ppoll is here unnecessarry (since we will not receive any
			 * GPIO state further changes
			 */
			do {
				for (i = 0; i < SW_BNUM; ++i) {
					if (dbnc_fdset[i].revents & POLLPRI) {
						lseek(dbnc_fdset[i].fd, 0, SEEK_SET);
						read(dbnc_fdset[i].fd, buf, MAX_BUF);
					}
				}
				memcpy(&dbnc_fdset, &fdset, sizeof(fdset));
			} while (poll(dbnc_fdset, SW_BNUM, SW_DEBOUNCE_TIMEOUT) > 0);

			/* polls each revents field for a specific received event
			 * and takes a callback action while button was pressed
			 * (not released!)
			 */
			for (i = 0; i < SW_BNUM; ++i) {
				if (poll_fdset[i].revents & POLLPRI) {
					lseek(poll_fdset[i].fd, 0, SEEK_SET);
					read(poll_fdset[i].fd, &val_buf, 1);
					lseek(poll_fdset[i].fd, 0, SEEK_END);

					if (val_buf == SW_PRESSED) {
						(sw_btnaction[i])();
						continue;
					}
				}
			}
		} else if (rfd == 0) {
			/* update the stopwatch info */
			sw_tick();
		} else {
			ERR("sw_work");
		}
	}

	printf("\nTerminated.\n");
	sw_fin(sw_btnfds);
	return EXIT_SUCCESS;
}
