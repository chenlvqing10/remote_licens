# Makefile for vm selftests

# arm-linux-gnueabi-gcc
CC = arm-linux-gnueabi-gcc
#CC = $(CROSS_COMPILE)gcc

CFLAGS += -pthread
CFLAGS += -I../../../../usr/include/

all:
	$(CC) $(CFLAGS) wdog_test_n7.c -o wdog_test_n7

n9:
	$(CC) $(CFLAGS) wdog_test_n9.c -o wdog_test_n9

clean:
	rm -fr ./wdog_test_n7 ./wdog_test_n9
