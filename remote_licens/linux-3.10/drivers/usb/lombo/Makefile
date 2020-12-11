ccflags-$(CONFIG_USB_LOMBO_DEBUG)	:= -DDEBUG

obj-$(CONFIG_USB_LOMBO)			+= lb_hdc.o

lb_hdc-y				:= core.o otg.o common.o
lb_hdc-$(CONFIG_USB_LOMBO_UDC)		+= udc.o
lb_hdc-$(CONFIG_USB_LOMBO_HOST)		+= host.o
lb_hdc-$(CONFIG_USB_LOMBO_DEBUG)	+= db.o

ifneq ($(CONFIG_OF),)
	obj-$(CONFIG_USB_LOMBO)		+= lb_hdc_nx.o
endif
