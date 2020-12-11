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

#ifndef _LOMBO_WB_H_
#define _LOMBO_WB_H_

#ifdef CONFIG_DRM_LOMBO_WB

extern int lombo_wb_set_para_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file);

extern int lombo_wb_queue_buf_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file);

extern int lombo_wb_dequeue_buf_ioctl(struct drm_device *drm,
	void *data, struct drm_file *file);

extern int lombo_wb_start_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file);

extern int lombo_wb_stop_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file);

#else

static inline int lombo_wb_set_para_ioctl(struct drm_device *drm,
	void *data, struct drm_file *file)
{
	return -ENODEV;
}

static inline int lombo_wb_queue_buf_ioctl(struct drm_device *drm,
	void *data, struct drm_file *file)
{
	return -ENODEV;
}

static inline int lombo_wb_dequeue_buf_ioctl(struct drm_device *drm,
	void *data, struct drm_file *file)
{
	return -ENODEV;
}

static inline int lombo_wb_start_ioctl(struct drm_device *drm,
	void *data, struct drm_file *file)
{
	return -ENODEV;
}

static inline int lombo_wb_stop_ioctl(struct drm_device *drm,
	void *data, struct drm_file *file)
{
	return -ENODEV;
}

#endif /* #ifdef CONFIG_DRM_LOMBO_WB */

#endif /* #ifndef _LOMBO_WB_H_ */
