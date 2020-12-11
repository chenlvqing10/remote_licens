/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef _PANEL_SIMPLE_H_
#define _PANEL_SIMPLE_H_

#include <drm/drm_crtc.h>
#include <drm/drm_mipi_dsi.h>
#include "../lombo/lombo_vo_manage.h"

struct panel_simple_host;

/**
 * @prepare: the time (in milliseconds) that it takes for the panel to
 *	     become ready and start receiving video data
 * @enable: the time (in milliseconds) that it takes for the panel to
 *	    display the first valid frame after starting to receive
 *	    video data
 * @disable: the time (in milliseconds) that it takes for the panel to
 *	     turn the display off (no content is visible)
 * @unprepare: the time (in milliseconds) that it takes for the panel
 *	       to power itself down completely
 * @power_on: the time (in milliseconds) that it takes for the power to
 *	     be stabled
 * @reset: the time (in milliseconds) that it takes for the reset
 */
struct panel_delay {
	unsigned int prepare;
	unsigned int enable;
	unsigned int disable;
	unsigned int unprepare;
	unsigned int power_on;
	unsigned int reset;
};

struct panel_simple_host {
	union {
		struct mipi_dsi_device *dsi;
		struct lombo_vo_device *lombo_vo_dev;
	};
};

struct panel_simple_ops {
	int (*attach)(void *param);
	int (*detach)(void *param);
	ssize_t (*transfer)(struct panel_simple_host *);
};

typedef enum init_panel_type {
	PANEL_SIMPLE_NONE = 0,
	PANEL_SIMPLE_DSI = 1,
	PANEL_SIMPLE_CPU = 2,
	PANEL_SIMPLE_I2C = 3,
	PANEL_SIMPLE_SPI = 4,
} init_panel_type_t;

struct panel_simple_desc {
	struct drm_display_mode *modes;
	unsigned int num_modes;
	unsigned int bpc;
	init_panel_type_t init_panel;

	struct {
		unsigned int width;
		unsigned int height;
	} size;
	struct panel_delay *delay;
	struct panel_simple_ops ops;
};

#ifdef CONFIG_DRM_PANEL_SPC_S92048
extern const struct panel_simple_desc simple_spc_s92048_desc;
#endif
#ifdef CONFIG_DRM_PANEL_FJ018WV04
extern const struct panel_simple_desc simple_fj018wv04_desc;
#endif
#ifdef CONFIG_DRM_PANEL_TIANMA_TM070JDHG30
extern const struct panel_simple_desc tianma_tm070jdhg30_desc;
#endif
#ifdef CONFIG_DRM_PANEL_WKS_WKS70002
extern const struct panel_simple_desc ek79003a_wks70002_desc;
#endif
#ifdef CONFIG_DRM_PANEL_DUAL_LINK_B101UAN021
extern const struct panel_simple_desc b101uan021_desc;
#endif
#ifdef CONFIG_DRM_PANEL_WKS43178
extern const struct panel_simple_desc wks43178_desc;
#endif
#ifdef CONFIG_DRM_PANEL_SLM5P5_HX8399C
extern const struct panel_simple_desc slm5p5_hx8399c_desc;
#endif
#ifdef CONFIG_DRM_PANEL_LT8912B
extern const struct panel_simple_desc lt8912b_desc;
#endif
#ifdef CONFIG_DRM_PANEL_XJS50FWI4015
extern const struct panel_simple_desc xjs50fwi4015_desc;
#endif
#ifdef CONFIG_DRM_PANEL_CJD9365
extern const struct panel_simple_desc cjd9365_desc;
#endif
#ifdef CONFIG_DRM_PANEL_AM500V40
extern const struct panel_simple_desc am500v40_desc;
#endif
#ifdef CONFIG_DRM_PANEL_SC500H40
extern const struct panel_simple_desc sc500hc40_desc;
#endif
#ifdef CONFIG_DRM_PANEL_DPMIPIMODEL
extern const struct panel_simple_desc dpmipi_desc;
#endif

#endif /* #ifndef _PANEL_SIMPLE_H_ */
