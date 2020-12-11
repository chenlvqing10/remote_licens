/*
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

#ifndef _LOMBO_ROT_H_
#define _LOMBO_ROT_H_

#ifdef CONFIG_DRM_LOMBO_ROT

extern int lombo_rot_get_ver_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file_priv);

extern int lombo_rot_open_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file);

extern int lombo_rot_close_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file);

extern  int lombo_rot_set_timeout_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file);

extern  int lombo_rot_exec_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file);

#else

static inline int lombo_rot_get_ver_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file_priv)
{
	return -ENODEV;
}

static inline int lombo_rot_open_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file_priv)
{
	return -ENODEV;
}

static inline int lombo_rot_close_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file_priv)
{
	return -ENODEV;
}

static inline int lombo_rot_set_timeout_ioctl(struct drm_device *drm,
	void *data, struct drm_file *file_priv)
{
	return -ENODEV;
}

static inline int lombo_rot_exec_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file_priv)
{
	return -ENODEV;
}

#endif /* #ifdef CONFIG_DRM_LOMBO_ROT */

#endif /* #ifndef _LOMBO_ROT_H_ */
