obj-y += lombo_rtc_power.o

ifeq ($(CONFIG_ARCH_LOMBO_N7), y)
SOURCE := $(srctree)/$(src)/csp/n7/rtc_power_operate.c
endif

ifeq ($(CONFIG_ARCH_LOMBO_N9), y)
SOURCE := $(srctree)/$(src)/csp/n9/rtc_power_operate.c
endif

ifneq ($(wildcard $(SOURCE)), )
obj-$(CONFIG_ARCH_LOMBO_N7) += csp/n7/
obj-$(CONFIG_ARCH_LOMBO_N9) += csp/n9/
else
obj-y += liblombo_rtc.o
$(obj)/liblombo_rtc.o: $(src)/liblombo_rtc
	cp $< $@
endif
