/* lombo_drm_drv.h
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef _LOMBO_DRV_H_
#define _LOMBO_DRV_H_

#include <linux/list.h>

struct drm_property;
struct drm_fbdev_cma;

struct lombo_boot_disp {

	/* boot_fb info */
	uint64_t phy_addr;
	uint32_t fmt; /* drm fourcc */
	uint32_t width;
	uint32_t height;
	uint32_t pitch; /* uint: bytes */

	/* region of boot_fb to copy */
	uint32_t copy_x;
	uint32_t copy_y;
	uint32_t copy_w;
	uint32_t copy_h;
	/* position of boot_fb on screen */
	uint32_t scn_x;
	uint32_t scn_y;
	uint32_t scn_w;
	uint32_t scn_h;

	/* VO info*/
	uint32_t output_type;
	uint32_t output_mode;
	uint32_t output_w;
	uint32_t output_h;

	/* display pipe */
	uint32_t win_id;
	uint32_t dc_id;
	uint32_t tcon_id;
	void *boot_plane; /* masked at dc set mode, for disable boot_fb */
	void *crtc; /* masked at crtc init */

};

struct lombo_drv {
	struct list_head tcon_list;
	struct list_head dc_list;
	struct list_head se_list;
	struct list_head ove_list;

	/* List of page flip events */
	struct list_head page_flip_event_list;

	/* Properties for crtc dc */
	struct drm_property *prop_color_enhance_enable;
	struct drm_property *prop_color_enhance_brightness;
	struct drm_property *prop_color_enhance_saturation;
	struct drm_property *prop_color_enhance_contrast;
	/* Properties for crtc tcon */
	struct drm_property *prop_gamma_enable;

	/* Properties for planes */
	struct drm_property *prop_zpos;
	struct drm_property *prop_color_space;
	struct drm_property *prop_color_range;
	/* for video low frequency sharpness */
	struct drm_property *prop_video_lti;
	/* for video high frequency sharpness */
	struct drm_property *prop_video_peak;

	struct drm_fbdev_cma *fbdev;
	struct lombo_boot_disp *boot_disp;

	int fb_win_id;
};

/* lombo_drm_open should use it belowly. */
struct lombo_file_private {
	void *dit_fpriv; /* for dit file private */
	void *rot_fpriv; /* for rot file private */
	void *scale_fpriv; /* for scale file private */
	void *wb_fpriv; /* for wb file private */

	int ignore_ioctl_permit;
};

extern s32 csp_dpu_get_register_base(unsigned long *addr);
extern s32 csp_ove_get_register_base(u32 id, unsigned long *addr);
extern s32 csp_se_get_register_base(u32 id, unsigned long *addr);
extern s32 csp_dc_get_register_base(u32 dc_index, unsigned long *addr);
extern void csp_doss_get_register_base(u32 doss_idx, unsigned long *addr);
extern s32 csp_tcon_get_register_base(u32 tcon_index, unsigned long *addr);
extern s32 csp_mipi_dsi_get_register_base(u32 index, unsigned long *addr);
extern s32 csp_tve_get_register_base(u32 index, unsigned long *addr);

#endif /* #ifndef _LOMBO_DRV_H_ */
