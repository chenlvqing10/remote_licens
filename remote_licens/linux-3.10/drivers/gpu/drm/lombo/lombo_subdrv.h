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

#ifndef _DRM_SUBDRV_H_
#define _DRM_SUBDRV_H_

#include "lombo_drv.h"


/*
 * Lombo drm sub driver structure.
 *
 * @list: sub driver has its own list object to register to lombo drm driver.
 * @dev: pointer to device object for subdrv device driver.
 * @drm_dev: pointer to drm_device and this pointer would be set
 *	when sub driver calls lombo_drm_subdrv_register().
 * @probe: this callback would be called by lombo drm driver after
 *	subdrv is registered to it.
 * @remove: this callback is used to release resources created
 *	by probe callback.
 * @open: this would be called with drm device file open.
 * @close: this would be called with drm device file close.
 */
struct lombo_subdrv {
	struct list_head list;
	struct device *dev;
	struct drm_device *drm;

	int (*probe)(struct drm_device *drm_dev, struct device *dev);
	void (*remove)(struct drm_device *drm_dev, struct device *dev);
	int (*open)(struct drm_device *drm_dev, struct device *dev,
			struct drm_file *file);
	void (*close)(struct drm_device *drm_dev, struct device *dev,
			struct drm_file *file);
};

/*
 * This function would be called by sub drivers to register
 * sub driver object to lombo drm driver.
 */
int lombo_subdrv_register(struct lombo_subdrv *subdrv);

/* this function removes subdrv list from lombo drm driver */
int lombo_subdrv_unregister(struct lombo_subdrv *subdrv);

int lombo_subdrv_probe(struct drm_device *drm);
int lombo_subdrv_remove(struct drm_device *drm);

int lombo_subdrv_open(struct drm_device *drm, struct drm_file *file);
void lombo_subdrv_close(struct drm_device *drm, struct drm_file *file);

#endif /* #ifndef _DRM_SUBDRV_H_ */
