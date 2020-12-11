TARGET=gpio_led
OBJS=gpio_led.c
CC=aarch64-linux-gnu-gcc
CFLAGS+=-c -Wall -g
$(TARGET):$(OBJS)
	$(CC) $^ -o $@                  
%.o:%.c          
	$(CC) $^ $(CFLAGS) -o $@
clean:
	$(RM) *.o $(TARGET) -r

