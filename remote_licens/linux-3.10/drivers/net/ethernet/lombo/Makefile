#
# Makefile for the lombo gmac drivers.
#

obj-y += gmac.o
SOURCE := $(srctree)/$(src)/csp/n7/gmac_csp.c
ifneq ($(wildcard $(SOURCE)), )
obj-$(CONFIG_ARCH_LOMBO_N7) += csp/n7/
else
obj-y += liblombo_gmac.o
$(obj)/liblombo_gmac.o: $(src)/liblombo_gmac
	cp $< $@
endif
