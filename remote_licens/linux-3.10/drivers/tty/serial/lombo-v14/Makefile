#
# Makefile for the lombo serial device drivers.
#

obj-y += lombo_uart.o
SOURCE := $(srctree)/$(src)/csp/n7/uart_operate.c
ifneq ($(wildcard $(SOURCE)), )
obj-$(CONFIG_ARCH_LOMBO_N7V0) += csp/n7/
obj-$(CONFIG_ARCH_LOMBO_N7V1) += csp/n7/
else
obj-y += liblombo_uart.o
$(obj)/liblombo_uart.o: $(src)/liblombo_uart
	cp $< $@
endif
