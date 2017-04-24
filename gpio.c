/* Copyright (c) 2011, RidgeRun
 * All rights reserved.
 *
 * Contributors include:
 *   Todd Fischer
 *   Brad Lu
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the RidgeRun.
 * 4. Neither the name of the RidgeRun nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY RIDGERUN ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL RIDGERUN BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "gpio.h"

#define ERR(source) 	(perror(source),\
                     	fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     	exit(EXIT_FAILURE))
#define SYSFS_GPIO_DIR 	"/sys/class/gpio"
#define MAX_BUF 		64
#define MAX_VAL 		2

void gpio_export(unsigned pin)
{
	int fd, len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) {
		ERR("gpio_export");
	}

	len = snprintf(buf, MAX_BUF, "%d", pin);
	write(fd, buf, len);
	close(fd);
}

void gpio_unexport(unsigned pin)
{
	int fd, len;
	char buf[MAX_BUF];
	
	fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
	if (fd < 0) {
		ERR("gpio_unexport");
	}

	len = snprintf(buf, MAX_BUF, "%d", pin);
	write(fd, buf, len);
	close(fd);
}

void gpio_set_dir(unsigned pin, char *dir)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, MAX_BUF, SYSFS_GPIO_DIR "/gpio%d/direction", pin);
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		ERR("gpio_set_dir");
	}

	write(fd, dir, strlen(dir) + 1);
	close(fd);
}

void gpio_set_edge(unsigned pin, char *edge)
{
	int fd;	
	char buf[MAX_BUF];

	snprintf(buf, MAX_BUF, SYSFS_GPIO_DIR "/gpio%d/edge", pin);
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		ERR("gpio_set_edge");
	}

	write(fd, edge, strlen(edge) + 1);
	close(fd);
}

void gpio_set_value(unsigned pin, unsigned int value)
{
	int fd, len;
	char val[MAX_VAL], buf[MAX_BUF];

	len = snprintf(buf, MAX_BUF, SYSFS_GPIO_DIR "/gpio%d/value", pin);
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		ERR("gpio_set_value");
	}

	len = snprintf(val, MAX_VAL, "%d", value);
	write(fd, val, len); 
	close(fd);
}

void gpio_get_value(unsigned pin, unsigned int *value)
{
	int fd;
	char val[MAX_VAL], buf[MAX_BUF];

	snprintf(buf, MAX_BUF, SYSFS_GPIO_DIR "/gpio%d/value", pin);
	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		ERR("gpio_get_value");
	}

	read(fd, val, MAX_VAL);
	*value = *val - '0';
	close(fd);
}

void gpio_open_value(unsigned pin, int *filedes)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, MAX_BUF, SYSFS_GPIO_DIR "/gpio%d/value", pin);
	fd = open(buf, O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		ERR("gpio_get_value");
	}
	
	*filedes = fd;
}

void gpio_read_value(int filedes, unsigned int *value)
{
	char val[MAX_VAL];
	
	lseek(filedes, 0, SEEK_SET);
	read(filedes, val, MAX_VAL);
	lseek(filedes, 0, SEEK_END);
	*value = *val - '0';
}

void gpio_close_value(int fd)
{
	close(fd);
}
