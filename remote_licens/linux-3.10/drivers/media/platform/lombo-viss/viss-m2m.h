/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * VISS_ISP driver code for LomboTech
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#ifndef __VISS_M2M_H__
#define __VISS_M2M_H__


#include <linux/device.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/printk.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <media/v4l2-device.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include <linux/interrupt.h>
#include <linux/fs.h>

#include <linux/of_i2c.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <media/v4l2-of.h>

#include "media-dev.h"
#include "viss-isp.h"

#define RAW_BUF_NUM 5

struct offline_buf {
	int isp_id;
	int raw_depth;
	struct v4l2_subdev *sensor;
	struct v4l2_subdev *csi;
	struct v4l2_rect win;
	struct list_head list;
	struct viss_isp_memory dma;
	struct timespec ts;
};

extern bool isp_offline_busy;
extern bool isp_offline;
extern int isp_offline_id;
extern int isp_offline_set;
extern struct timespec isp_offline_ts;

extern struct list_head *offline_buf_pending[VISS_IS_MAX_NUM];
extern struct list_head offline_fifo;

extern struct viss_is *glb_is[VISS_IS_MAX_NUM];
extern struct viss_is_dev *glb_viss[VISS_IS_MAX_NUM];

extern void isp_offline_process(void);
extern int offline_raw_read(struct list_head *list);
extern int offline_buf_init(struct device *dev, int w, int h,
					int d, int id);
extern void offline_buf_exit(struct device *dev, int id);

static inline void offline_buf_add(struct list_head *list,
		struct offline_buf *buf)
{
	list_add_tail(&buf->list, list);
}

static inline struct offline_buf *offline_buf_pop(
		struct list_head *list)
{
	struct offline_buf *buf;

	buf = list_entry(list->next, struct offline_buf, list);
	list_del(&buf->list);
	return buf;
}

static inline void offline_fifo_add(struct list_head *list)
{
	struct offline_buf *buf = offline_buf_pop(list);
	ktime_get_real_ts(&buf->ts);
	offline_buf_add(&offline_fifo, buf);
}

static inline void offline_fifo_pop(void)
{
	if (!list_empty(&offline_fifo)) {
		struct offline_buf *buf = offline_buf_pop(&offline_fifo);
		offline_buf_add(offline_buf_pending[buf->isp_id], buf);
	}
	isp_offline_id = -1;
	isp_offline_busy = 0;
}

static inline void offline_fifo_release(int id)
{
	struct offline_buf *buf;
	struct offline_buf *next_buf;
	list_for_each_entry_safe(buf, next_buf, &offline_fifo, list) {
		if (buf->isp_id == id) {
			list_del(&buf->list);
			offline_buf_add(offline_buf_pending[buf->isp_id], buf);
		}
	}
}

static inline void offline_fifo_process(void)
{
	if (!list_empty(&offline_fifo))
		isp_offline_process();
}

static inline bool isp_is_offline(struct v4l2_subdev *sensor)
{
	struct viss_source_info *si;

	if (NULL == sensor)
		return false;

	si = v4l2_get_subdev_hostdata(sensor);
	if (si == NULL) {
		PRT_ERR("can't get sensor configuration data!\n");
		return false;
	}

	return (si->isp_path == 2);
}

static inline bool isp_is_online(struct v4l2_subdev *sensor)
{
	struct viss_source_info *si;

	if (NULL == sensor)
		return false;

	si = v4l2_get_subdev_hostdata(sensor);
	if (si == NULL) {
		PRT_ERR("can't get sensor configuration data!\n");
		return false;
	}

	return (si->isp_path == 1);
}

#endif /*__VISS_M2M_H__*/

