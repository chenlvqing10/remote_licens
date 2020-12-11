#
# Makefile for the lombo spi drivers.
#

obj-y += lombo_spi.o

ifeq ($(CONFIG_ARCH_LOMBO_N7), y)
SOURCE := $(srctree)/$(src)/csp/n7/spi_operate.c
endif

ifeq ($(CONFIG_ARCH_LOMBO_N9), y)
SOURCE := $(srctree)/$(src)/csp/n9/spi_operate.c
endif

ifneq ($(wildcard $(SOURCE)), )
obj-$(CONFIG_ARCH_LOMBO_N7) += csp/n7/
obj-$(CONFIG_ARCH_LOMBO_N9) += csp/n9/
else
obj-y += liblombo_spi.o
$(obj)/liblombo_spi.o: $(src)/liblombo_spi
	cp $< $@
endif
