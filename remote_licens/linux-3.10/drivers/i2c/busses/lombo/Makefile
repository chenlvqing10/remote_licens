#
# Makefile for LOMBOTECH i2c host controller drivers
#

obj-$(CONFIG_ARCH_LOMBO)	+= i2c.o
SOURCE := $(srctree)/$(src)/n7/i2c_csp.c
ifneq ($(wildcard $(SOURCE)), )
obj-$(CONFIG_ARCH_LOMBO_N7) += n7/
obj-$(CONFIG_ARCH_LOMBO_N9) += n7/
else
obj-y += liblombo_i2c.o
$(obj)/liblombo_i2c.o: $(src)/liblombo_i2c
	cp $< $@
endif
