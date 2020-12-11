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

#ifndef _PANEL_LVDS_H_
#define _PANEL_LVDS_H_

#include <drm/drm_crtc.h>

struct panel_lvds_desc {
	const struct drm_display_mode *modes;
	unsigned int num_modes;

	struct {
		unsigned int width;
		unsigned int height;
	} size;

	/**
	 * @prepare: the time (in milliseconds) that it takes for the panel to
	 *           become ready and start receiving video data
	 * @enable: the time (in milliseconds) that it takes for the panel to
	 *          display the first valid frame after starting to receive
	 *          video data
	 * @disable: the time (in milliseconds) that it takes for the panel to
	 *           turn the display off (no content is visible)
	 * @unprepare: the time (in milliseconds) that it takes for the panel
	 *             to power itself down completely
	 */
	struct {
		unsigned int prepare;
		unsigned int enable;
		unsigned int disable;
		unsigned int unprepare;
	} delay;

};

#ifdef CONFIG_DRM_PANEL_WKS_WKS70002
extern const struct panel_lvds_desc ek79003a_wks70002_desc;
#endif

#ifdef CONFIG_DRM_PANEL_TIANMA_TM070JDHG30
extern const struct panel_lvds_desc tianma_tm070jdhg30_desc;
#endif

#endif /* #ifndef _PANEL_LVDS_H_ */
