OBJS := gpio.o sw.o main.o 
stopwatch: $(OBJS)
	$(CC) -o stopwatch $(OBJS)
$(OBJS): %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@
.PHONY: clean
clean: 
	rm stopwatch $(OBJS)
