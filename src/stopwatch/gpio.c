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
	
	read(filedes, val, MAX_VAL);
	*value = *val - '0';
}

void gpio_close_value(int fd)
{
	close(fd);
}
