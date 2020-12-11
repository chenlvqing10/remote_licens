#
# Makefile for LOMBOTECH sdc host controller drivers
#

obj-$(CONFIG_ARCH_LOMBO)	+= sdc.o
ifeq ($(CONFIG_ARCH_LOMBO_N7), y)
SOURCE := $(srctree)/$(src)/n7/sdc_csp.c
ifneq ($(wildcard $(SOURCE)), )
obj-$(CONFIG_ARCH_LOMBO_N7) += n7/
else
obj-y += liblombo_sdc.o
$(obj)/liblombo_sdc.o: $(src)/liblombo_sdc
	cp $< $@
endif
else ifeq ($(CONFIG_ARCH_LOMBO_N9), y)
SOURCE := $(srctree)/$(src)/n9/sdc_csp.c
ifneq ($(wildcard $(SOURCE)), )
obj-$(CONFIG_ARCH_LOMBO_N9) += n9/
else
obj-y += liblombo_sdc.o
$(obj)/liblombo_sdc.o: $(src)/liblombo_sdc
	cp $< $@
endif
endif

#ccflags-y := -DDEBUG
