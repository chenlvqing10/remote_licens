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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/clk.h>

#include <drm/drmP.h>
#include <drm/drm_fb_cma_helper.h>
#include <drm/drm_gem_cma_helper.h>

#include <drm/lombo_drm.h>
#include "lombo_drv.h"
#include "lombo_subdrv.h"
#include "lombo_dpu.h"

#include "csp_dpu_rot.h"

enum rot_client_state {
	ROT_CLIENT_STATE_IDLE = 0,
	ROT_CLIENT_STATE_WAIT2START,
	ROT_CLIENT_STATE_PREPARE, /* can apply exec arg to rot */
	ROT_CLIENT_STATE_WAIT4FINISH,
	ROT_CLIENT_STATE_FINISH = ROT_CLIENT_STATE_IDLE,
};

enum rot_state {
	ROT_STATE_IDLE = 0,
	ROT_STATE_BUSSY,
};

struct rot_client {
	struct device *dev;
	uint32_t id;
	struct list_head file_node;

	uint64_t timeout; /* ns */

	struct drm_framebuffer *in_fb;
	struct drm_framebuffer *out_fb;

	struct list_head work_node;
	enum rot_client_state state;
	wait_queue_head_t wait;

	spinlock_t wlock;
	int wlocked;
};

struct rot_context {
	struct lombo_subdrv subdrv;

	/** Mapping of client handles to client pointers. */
	struct idr client_idr;
	/** Lock for synchronization of access to client_idr. */
	spinlock_t idr_lock;
	uint32_t client_cnt;

	struct device *dpu_top;
	struct lombo_dpu_clk *clk;
	struct lombo_dpu_clk *tck; /* timer clock */
	uint32_t tck_rate;

	struct resource *regs_res;
	void __iomem *regs;
	int irq;
	uint32_t irq_src;

	spinlock_t work_lock;
	enum rot_state state;
	struct list_head work_list; /* work list head */
	uint32_t work_finish;
	uint32_t work_err;
	uint32_t irq_cnt; /* for debug */
	uint32_t work_empty; /* for debug */
	uint32_t work_time;

	struct mutex en_mutex;
	int en_ref_cnt;
	bool resume;
};

struct rot_file_private {
	struct rot_context *rot;
	spinlock_t client_slock; /* for protect client_list */
	struct list_head client_list;
};

static void rot_udelay(unsigned long time)
{
	udelay(time);
}

static inline int _rot_resume(struct rot_context *rot)
{
	DRM_DEBUG_KMS("resume=%d, %s\n", rot->resume,
		      rot->resume ? "do nothing" : "do resume");
	if (rot->resume)
		return 0;

	lombo_dpu_clk_prepare_enable(rot->clk, 0);
	rot->tck = lombo_dpu_get_enable_tck(rot->dpu_top,
					    0, &rot->tck_rate);
	csp_rot_init(); /* init module */

	rot->resume = 1;
	return 0;
}

static int _rot_suspend(struct rot_context *rot)
{
	DRM_DEBUG_KMS("resume=%d, %s\n", rot->resume,
		      rot->resume ? "do suspend" : "do nothing");
	if (!rot->resume)
		return 0;

	csp_rot_exit(); /* exit module */
	if (rot->tck) {
		lombo_dpu_clk_disable_unprepare(rot->tck);
		lombo_dpu_put_clk(&rot->tck);
	}
	rot->tck_rate = 0;
	lombo_dpu_clk_disable_unprepare(rot->clk);

	rot->resume = 0;
	return 0;
}

static int rot_enable(struct rot_context *rot)
{
	int ref_cnt;
	int ret = 0;

	mutex_lock(&rot->en_mutex);
	rot->en_ref_cnt++;
	ref_cnt = rot->en_ref_cnt;
	if (ref_cnt == 1)
		ret = _rot_resume(rot);
	mutex_unlock(&rot->en_mutex);

	if (ref_cnt > 1) {
		DRM_DEBUG_KMS("%d: rot have enabled\n", __LINE__);
	} else if (ref_cnt == 1) {
		DRM_DEBUG_KMS("%d: rot enable\n", __LINE__);
	} else {
		DRM_ERROR("%d: bad rot en_ref_cnt=%d\n", __LINE__, ref_cnt);
		return -1;
	}
	return ret;
}

static int rot_disable(struct rot_context *rot)
{
	int ref_cnt;

	mutex_lock(&rot->en_mutex);
	rot->en_ref_cnt--;
	ref_cnt = rot->en_ref_cnt;
	if (ref_cnt == 0)
		_rot_suspend(rot);
	mutex_unlock(&rot->en_mutex);

	if (ref_cnt > 0) {
		DRM_DEBUG_KMS("%d: keep rot enable\n", __LINE__);
	} else if (ref_cnt == 0) {
		DRM_DEBUG_KMS("%d: rot disable\n", __LINE__);
	} else {
		DRM_ERROR("%d: bad rot en_ref_cnt=%d\n", __LINE__, ref_cnt);
		return -1;
	}
	return 0;
}

/*
 * judge weather need swap width and height
 * return:
 *    0 - no need;
 *    1 - need.
*/
static inline int rot_transform_swap_wh(drm_lombo_transform_t tf)
{
	return LOMBO_DRM_TRANSFORM_ROT_90 & tf;
}

int lombo_rot_get_ver_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file)
{
	struct drm_lombo_rot_ver *version = data;
	const uint32_t ROT_VERSION_MAJOR = 1;
	const uint32_t ROT_VERSION_MINOR = 0;

	DRM_DEBUG("%d\n", __LINE__);

	version->major = ROT_VERSION_MAJOR;
	version->minor = ROT_VERSION_MINOR;

	return 0;
}

int lombo_rot_open_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file)
{
	struct lombo_file_private *file_priv = file->driver_priv;
	struct rot_file_private *rot_fpriv = file_priv->rot_fpriv;
	struct rot_context *rot = rot_fpriv->rot;
	drm_lombo_rot_user_handle_t *handle = data;
	struct rot_client *client;
	int ret;

	client = kzalloc(sizeof(*client), GFP_KERNEL);
	if (!client) {
		DRM_ERROR("failed to alloc for rot client\n");
		return -ENOMEM;
	}
	client->dev = rot->subdrv.dev;

	idr_preload(GFP_KERNEL);
	spin_lock(&rot->idr_lock);
	ret = idr_alloc(&rot->client_idr, client, 1, 0, GFP_NOWAIT);
	spin_unlock(&rot->idr_lock);
	idr_preload_end();
	if (ret < 1) {
		DRM_ERROR("failed to idr alloc for rot client, ret=%d\n", ret);
		kfree(client);
		return ret ? ret : -1;
	}
	rot->client_cnt++;
	client->id = ret;
	*handle = ret;

	client->timeout = 3000000000ULL; /* 3 second */

	INIT_LIST_HEAD(&client->work_node);
	init_waitqueue_head(&client->wait);
	spin_lock_init(&client->wlock);
	client->wlocked = 0;

	INIT_LIST_HEAD(&client->file_node);
	spin_lock(&rot_fpriv->client_slock);
	list_add_tail(&client->file_node, &rot_fpriv->client_list);
	spin_unlock(&rot_fpriv->client_slock);

	rot_enable(rot);

	return 0;
}

int lombo_rot_close_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file)
{
	struct lombo_file_private *file_priv = file->driver_priv;
	struct rot_file_private *rot_fpriv = file_priv->rot_fpriv;
	struct rot_context *rot = rot_fpriv->rot;
	drm_lombo_rot_user_handle_t *handle = data;
	struct rot_client *client;

	DRM_DEBUG("%d\n", __LINE__);

	spin_lock(&rot->idr_lock);
	client = idr_find(&rot->client_idr, *handle);
	if (client == NULL) {
		spin_unlock(&rot->idr_lock);
		DRM_DEBUG("%d: fail rot close. handle=%d\n", __LINE__, *handle);
		return -EINVAL;
	}
	idr_remove(&rot->client_idr, *handle);
	rot->client_cnt--;
	spin_unlock(&rot->idr_lock);

	spin_lock(&rot_fpriv->client_slock);
	list_del(&client->file_node);
	spin_unlock(&rot_fpriv->client_slock);

	*handle = 0;
	kfree(client);

	rot_disable(rot);

	return 0;
}

int lombo_rot_set_timeout_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file)
{
	struct lombo_file_private *file_priv = file->driver_priv;
	struct rot_file_private *rot_fpriv = file_priv->rot_fpriv;
	struct rot_context *rot = rot_fpriv->rot;
	struct drm_lombo_rot_timeout *timeout = data;
	struct rot_client *client;

	DRM_DEBUG("%d, handle=%u, time=%llu\n", __LINE__,
		timeout->handle, timeout->time_ns);

	spin_lock(&rot->idr_lock);
	client = idr_find(&rot->client_idr, timeout->handle);
	if (client == NULL) {
		spin_unlock(&rot->idr_lock);
		return -EINVAL;
	}
	spin_unlock(&rot->idr_lock);

	if (timeout->time_ns > 0)
		client->timeout = timeout->time_ns;

	return 0;
}

static int rot_client_declare_work(struct rot_client *client)
{
	uint32_t cnt = 1;

	while (cnt) {
		spin_lock(&client->wlock);
		if (client->wlocked == 0) {
			client->wlocked = 1;
			spin_unlock(&client->wlock);
			return 0;
		}
		spin_unlock(&client->wlock);
		usleep_range(1, 2);
		DRM_ERROR("client%d in diff threads. cnt=%d\n",
			client->id, cnt);
		cnt++;
	}

	return -ETIMEDOUT;
}

static int rot_client_undeclare_work(struct rot_client *client)
{
	spin_lock(&client->wlock);
	client->wlocked = 0;
	spin_unlock(&client->wlock);

	return 0;
}

/**
 * get fbs with check para.
 */
static int rot_client_get_fbs(struct drm_device *drm,
	struct rot_client *client, struct drm_lombo_rot_exec_arg *exec_arg)
{
	drm_lombo_transform_t tf = exec_arg->transform;
	struct drm_framebuffer *in_fb, *out_fb;
	uint32_t fmt;
	const struct drm_format_info *format;
	uint32_t width, height;
	const uint32_t min_size = 32;
	const uint32_t block_size_align_byte = 16;
	const uint32_t width_align_byte = 16;
	const uint32_t addr_align_byte = 32;
	int i;

	DRM_DEBUG("client[%u]:transform=%d,infb[%u],outfb[%u]\n",
		client->id, exec_arg->transform,
		exec_arg->in_fb, exec_arg->out_fb);

	/* check transform */
	if (tf > 7) {
		DRM_ERROR("invalid transform=%d\n", tf);
		return -EINVAL;
	}

	/* get in_fb */
	in_fb = drm_framebuffer_lookup(drm, exec_arg->in_fb);
	if (!in_fb) {
		DRM_ERROR("failed to get in_fb id=%d\n", exec_arg->in_fb);
		return -EINVAL;
	}
	client->in_fb = in_fb;

	/* get out_fb */
	out_fb = drm_framebuffer_lookup(drm, exec_arg->out_fb);
	if (!out_fb) {
		DRM_ERROR("failed to get out_fb id=%d\n", exec_arg->out_fb);
		return -EINVAL;
	}
	client->out_fb = out_fb;

	fmt = in_fb->format->format;
	if (rot_transform_swap_wh(tf)) {
		/**
		 * fix format for ROT, such as yuv422->yuv420.
		 */
		if (fmt == DRM_FORMAT_NV16)
			fmt = DRM_FORMAT_NV12;
		else if (fmt == DRM_FORMAT_NV61)
			fmt = DRM_FORMAT_NV21;
		else if (fmt == DRM_FORMAT_YUV422)
			fmt = DRM_FORMAT_YUV420;
		else if (fmt == DRM_FORMAT_YVU422)
			fmt = DRM_FORMAT_YVU420;
	}
	/* check pixel format */
	if (fmt != out_fb->format->format) {
		DRM_ERROR("tf=%x,in_fmt=%x,fmt=%x,out_fmt=%x\n", tf,
			in_fb->format->format, fmt,
			out_fb->format->format);
		return -EINVAL;
	}
	format = out_fb->format;
	for (i = 0; i < format->num_planes; i++) {
		/* check plane cpp */
		switch (format->cpp[i]) {
		case 1:
		case 2:
		case 4:
		/* can add more cases if ROT support */
			break;
		default:
			DRM_ERROR("%d:fmt=0x%x,plane=%d,cpp=%d\n",
				__LINE__, format->format , i,
				format->cpp[i]);
			return -EINVAL;
		}
	}

	/* check fbs size */
	if (rot_transform_swap_wh(tf)) {
		width = in_fb->height;
		height = in_fb->width;
	} else {
		width = in_fb->width;
		height = in_fb->height;
	}
	if ((width != out_fb->width)
		|| (height != out_fb->height)
		|| (width < min_size)
		|| (height < min_size)) {
		DRM_ERROR("%d: %dx%d -> %dx%d. min[%dx%d]\n", __LINE__,
			width, height, out_fb->width, out_fb->height,
			min_size, min_size);
		return -EINVAL;
	}
	for (i = 0; i < format->num_planes; i++) {
		uint32_t width_byte, height_byte;
		uint32_t in_pitch, out_pitch;
		uint64_t in_addr, out_addr;
		struct drm_gem_cma_object *in_buf, *out_buf;

		if (i > 0) {
			width_byte = format->cpp[i] * width / format->hsub;
			height_byte = format->cpp[i] * height / format->vsub;
		} else {
			width_byte = format->cpp[i] * width;
			height_byte = format->cpp[i] * height;
		}
		/* check plane block align */
		if ((width_byte & (block_size_align_byte - 1))
			|| (height_byte & (block_size_align_byte - 1))) {
			DRM_ERROR("%d: %dx%d not align %dx%d\n", __LINE__,
				width_byte, height_byte,
				block_size_align_byte, block_size_align_byte);
			return -EINVAL;
		}

		in_pitch = in_fb->pitches[i];
		out_pitch = out_fb->pitches[i];
		if ((in_pitch & (width_align_byte - 1))
			|| (out_pitch & (width_align_byte - 1))) {
			DRM_ERROR("%d: in_pitch=%d, out_pitch=%d\n",
				__LINE__, in_pitch, out_pitch);
			return -EINVAL;
		}

		in_buf = drm_fb_cma_get_gem_obj(in_fb, i);
		out_buf = drm_fb_cma_get_gem_obj(out_fb, i);
		if (!(in_buf && out_buf)) {
			DRM_ERROR("line=%d\n", __LINE__);
			return -EINVAL;
		}
		in_addr = in_buf->paddr + in_fb->offsets[i];
		out_addr = out_buf->paddr + out_fb->offsets[i];
		if ((in_addr & (addr_align_byte - 1))
			|| (out_addr & (addr_align_byte - 1))) {
			DRM_ERROR("line=%d, addr[0x%llx,0x%llx] align err\n",
				__LINE__, in_addr, out_addr);
			return -EINVAL;
		}
	}

	return 0;
}

static int rot_client_put_fbs(struct drm_device *drm,
	struct rot_client *client)
{
	DRM_DEBUG("%d\n", __LINE__);

	if (client->in_fb) {
		drm_framebuffer_unreference(client->in_fb);
		client->in_fb = NULL;
	}

	if (client->out_fb) {
		drm_framebuffer_unreference(client->out_fb);
		client->out_fb = NULL;
	}

	return 0;
}

static bool is_first_client(struct rot_context *rot,
	struct rot_client *current_client)
{
	return rot->work_list.next == &current_client->work_node;
}

static int rot_client_get_device(struct rot_context *rot,
	struct rot_client *client)
{
	uint64_t time = client->timeout;
	long timeout, ret;
	unsigned long flags;

	spin_lock_irqsave(&rot->work_lock, flags);
	list_add_tail(&client->work_node, &rot->work_list);
	if ((rot->state == ROT_STATE_IDLE) && is_first_client(rot, client)) {
		rot->state = ROT_STATE_BUSSY;
		client->state = ROT_CLIENT_STATE_PREPARE;
		spin_unlock_irqrestore(&rot->work_lock, flags);
		return 0;
	}
	client->state = ROT_CLIENT_STATE_WAIT2START;
	spin_unlock_irqrestore(&rot->work_lock, flags);

	time += 999; /* make sure time > 1000 */
	do_div(time, 1000);
	timeout = usecs_to_jiffies((uint32_t)time);
	ret = wait_event_interruptible_timeout(client->wait,
		rot->state == ROT_STATE_IDLE && is_first_client(rot, client),
		timeout);
	spin_lock_irqsave(&rot->work_lock, flags);
	if (rot->state != ROT_STATE_IDLE || !is_first_client(rot, client)) {
		list_del(&client->work_node);
		client->state = ROT_CLIENT_STATE_IDLE;
		spin_unlock_irqrestore(&rot->work_lock, flags);
		DRM_ERROR("wait2start timeout. timeout=%lujif, ret=%lu\n",
			timeout, ret);
		return -1;
	}
	rot->state = ROT_STATE_BUSSY;
	client->state = ROT_CLIENT_STATE_PREPARE;
	spin_unlock_irqrestore(&rot->work_lock, flags);

	return 0;
}

static int rot_client_put_device(struct rot_context *rot,
	struct rot_client *client)
{
	unsigned long flags;

	DRM_DEBUG("%d\n", __LINE__);

	spin_lock_irqsave(&rot->work_lock, flags);
	list_del(&client->work_node);
	rot->state = ROT_STATE_IDLE;
	if (!list_empty(&rot->work_list)) {
		struct rot_client *next = list_first_entry(
			&rot->work_list, struct rot_client, work_node);
		wake_up_interruptible(&next->wait);
	}
	spin_unlock_irqrestore(&rot->work_lock, flags);

	return 0;
}

static int rot_client_exec_fb_sync(struct rot_context *rot,
	struct rot_client *client, struct drm_lombo_rot_exec_arg *exec_arg)
{
	int ret;
	uint64_t time;
	long timeout, timeout_ret;
	unsigned long flags;
	uint32_t work_finish, work_err, work_time;

	rot_cfg_ch_t cfgs;
	struct drm_framebuffer *in_fb, *out_fb;
	const struct drm_format_info *format;
	int plane;
	const uint32_t transform[] = {
		[LOMBO_DRM_TRANSFORM_COPY] = ROT_MODE_0,
		[LOMBO_DRM_TRANSFORM_FLIP_H] = ROT_MODE_4,
		[LOMBO_DRM_TRANSFORM_FLIP_V] = ROT_MODE_6,
		[LOMBO_DRM_TRANSFORM_ROT_180] = ROT_MODE_2,
		[LOMBO_DRM_TRANSFORM_ROT_90] = ROT_MODE_1,
		[LOMBO_DRM_TRANSFORM_FLIP_H_ROT_90] = ROT_MODE_5,
		[LOMBO_DRM_TRANSFORM_FLIP_V_ROT_90] = ROT_MODE_7,
		[LOMBO_DRM_TRANSFORM_ROT_270] = ROT_MODE_3,
	};

	in_fb = client->in_fb;
	out_fb = client->out_fb;
	format = in_fb->format;

	for (plane = 0; plane < format->num_planes; plane++) {
		struct drm_gem_cma_object *in_buf, *out_buf;
		int horz_sampling = 1;
		int vert_sampling = 1;

		in_buf = drm_fb_cma_get_gem_obj(in_fb, plane);
		out_buf = drm_fb_cma_get_gem_obj(out_fb, plane);
		if (!(in_buf && out_buf)) {
			DRM_ERROR("line=%d\n", __LINE__);
			return -1;
		}
		if (plane > 0) {
			horz_sampling = format->hsub;
			vert_sampling = format->vsub;
		}

		/* config process parameters */
		memset(&cfgs, 0, sizeof(cfgs));
		cfgs.en = 1;
		cfgs.bytespp = format->cpp[plane];
		cfgs.in_w = in_fb->width / horz_sampling;
		cfgs.in_h = in_fb->height / vert_sampling;
		cfgs.in_addr = in_buf->paddr + in_fb->offsets[plane];
		cfgs.in_lnstd = in_fb->pitches[plane];
		cfgs.out_w = out_fb->width / horz_sampling;
		cfgs.out_h = out_fb->height / vert_sampling;
		cfgs.out_addr = out_buf->paddr + out_fb->offsets[plane];
		cfgs.out_lnstd = out_fb->pitches[plane];

		/* sampling of hori&vert of chroma must be same if swap w&h */
		if ((plane > 0)
			&& rot_transform_swap_wh(exec_arg->transform)
			&& (horz_sampling != vert_sampling)) {
			DRM_DEBUG("sw_resampling. plane=%d, sampling=%d,%d\n",
				plane, horz_sampling, vert_sampling);
			cfgs.in_h >>= 1;
			cfgs.in_lnstd <<= 1;
		}

		ret = csp_rot_set_cfg_ch(plane, &cfgs);
		if (ret) {
			DRM_ERROR("rot_set_cfg ch=%d fail\n", plane);
			return ret;
		}
	}
	for (; plane < 3; plane++) {
		cfgs.en = 0;
		csp_rot_set_cfg_ch(plane, &cfgs);
		DRM_DEBUG("cfgs chn%d: en=%d\n", plane, cfgs.en);
	}

	/* config process mode */
	if (exec_arg->transform >= ARRAY_SIZE(transform)) {
		DRM_ERROR("invalid transform %d\n", exec_arg->transform);
		return -EINVAL;
	}
	ret = csp_rot_set_mode(transform[exec_arg->transform]);
	if (ret) {
		DRM_ERROR("csp_rot_set_mode fail, ret=%d\n", ret);
		return ret;
	}

	/* start process */
	client->state = ROT_CLIENT_STATE_WAIT4FINISH;
	csp_rot_enable_irq(rot->irq_src);
	csp_rot_start();

	/* wait for finish */
	time = client->timeout + 999;
	do_div(time, 1000);
	timeout = usecs_to_jiffies((uint32_t)time);
	timeout_ret = wait_event_interruptible_timeout(client->wait,
		client->state != ROT_CLIENT_STATE_WAIT4FINISH, timeout);

	spin_lock_irqsave(&rot->work_lock, flags);
	if (client->state != ROT_CLIENT_STATE_FINISH) {
		csp_rot_clear_irq(rot->irq_src);
		csp_rot_disable_irq(rot->irq_src);
		csp_rot_stop();
		csp_rot_reset();
		client->state = ROT_CLIENT_STATE_IDLE;
		spin_unlock_irqrestore(&rot->work_lock, flags);
		DRM_ERROR("wait4finish timeout: %lujif --> %lu\n",
			timeout, timeout_ret);
		DRM_ERROR("irq_cnt=%u, work_empty_cnt=%u\n",
			rot->irq_cnt, rot->work_empty);
		return -1;
	}
	work_finish = rot->work_finish;
	work_err = rot->work_err;
	work_time = rot->work_time;
	spin_unlock_irqrestore(&rot->work_lock, flags);

	/* here is ROT_CLIENT_STATE_FINISH */
	csp_rot_disable_irq(rot->irq_src);
	csp_rot_stop();

	DRM_DEBUG("rot: client%d: work_time=%u ms\n", client->id, work_time);
	if (work_err) {
		DRM_ERROR("client(%d) work_err(0x%x)\n", client->id, work_err);
		return -1;
	}

	return 0;
}

int lombo_rot_exec_ioctl(struct drm_device *drm,
	void *data, struct drm_file *file)
{
	struct lombo_file_private *file_priv = file->driver_priv;
	struct rot_file_private *rot_fpriv = file_priv->rot_fpriv;
	struct rot_context *rot = rot_fpriv->rot;
	struct drm_lombo_rot_exec_arg *exec_arg = data;
	struct rot_client *client;
	int ret;

	spin_lock(&rot->idr_lock);
	client = idr_find(&rot->client_idr, exec_arg->handle);
	if (client == NULL) {
		spin_unlock(&rot->idr_lock);
		return -EINVAL;
	}
	spin_unlock(&rot->idr_lock);

	ret = rot_client_declare_work(client);
	if (ret)
		return ret;

	ret = rot_client_get_fbs(drm, client, exec_arg);
	if (ret)
		goto out;

	ret = rot_client_get_device(rot, client);
	if (ret)
		goto out;

	ret = rot_client_exec_fb_sync(rot, client, exec_arg);

	rot_client_put_device(rot, client);

out:
	/* guarantee unreference all fbs */
	rot_client_put_fbs(drm, client);

	rot_client_undeclare_work(client);

	return ret;
}

static irqreturn_t rot_irq_handler(int irq, void *dev_id)
{
	struct rot_context *rot = dev_id;
	unsigned long flags;

	rot->irq_cnt++;

	spin_lock_irqsave(&rot->work_lock, flags);
	if (!list_empty(&rot->work_list)) {
		struct rot_client *client = list_first_entry(
			&rot->work_list, struct rot_client, work_node);
		rot->work_finish =
			csp_rot_query_and_clear_irq(ROT_IRQ_FINISH) ? 1 : 0;
		rot->work_err =
			csp_rot_query_and_clear_irq(ROT_IRQ_SRCDECERR) ? 1 : 0;
		rot->work_err |=
			csp_rot_query_and_clear_irq(ROT_IRQ_DSTDECERR) ? 2 : 0;
		csp_rot_disable_irq(rot->irq_src);
		csp_rot_get_time(rot->tck_rate, &rot->work_time);
		client->state = ROT_CLIENT_STATE_FINISH;
		wake_up_interruptible(&client->wait);
	} else {
		rot->work_empty++;
	}
	spin_unlock_irqrestore(&rot->work_lock, flags);

	return IRQ_HANDLED;
}

static int rot_subdrv_probe(struct drm_device *drm, struct device *dev)
{
	return 0;
}

static void rot_subdrv_remove(struct drm_device *drm, struct device *dev)
{
	return;
}

static int rot_subdrv_open(struct drm_device *drm, struct device *dev,
		struct drm_file *file)
{
	struct rot_context *rot = dev_get_drvdata(dev);
	struct rot_file_private *rot_fpriv;
	struct lombo_file_private *file_priv = file->driver_priv;

	rot_fpriv = kzalloc(sizeof(*rot_fpriv), GFP_KERNEL);
	if (!rot_fpriv) {
		DRM_ERROR("%d\n", __LINE__);
		return -ENOMEM;
	}

	rot_fpriv->rot = rot;
	spin_lock_init(&rot_fpriv->client_slock);
	INIT_LIST_HEAD(&rot_fpriv->client_list);

	file_priv->rot_fpriv = rot_fpriv;

	return 0;
}

static void rot_subdrv_close(struct drm_device *drm, struct device *dev,
		struct drm_file *file)
{
	struct rot_context *rot = dev_get_drvdata(dev);
	struct lombo_file_private *file_priv = file->driver_priv;
	struct rot_file_private *rot_fpriv = file_priv->rot_fpriv;
	uint32_t rm_cnt_l = 0;
	uint32_t rm_cnt_i = 0;
	uint32_t client_cnt = 0;

	spin_lock(&rot_fpriv->client_slock);
	while (!list_empty(&rot_fpriv->client_list)) {
		struct rot_client *client_l; /* client in list */
		struct rot_client *client_i; /* client in idr */

		client_l = list_first_entry(&rot_fpriv->client_list,
			struct rot_client, file_node);
		list_del(&client_l->file_node);
		spin_unlock(&rot_fpriv->client_slock);
		rm_cnt_l++;

		spin_lock(&rot->idr_lock);
		client_i = idr_find(&rot->client_idr, client_l->id);
		if ((client_i != NULL) && (client_i == client_l)) {
			idr_remove(&rot->client_idr, client_l->id);
			client_cnt = rot->client_cnt;
			rot->client_cnt--;
			rm_cnt_i++;
		}
		spin_unlock(&rot->idr_lock);

		kfree(client_l);
		rot_disable(rot);

		spin_lock(&rot_fpriv->client_slock);
	}
	spin_unlock(&rot_fpriv->client_slock);

	kfree(rot_fpriv);
	file_priv->rot_fpriv = NULL;

	if ((rm_cnt_i > 0) || (rm_cnt_l > 0))
		DRM_ERROR("rot client leak: rm_cnt[%d,%d] of cnt=%d\n",
			rm_cnt_l, rm_cnt_i, client_cnt);

	return;
}

static int rot_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct device_node *top;
	struct platform_device *top_pdev;
	struct rot_context *rot;
	struct lombo_subdrv *subdrv;
	struct resource *res;
	int ret = 0;
	u32 value;

	top = of_parse_phandle(dev->of_node, "top", 0);
	if (!top) {
		DRM_ERROR("failed to parse phandle of top\n");
		return -EINVAL;
	}
	top_pdev = of_find_device_by_node(top);
	if (!top_pdev) {
		DRM_ERROR("failed to find device dpu top\n");
		return -EINVAL;
	}

	rot = kzalloc(sizeof(*rot), GFP_KERNEL);
	if (!rot) {
		dev_err(dev, "failed to alloc for rot ctx\n");
		return -ENOMEM;
	}
	rot->dpu_top = &top_pdev->dev;

	/* parse register */
	rot->regs_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	rot->regs = devm_ioremap_resource(dev, rot->regs_res);
	if (IS_ERR(rot->regs)) {
		dev_err(dev, "failed to get reg for rot\n");
		return PTR_ERR(rot->regs);
	}
	csp_rot_set_register_base((uintptr_t)rot->regs,
		resource_size(rot->regs_res));
	csp_rot_set_delay_func(rot_udelay);

	/* parse irq */
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (res == NULL) {
		dev_err(dev, "get interrupt resource failed.\n");
		return -ENXIO;
	}
	ret = devm_request_irq(dev, res->start,
		rot_irq_handler, 0, "lombo_rot", rot);
	if (ret) {
		dev_err(dev, "request interrupt failed.\n");
		return ret;
	}
	rot->irq = res->start;
	rot->irq_src = ROT_IRQ_FINISH;
	pr_debug("rot irqno=%d, irq_src=0x%x\n", rot->irq, rot->irq_src);

	/* get clock */
	rot->clk = lombo_dpu_get_clk(dev, rot->dpu_top, DPU_MOD_ROT);
	if (!rot->clk) {
		dev_err(dev, "get clk for rot failed\n");
		return -ENODEV;
	}
	if (!of_property_read_u32(np, "clk_rate", &value))
		lombo_dpu_clk_set_rate(rot->clk, value);
	else
		DRM_ERROR("failed to of get rot clk_rate\n");
	if (!of_property_read_u32(np, "sclk", &value))
		lombo_dpu_clk_set_parent(rot->clk, value);
	else
		lombo_dpu_clk_set_parent(rot->clk, 0);

	idr_init(&rot->client_idr);
	spin_lock_init(&rot->idr_lock);

	spin_lock_init(&rot->work_lock);
	INIT_LIST_HEAD(&rot->work_list);

	mutex_init(&rot->en_mutex);

	subdrv = &rot->subdrv;
	subdrv->dev = dev;
	subdrv->probe = rot_subdrv_probe;
	subdrv->remove = rot_subdrv_remove;
	subdrv->open = rot_subdrv_open;
	subdrv->close = rot_subdrv_close;

	ret = dev_set_drvdata(dev, rot);
	if (ret) {
		dev_err(dev, "failed to set drvdata\n");
		return ret;
	}

	ret = lombo_subdrv_register(subdrv);
	if (ret < 0)
		DRM_ERROR("failed to register lombo rot device.\n");

	return ret;
}

static int rot_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct rot_context *rot = dev_get_drvdata(dev);
	uint32_t client_cnt = rot->client_cnt;
	int ret = 0;

	dev_info(dev, "rot remove\n");

	if (client_cnt > 0)
		dev_err(dev, "remain rot %d client!\n", client_cnt);

	ret = lombo_subdrv_unregister(&rot->subdrv);
	if (ret)
		dev_err(dev, "rot subdrv unregister failed\n");

	lombo_dpu_put_clk(&rot->clk);
	dev_set_drvdata(dev, NULL);
	csp_rot_set_register_base(0, 0);
	kfree(rot);

	return ret;
}

int lombo_rot_pm_suspend(struct device *dev)
{
	struct rot_context *rot = dev_get_drvdata(dev);

	if (rot->state == ROT_STATE_BUSSY)
		DRM_ERROR("bussy to suspend\n");

	return _rot_suspend(rot);
}

int lombo_rot_pm_resume(struct device *dev)
{
	struct rot_context *rot = dev_get_drvdata(dev);

	if (rot->client_cnt)
		return _rot_resume(rot);
	else
		return 0;
}

static const struct dev_pm_ops lombo_rot_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(lombo_rot_pm_suspend, lombo_rot_pm_resume)
};

static struct of_device_id lombo_rot_of_table[] = {
	{ .compatible = "lombo,lombo-n7v0-rot" },
	{ .compatible = "lombo,lombo-n7v1-rot" },
	{ .compatible = "lombo,lombo-n8v0-rot" },
	{ .compatible = "lombo,lombo-n9v0-rot" },
	{ }
};

struct platform_driver lombo_rot_platform_driver = {
	.driver = {
		.name = "lombo-rot",
		.owner = THIS_MODULE,
		.of_match_table = lombo_rot_of_table,
		.pm = &lombo_rot_pm_ops,
	},
	.probe = rot_probe,
	.remove = rot_remove,
};
module_platform_driver(lombo_rot_platform_driver);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Lombo SoC DPU Rotation Driver");
MODULE_LICENSE("GPL");
