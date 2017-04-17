#ifndef _GPIO_H
#define _GPIO_H

#define GPIO_VAL_LOW		0
#define GPIO_VAL_HIGH 		1
#define GPIO_DIR_IN  		"in" 
#define GPIO_DIR_OUT 		"out"
#define GPIO_EDGE_RISING	"rising"
#define GPIO_EDGE_FALLING 	"falling"
#define GPIO_EDGE_BOTH		"both"

#define GPIO_LED_1 			5
#define GPIO_LED_2		 	6
#define GPIO_LED_3 			13
#define GPIO_LED_4 			19

#define GPIO_BTN_TOP 		20
#define GPIO_BTN_LEFT 		21
#define GPIO_BTN_RIGHT 		16

void gpio_export(unsigned pin);
void gpio_unexport(unsigned pin);
void gpio_set_dir(unsigned pin, char *dir);
void gpio_set_edge(unsigned pin, char *edge);
void gpio_set_value(unsigned pin, unsigned int value);
void gpio_get_value(unsigned pin, unsigned int *value);
void gpio_open_value(unsigned pin, int *filedes);
void gpio_read_value(int filedes, unsigned int *value);
void gpio_close_value(int fd);

#endif
