#
# DMA Controllers
#

obj-y += lombo_dma.o

ifeq ($(CONFIG_ARCH_LOMBO_N7), y)
SOURCE := $(srctree)/$(src)/csp/n7/dma_operate.c
endif

ifeq ($(CONFIG_ARCH_LOMBO_N9), y)
SOURCE := $(srctree)/$(src)/csp/n9/dma_operate.c
endif

ifneq ($(wildcard $(SOURCE)), )
obj-$(CONFIG_ARCH_LOMBO_N7) += csp/n7/
obj-$(CONFIG_ARCH_LOMBO_N9) += csp/n9/
else
obj-y += liblombo_dma.o
$(obj)/liblombo_dma.o: $(src)/liblombo_dma
	cp $< $@
endif
