#
# Makefile for LOMBOTECH viss drivers
#

lombo-viss-vic-objs := viss-vic.o
lombo-viss-mipi-csi-objs:= viss-mipi-csi.o
lombo-viss-is-objs := viss-isp.o viss-isp-preview.o
lombo-viss-objs := media-dev.o viss-m2m.o

ifeq ($(CONFIG_ARCH_LOMBO_N7V0), y)
SOURCE := $(srctree)/$(src)/n7v0/mipi_csi/mipi_csi_csp.c
ifneq ($(wildcard $(SOURCE)), )
lombo-viss-mipi-csi-objs += n7v0/mipi_csi/
else
lombo-viss-mipi-csi-objs += liblombo_mipi_csi.o
obj-$(CONFIG_VIDEO_LOMBO_MIPI_CSI) += liblombo_mipi_csi.o
$(obj)/liblombo_mipi_csi.o: $(src)/liblombo_mipi_csi
	cp $< $@
endif

SOURCE := $(srctree)/$(src)/n7v0/vic/vic_csp.c
ifneq ($(wildcard $(SOURCE)), )
lombo-viss-vic-objs += n7v0/vic/
else
lombo-viss-vic-objs += liblombo_vic.o
$(obj)/liblombo_vic.o: $(src)/liblombo_vic
	cp $< $@
endif

SOURCE := $(srctree)/$(src)/n7v0/isp/isp_csp.c
ifneq ($(wildcard $(SOURCE)), )
lombo-viss-is-objs += n7v0/isp/
else
lombo-viss-is-objs += liblombo_isp.o
$(obj)/liblombo_isp.o: $(src)/liblombo_isp
	cp $< $@
endif

SOURCE := $(srctree)/$(src)/n7v0/viss_top/viss_top_csp.c
ifneq ($(wildcard $(SOURCE)), )
lombo-viss-objs += n7v0/viss_top/
else
lombo-viss-objs += liblombo_viss_top.o
$(obj)/liblombo_viss_top.o: $(src)/liblombo_viss_top
	cp $< $@
endif


else ifeq ($(CONFIG_ARCH_LOMBO_N7V1), y)
SOURCE := $(srctree)/$(src)/n7v1/mipi_csi/mipi_csi_csp.c
ifneq ($(wildcard $(SOURCE)), )
lombo-viss-mipi-csi-objs += n7v1/mipi_csi/
else
lombo-viss-mipi-csi-objs += liblombo_mipi_csi.o
obj-$(CONFIG_VIDEO_LOMBO_MIPI_CSI) += liblombo_mipi_csi.o
$(obj)/liblombo_mipi_csi.o: $(src)/liblombo_mipi_csi
	cp $< $@
endif

SOURCE := $(srctree)/$(src)/n7v1/vic/vic_csp.c
ifneq ($(wildcard $(SOURCE)), )
lombo-viss-vic-objs += n7v1/vic/
else
lombo-viss-vic-objs += liblombo_vic.o
$(obj)/liblombo_vic.o: $(src)/liblombo_vic
	cp $< $@
endif

SOURCE := $(srctree)/$(src)/n7v1/isp/isp_csp.c
ifneq ($(wildcard $(SOURCE)), )
lombo-viss-is-objs += n7v1/isp/
else
lombo-viss-is-objs += liblombo_isp.o
$(obj)/liblombo_isp.o: $(src)/liblombo_isp
	cp $< $@
endif

SOURCE := $(srctree)/$(src)/n7v1/viss_top/viss_top_csp.c
ifneq ($(wildcard $(SOURCE)), )
lombo-viss-objs += n7v1/viss_top/
else
lombo-viss-objs += liblombo_viss_top.o
$(obj)/liblombo_viss_top.o: $(src)/liblombo_viss_top
	cp $< $@
endif

else ifeq ($(CONFIG_ARCH_LOMBO_N9V0), y)
SOURCE := $(srctree)/$(src)/n9v0/mipi_csi/mipi_csi_csp.c
ifneq ($(wildcard $(SOURCE)), )
lombo-viss-mipi-csi-objs += n9v0/mipi_csi/
else
lombo-viss-mipi-csi-objs += liblombo_mipi_csi.o
obj-$(CONFIG_VIDEO_LOMBO_MIPI_CSI) += liblombo_mipi_csi.o
$(obj)/liblombo_mipi_csi.o: $(src)/liblombo_mipi_csi
	cp $< $@
endif

SOURCE := $(srctree)/$(src)/n9v0/vic/vic_csp.c
ifneq ($(wildcard $(SOURCE)), )
lombo-viss-vic-objs += n9v0/vic/
else
lombo-viss-vic-objs += liblombo_vic.o
$(obj)/liblombo_vic.o: $(src)/liblombo_vic
	cp $< $@
endif

SOURCE := $(srctree)/$(src)/n9v0/isp/isp_csp.c
ifneq ($(wildcard $(SOURCE)), )
lombo-viss-is-objs += n9v0/isp/
else
lombo-viss-is-objs += liblombo_isp.o
$(obj)/liblombo_isp.o: $(src)/liblombo_isp
	cp $< $@
endif

SOURCE := $(srctree)/$(src)/n9v0/viss_top/viss_top_csp.c
ifneq ($(wildcard $(SOURCE)), )
lombo-viss-objs += n9v0/viss_top/
else
lombo-viss-objs += liblombo_viss_top.o
$(obj)/liblombo_viss_top.o: $(src)/liblombo_viss_top
	cp $< $@
endif


endif

obj-$(CONFIG_VIDEO_LOMBO_VIC)		+= $(lombo-viss-vic-objs)
obj-$(CONFIG_VIDEO_LOMBO_MIPI_CSI)	+= $(lombo-viss-mipi-csi-objs)
obj-$(CONFIG_VIDEO_LOMBO_ISP)		+= $(lombo-viss-is-objs)
obj-$(CONFIG_VIDEO_LOMBO_VISS)		+= $(lombo-viss-objs)

obj-$(CONFIG_VIDEO_LOMBO_CCCORE)	+= cccore/

#ccflags-y := -DDEBUG
