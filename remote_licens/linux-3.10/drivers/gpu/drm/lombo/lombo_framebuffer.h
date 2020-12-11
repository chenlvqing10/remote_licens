/* lombo_drm_fb.c
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _LOMBO_FRAMEBUFFER_H_
#define _LOMBO_FRAMEBUFFER_H_

int lombo_framebuffer_init(struct drm_device *drm);
int lombo_framebuffer_free(struct drm_device *drm);

#endif /* #ifndef _LOMBO_FRAMEBUFFER_H_ */
