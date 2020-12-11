#
# lombo memctrl specific Makefile
#

obj-y += memctrl.o
SOURCE := $(srctree)/$(src)/csp/csp_memctrl.c
ifneq ($(wildcard $(SOURCE)), )
obj-y += csp/
else
obj-y += liblombo_memctrl.o
$(obj)/liblombo_memctrl.o: $(src)/liblombo_memctrl
	cp $< $@
endif
