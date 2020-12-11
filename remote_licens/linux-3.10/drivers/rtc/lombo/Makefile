#
# lombo rtc Makefile
#

obj-$(CONFIG_ARCH_LOMBO_N7)	+= lombo_n7_rtc_power.o n7_rtc.o
obj-$(CONFIG_ARCH_LOMBO_N9)	+= n9_rtc.o

ifeq ($(CONFIG_ARCH_LOMBO_N7), y)
SOURCE := $(srctree)/$(src)/csp/n7/rtc_power_operate.c rtc_csp.c
endif

ifeq ($(CONFIG_ARCH_LOMBO_N9), y)
SOURCE := $(srctree)/$(src)/csp/n9/rtc_csp.c
endif

ifneq ($(wildcard $(SOURCE)), )
obj-$(CONFIG_ARCH_LOMBO_N7) += csp/n7/
obj-$(CONFIG_ARCH_LOMBO_N9) += csp/n9/
else
obj-y += liblombo_rtc.o
$(obj)/liblombo_rtc.o: $(src)/liblombo_rtc
	cp $< $@
endif
