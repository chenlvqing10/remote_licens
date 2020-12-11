
lombo-drm-y +=	lombo_drv.o lombo_crtc.o lombo_framebuffer.o lombo_fb_sc.o \
		lombo_fb_array.o lombo_subdrv.o
obj-$(CONFIG_DRM_LOMBO) += lombo-drm.o

lombo-dpu-y += lombo_dpu.o csp_dpu_top.o csp_dpu_common.o
obj-$(CONFIG_DRM_LOMBO_DPU_TOP) += lombo-dpu.o

lombo-dc-y += lombo_dc.o lombo_dc_win.o csp_dpu_dc.o
obj-$(CONFIG_DRM_LOMBO_DC) += lombo-dc.o

lombo-ove-y += lombo_ove.o csp_dpu_ove.o
obj-$(CONFIG_DRM_LOMBO_OVE) += lombo-ove.o

lombo-se-y += lombo_se.o csp_dpu_se.o
obj-$(CONFIG_DRM_LOMBO_SE) += lombo-se.o

lombo-scale-y += lombo_scale.o
obj-y += lombo-scale.o

lombo-wb-y += lombo_wb.o csp_dpu_wb.o
obj-$(CONFIG_DRM_LOMBO_WB) += lombo-wb.o

lombo-dit-y += lombo_dit.o csp_dpu_dit.o
obj-$(CONFIG_DRM_LOMBO_DIT) += lombo-dit.o

lombo-rot-y += lombo_rot.o csp_dpu_rot.o
obj-$(CONFIG_DRM_LOMBO_ROT) += lombo-rot.o

lombo-doss-y += lombo_doss.o csp_doss_top.o
obj-$(CONFIG_DRM_LOMBO_DOSS_TOP) += lombo-doss.o

# to support csp_tcon to lib
ifdef CONFIG_ARCH_LOMBO_N7V0
CSP_TCON_SOURCE := $(srctree)/$(src)/csp_tcon/csp_doss_tcon_v1_0.c
else
CSP_TCON_SOURCE := $(srctree)/$(src)/csp_tcon/csp_doss_tcon.c
endif
ifneq ($(wildcard $(CSP_TCON_SOURCE)), )
obj-$(CONFIG_DRM_LOMBO_TCON) += csp_tcon/
else
obj-$(CONFIG_DRM_LOMBO_TCON) += liblombo_tcon.o
$(obj)/liblombo_tcon.o: $(src)/liblombo_tcon
	cp $< $@
endif
lombo-tcon-y += lombo_tcon.o lombo_rgb.o
obj-$(CONFIG_DRM_LOMBO_TCON) += lombo-tcon.o

# to support csp_mipi_dsi to lib
ifdef CONFIG_ARCH_LOMBO_N9V0
CSP_DSI_SOURCE := $(srctree)/$(src)/csp_dsi/csp_doss_mipi_dsi_v12.c
else
CSP_DSI_SOURCE := $(srctree)/$(src)/csp_dsi/csp_doss_mipi_dsi.c
endif

ifneq ($(wildcard $(CSP_DSI_SOURCE)), )
obj-$(CONFIG_DRM_LOMBO_MIPI_DSI) += csp_dsi/
else
obj-$(CONFIG_DRM_LOMBO_MIPI_DSI) += liblombo_mipi_dsi.o
$(obj)/liblombo_mipi_dsi.o: $(src)/liblombo_mipi_dsi
	cp $< $@
endif
lombo-mipi-dsi-y += lombo_mipi_dsi.o
obj-$(CONFIG_DRM_LOMBO_MIPI_DSI) += lombo-mipi-dsi.o

# to support csp_tve to lib
CSP_TVE_SOURCE := $(srctree)/$(src)/csp_tve/csp_doss_tve.c

ifneq ($(wildcard $(CSP_TVE_SOURCE)), )
obj-$(CONFIG_DRM_LOMBO_TVE) += csp_tve/
else
obj-$(CONFIG_DRM_LOMBO_TVE) += liblombo_tve.o
$(obj)/liblombo_tve.o: $(src)/liblombo_tve
	cp $< $@
endif
lombo-tve-y += lombo_tve.o
obj-$(CONFIG_DRM_LOMBO_TVE) += lombo-tve.o
