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

#ifndef _LOMBO_DIT_H_
#define _LOMBO_DIT_H_

#ifdef CONFIG_DRM_LOMBO_DIT

extern int lombo_dit_get_ver_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file_priv);

extern int lombo_dit_open_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file);

extern int lombo_dit_close_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file);

extern  int lombo_dit_set_timeout_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file);

extern  int lombo_dit_exec_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file);

#else

static inline int lombo_dit_get_ver_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file_priv)
{
	return -ENODEV;
}

static inline int lombo_dit_open_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file_priv)
{
	return -ENODEV;
}

static inline int lombo_dit_close_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file_priv)
{
	return -ENODEV;
}

static inline int lombo_dit_set_timeout_ioctl(struct drm_device *drm,
	void *data, struct drm_file *file_priv)
{
	return -ENODEV;
}

static inline int lombo_dit_exec_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file_priv)
{
	return -ENODEV;
}

#endif /* #ifdef CONFIG_DRM_LOMBO_LEGACY_DIT */

#endif /* #ifndef _LOMBO_DRM_DIT_H_ */
