#
# Makefile for the lombo watchdog drivers.
#

obj-$(CONFIG_ARCH_LOMBO_N7) += lombo_wdt_n7.o
obj-$(CONFIG_ARCH_LOMBO_N9) += lombo_wdt_n9.o

ifeq ($(CONFIG_ARCH_LOMBO_N7), y)
SOURCE := $(srctree)/$(src)/csp/n7/wdt_operate.c
endif

ifeq ($(CONFIG_ARCH_LOMBO_N9), y)
SOURCE := $(srctree)/$(src)/csp/n9/wdt_operate.c
endif

ifneq ($(wildcard $(SOURCE)), )
obj-$(CONFIG_ARCH_LOMBO_N7) += csp/n7/
obj-$(CONFIG_ARCH_LOMBO_N9) += csp/n9/
else
obj-y += liblombo_wdt.o
$(obj)/liblombo_wdt.o: $(src)/liblombo_wdt
	cp $< $@
endif
