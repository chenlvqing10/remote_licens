/**
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/list.h>
#include <linux/errno.h>
#include <linux/export.h>

#include <drm/drmP.h>
#include "lombo_subdrv.h"

static LIST_HEAD(lombo_subdrv_list);

int lombo_subdrv_register(struct lombo_subdrv *subdrv)
{
	DRM_DEBUG_DRIVER("\n");

	if (!subdrv)
		return -EINVAL;

	list_add_tail(&subdrv->list, &lombo_subdrv_list);

	return 0;
}
EXPORT_SYMBOL(lombo_subdrv_register);

int lombo_subdrv_unregister(struct lombo_subdrv *subdrv)
{
	DRM_DEBUG_DRIVER("\n");

	if (!subdrv)
		return -EINVAL;

	list_del(&subdrv->list);

	return 0;
}
EXPORT_SYMBOL(lombo_subdrv_unregister);

int lombo_subdrv_probe(struct drm_device *drm)
{
	struct lombo_subdrv *subdrv, *n;

	DRM_DEBUG_DRIVER("\n");

	list_for_each_entry_safe(subdrv, n, &lombo_subdrv_list, list) {
		int err = 0;

		subdrv->drm = drm;
		if (subdrv->probe)
			err = subdrv->probe(drm, subdrv->dev);
		if (err) {
			DRM_DEBUG("lombo subdrv probe failed.\n");
			list_del(&subdrv->list);
		}

	}

	return 0;
}
EXPORT_SYMBOL(lombo_subdrv_probe);

int lombo_subdrv_remove(struct drm_device *drm)
{
	struct lombo_subdrv *subdrv;

	DRM_DEBUG_DRIVER("\n");

	list_for_each_entry(subdrv, &lombo_subdrv_list, list)
		if (subdrv->remove)
			subdrv->remove(drm, subdrv->dev);

	return 0;
}
EXPORT_SYMBOL(lombo_subdrv_remove);

int lombo_subdrv_open(struct drm_device *drm, struct drm_file *file)
{
	struct lombo_subdrv *subdrv;

	 DRM_DEBUG_DRIVER("\n");

	list_for_each_entry(subdrv, &lombo_subdrv_list, list) {
		int err = 0;

		subdrv->drm = drm;
		if (subdrv->open)
			err = subdrv->open(drm, subdrv->dev, file);
		if (err)
			DRM_DEBUG_DRIVER("subdrv open failed\n");
	}

	return 0;
}
EXPORT_SYMBOL(lombo_subdrv_open);

void lombo_subdrv_close(struct drm_device *drm, struct drm_file *file)
{
	struct lombo_subdrv *subdrv;

	 DRM_DEBUG_DRIVER("\n");

	list_for_each_entry(subdrv, &lombo_subdrv_list, list) {
		if (subdrv->close)
			subdrv->close(drm, subdrv->dev, file);
	}
}
EXPORT_SYMBOL(lombo_subdrv_close);
