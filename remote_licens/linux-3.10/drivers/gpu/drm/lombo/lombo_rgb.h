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

#ifndef _LOMBO_RGB_H_
#define _LOMBO_RGB_H_

struct lombo_rgb;

struct lombo_rgb *lombo_rgb_init(struct drm_device *drm,
	struct lombo_tcon *tcon,
	struct device_node *np, const char *phandle_name,
	uint32_t possible_crtcs);
int lombo_rgb_deinit(struct lombo_rgb *rgb);

#endif /* #ifndef _LOMBO_RGB_H_ */
