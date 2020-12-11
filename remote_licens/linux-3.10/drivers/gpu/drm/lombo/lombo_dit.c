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

#include "csp_dpu_dit.h"

enum dit_client_state {
	DIT_CLIENT_STATE_IDLE = 0,
	DIT_CLIENT_STATE_WAIT2START,
	DIT_CLIENT_STATE_PREPARE, /* can apply exec arg to dit */
	DIT_CLIENT_STATE_WAIT4FINISH,
	DIT_CLIENT_STATE_FINISH = DIT_CLIENT_STATE_IDLE,
};

enum dit_state {
	DIT_STATE_IDLE = 0,
	DIT_STATE_BUSSY,
};

struct dit_client {
	struct device *dev;
	uint32_t id;
	struct list_head file_node;

	uint64_t timeout; /* ns */

	struct drm_framebuffer *in_fb0;
	struct drm_framebuffer *in_fb1;
	struct drm_framebuffer *in_fb2;
	struct drm_framebuffer *out_fb0;

	struct list_head work_node;
	enum dit_client_state state;
	wait_queue_head_t wait;

	spinlock_t wlock;
	int wlocked;
};

struct dit_context {
	struct lombo_subdrv subdrv;

	struct device *dpu_top;
	struct lombo_dpu_clk *clk;

	struct resource *regs_res;
	void __iomem *regs;
	int irq;
	uint32_t irq_src;
	uint32_t irq_cnt; /* for debug */
	uint32_t work_empty; /* for debug */

	/** Mapping of client handles to client pointers. */
	struct idr client_idr;
	/** Lock for synchronization of access to client_idr. */
	spinlock_t idr_lock;
	uint32_t client_cnt;

	enum dit_state state;
	spinlock_t work_lock;
	struct list_head work_list; /* work list head */
	uint32_t irq_status; /* return dit irq wb status in irq_handler */
	uint32_t work_cnt;

	spinlock_t en_lock;
	int en_ref_cnt;
	bool resume;
};

struct dit_file_private {
	struct dit_context *dit;
	spinlock_t client_slock; /* for protect client_list */
	struct list_head client_list;
};

static inline int _dit_resume(struct dit_context *dit)
{
	DRM_DEBUG_KMS("resume=%d, %s\n", dit->resume,
		dit->resume ? "do nothing" : "do resume");
	if (dit->resume)
		return 0;

	lombo_dpu_clk_prepare_enable(dit->clk, 0);
	csp_dit_set_wb_timer_thr(0xFFFF);
	csp_dit_clear_irq_status(DIT_IRQ_WB_FINISH
		| DIT_IRQ_WB_OVERFLOW | DIT_IRQ_WB_TIMEOUT);

	dit->resume = 1;
	return 0;
}

static inline int _dit_suspend(struct dit_context *dit)
{
	DRM_DEBUG_KMS("resume=%d, %s\n", dit->resume,
		      dit->resume ? "do suspend" : "do nothing");
	if (!dit->resume)
		return 0;

	lombo_dpu_clk_disable_unprepare(dit->clk);

	dit->resume = 0;
	return 0;
}

static int dit_enable(struct dit_context *dit)
{
	int ref_cnt;

	spin_lock(&dit->en_lock);
	dit->en_ref_cnt++;
	ref_cnt = dit->en_ref_cnt;
	if (ref_cnt == 1)
		_dit_resume(dit);
	spin_unlock(&dit->en_lock);

	if (ref_cnt == 1) {
		DRM_DEBUG_KMS("%d: dit done enabled\n", __LINE__);
	} else if (ref_cnt > 1) {
		DRM_DEBUG_KMS("%d: dit is enabling\n", __LINE__);
	} else {
		DRM_ERROR("%d: bad en_ref_cnt=%d\n", __LINE__, ref_cnt);
		return -1;
	}
	return 0;
}

static int dit_disable(struct dit_context *dit)
{
	int ref_cnt;

	spin_lock(&dit->en_lock);
	dit->en_ref_cnt--;
	ref_cnt = dit->en_ref_cnt;
	if (ref_cnt == 0)
		_dit_suspend(dit);
	spin_unlock(&dit->en_lock);

	if (ref_cnt == 0) {
		DRM_DEBUG_KMS("%d: dit done disabled\n", __LINE__);
	} else if (ref_cnt > 0) {
		DRM_DEBUG_KMS("%d: keep enable\n", __LINE__);
	} else {
		DRM_ERROR("%d: bad en_ref_cnt=%d\n", __LINE__, ref_cnt);
		return -1;
	}
	return 0;
}

int lombo_dit_get_ver_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file)
{
	struct drm_lombo_dit_ver *version = data;
	uint32_t major = 0;
	uint32_t minor = 0;

	DRM_DEBUG("%d\n", __LINE__);

	csp_dit_get_ver(&major, &minor);
	version->major = major;
	version->minor = minor;

	return 0;
}

int lombo_dit_open_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file)
{
	struct lombo_file_private *file_priv = file->driver_priv;
	struct dit_file_private *dit_fpriv = file_priv->dit_fpriv;
	struct dit_context *dit = dit_fpriv->dit;
	drm_lombo_dit_user_handle_t *handle = data;
	struct dit_client *client;
	int ret;

	DRM_DEBUG("%d\n", __LINE__);

	client = kzalloc(sizeof(*client), GFP_KERNEL);
	if (!client) {
		DRM_ERROR("failed to alloc for dit client\n");
		return -ENOMEM;
	}
	client->dev = dit->subdrv.dev;

	idr_preload(GFP_KERNEL);
	spin_lock(&dit->idr_lock);
	ret = idr_alloc(&dit->client_idr, client, 1, 0, GFP_NOWAIT);
	spin_unlock(&dit->idr_lock);
	idr_preload_end();
	if (ret < 1) {
		DRM_ERROR("failed to idr alloc for dit client, ret=%d\n", ret);
		kfree(client);
		return ret ? ret : -1;
	}
	dit->client_cnt++;
	client->id = ret;
	*handle = ret;

	client->timeout = 3000000000ULL;

	INIT_LIST_HEAD(&client->work_node);
	init_waitqueue_head(&client->wait);
	spin_lock_init(&client->wlock);
	client->wlocked = 0;

	INIT_LIST_HEAD(&client->file_node);
	spin_lock(&dit_fpriv->client_slock);
	list_add_tail(&client->file_node, &dit_fpriv->client_list);
	spin_unlock(&dit_fpriv->client_slock);

	dit_enable(dit);

	return 0;
}

int lombo_dit_close_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file)
{
	struct lombo_file_private *file_priv = file->driver_priv;
	struct dit_file_private *dit_fpriv = file_priv->dit_fpriv;
	struct dit_context *dit = dit_fpriv->dit;
	drm_lombo_dit_user_handle_t *handle = data;
	struct dit_client *client;

	DRM_DEBUG("%d\n", __LINE__);

	spin_lock(&dit->idr_lock);
	client = idr_find(&dit->client_idr, *handle);
	if (client == NULL) {
		spin_unlock(&dit->idr_lock);
		DRM_DEBUG("%d: fail dit close. handle=%d\n", __LINE__, *handle);
		return -EINVAL;
	}
	idr_remove(&dit->client_idr, *handle);
	dit->client_cnt--;
	spin_unlock(&dit->idr_lock);

	spin_lock(&dit_fpriv->client_slock);
	list_del(&client->file_node);
	spin_unlock(&dit_fpriv->client_slock);

	*handle = 0;
	kfree(client);

	dit_disable(dit);

	return 0;
}

int lombo_dit_set_timeout_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file)
{
	struct lombo_file_private *file_priv = file->driver_priv;
	struct dit_file_private *dit_fpriv = file_priv->dit_fpriv;
	struct dit_context *dit = dit_fpriv->dit;
	struct drm_lombo_dit_timeout *timeout = data;
	struct dit_client *client;

	DRM_DEBUG("%d, handle=%u, time=%llu\n", __LINE__,
		timeout->handle, timeout->time_ns);

	spin_lock(&dit->idr_lock);
	client = idr_find(&dit->client_idr, timeout->handle);
	if (client == NULL) {
		spin_unlock(&dit->idr_lock);
		return -EINVAL;
	}
	spin_unlock(&dit->idr_lock);

	if (timeout->time_ns > 0)
		client->timeout = timeout->time_ns;

	return 0;
}

/* get fb if fb_id > 0 */
static int dit_client_get_fb(struct drm_device *drm,
	uint32_t fb_id, struct drm_framebuffer **fb)
{
	struct drm_framebuffer *f = NULL;

	if (fb_id > 0) {
		f = drm_framebuffer_lookup(drm, fb_id);
		if (!f)
			return -1;
		*fb = f;
	}
	return 0;
}

static int dit_client_declare_work(struct dit_client *client)
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

static int dit_client_undeclare_work(struct dit_client *client)
{
	spin_lock(&client->wlock);
	client->wlocked = 0;
	spin_unlock(&client->wlock);

	return 0;
}

/**
 * get fbs with check para.
 */
static int dit_client_get_fbs(struct drm_device *drm,
	struct dit_client *client, struct drm_lombo_dit_exec_arg *exec_arg)
{
	const struct drm_format_info *format;
	uint32_t width, height;
	uint32_t plane;

	const uint32_t min_width = 32;
	const uint32_t min_height = 8;
	const uint32_t width_align_byte = 32;
	const uint32_t addr_align_byte = 32;

	DRM_DEBUG("client[%u]. exec_arg: TopFieldFirst[%s] ",
		client->id,
		exec_arg->top_filed_first ? "Y" : "N");
	DRM_DEBUG("frame_seq=%u. infb[%u,%u,%u], outfb[%u]\n",
		exec_arg->frame_seq,
		exec_arg->in_fb0,
		exec_arg->in_fb1,
		exec_arg->in_fb2,
		exec_arg->out_fb0);

	if ((exec_arg->in_fb0 == 0)
		|| (exec_arg->in_fb1 == 0)
		|| (exec_arg->out_fb0 == 0)) {
		DRM_ERROR("%d\n", __LINE__);
		return -EINVAL;
	}
	if ((!exec_arg->top_filed_first)
		&& (exec_arg->in_fb2 == 0)) {
		DRM_ERROR("%d\n", __LINE__);
		return -EINVAL;
	}

	if (dit_client_get_fb(drm,
		exec_arg->in_fb0, &client->in_fb0)) {
		DRM_ERROR("%d: failed to get fb id=%d\n",
			__LINE__, exec_arg->in_fb0);
		return -1;
	}

	if (dit_client_get_fb(drm,
		exec_arg->in_fb1, &client->in_fb1)) {
		DRM_ERROR("%d: failed to get fb id=%d\n",
			__LINE__, exec_arg->in_fb1);
		return -1;
	}

	if (dit_client_get_fb(drm,
		exec_arg->in_fb2, &client->in_fb2)) {
		DRM_ERROR("%d: failed to get fb id=%d\n",
			__LINE__, exec_arg->in_fb2);
		return -1;
	}

	if (dit_client_get_fb(drm,
		exec_arg->out_fb0, &client->out_fb0)) {
		DRM_ERROR("%d: failed to get fb id=%d\n",
			__LINE__, exec_arg->out_fb0);
		return -1;
	}

	/* check pixel format */
	format = client->in_fb0->format;
	if (format != client->out_fb0->format) {
		DRM_ERROR("%d:fmt[in:%d; out:%d]\n", __LINE__,
			  format->format, client->out_fb0->format->format);
		return -EINVAL;
	}

	/* check fbs  size */
	width = client->in_fb0->width;
	height = client->in_fb0->height;
	if ((width != client->out_fb0->width)
		|| (height != client->out_fb0->height)
		|| (width < min_width)
		|| (height < min_height)) {
		DRM_ERROR("%d:%dx%d->%dx%d. min[%dx%d]\n", __LINE__,
			width, height,
			client->out_fb0->width, client->out_fb0->height,
			min_width, min_height);
		return -EINVAL;
	}

	for (plane = 0; plane < format->num_planes; plane++) {
		uint32_t in_pitch, out_pitch;
		uint64_t in_addr, out_addr;
		struct drm_gem_cma_object *in_buf, *out_buf;

		/* check pitch align */
		in_pitch = client->in_fb0->pitches[plane];
		out_pitch = client->out_fb0->pitches[plane];
		if ((in_pitch & (width_align_byte - 1))
			|| (out_pitch & (width_align_byte - 1))) {
			DRM_ERROR("%d: in_pitch=%d, out_pitch=%d\n",
				__LINE__, in_pitch, out_pitch);
			return -EINVAL;
		}

		/* check addr align */
		in_buf = drm_fb_cma_get_gem_obj(client->in_fb0, plane);
		out_buf = drm_fb_cma_get_gem_obj(client->out_fb0, plane);
		if (!(in_buf && out_buf)) {
			DRM_ERROR("line=%d\n", __LINE__);
			return -EINVAL;
		}
		in_addr = in_buf->paddr
			+ client->in_fb0->offsets[plane];
		out_addr = out_buf->paddr
			+ client->out_fb0->offsets[plane];
		if ((in_addr & (addr_align_byte - 1))
			|| (out_addr & (addr_align_byte - 1))) {
			DRM_ERROR("line=%d, addr[0x%llx,0x%llx] align err\n",
				__LINE__, in_addr, out_addr);
			return -EINVAL;
		}
	}

	return 0;
}

static int dit_client_put_fbs(struct drm_device *drm,
	struct dit_client *client)
{
	DRM_DEBUG("%d\n", __LINE__);

	if (client->in_fb0) {
		drm_framebuffer_unreference(client->in_fb0);
		client->in_fb0 = NULL;
	}

	if (client->in_fb1) {
		drm_framebuffer_unreference(client->in_fb1);
		client->in_fb1 = NULL;
	}

	if (client->in_fb2) {
		drm_framebuffer_unreference(client->in_fb2);
		client->in_fb2 = NULL;
	}

	if (client->out_fb0) {
		drm_framebuffer_unreference(client->out_fb0);
		client->out_fb0 = NULL;
	}

	return 0;
}

static int dit_client_get_device(struct dit_context *dit,
	struct dit_client *client)
{
	uint64_t time = client->timeout;
	long timeout, ret;
	unsigned long flags;

	spin_lock_irqsave(&dit->work_lock, flags);
	list_add_tail(&client->work_node, &dit->work_list);
	if (dit->state == DIT_STATE_IDLE) {
		dit->state = DIT_STATE_BUSSY;
		client->state = DIT_CLIENT_STATE_PREPARE;
		spin_unlock_irqrestore(&dit->work_lock, flags);
		return 0;
	}
	client->state = DIT_CLIENT_STATE_WAIT2START;
	spin_unlock_irqrestore(&dit->work_lock, flags);

	time += 999; /* make sure time > 1000 */
	do_div(time, 1000);
	timeout = usecs_to_jiffies((uint32_t)time);
	ret = wait_event_interruptible_timeout(
		client->wait,
		dit->state == DIT_STATE_IDLE,
		timeout);
	spin_lock_irqsave(&dit->work_lock, flags);
	if (dit->state != DIT_STATE_IDLE) {
		list_del(&client->work_node);
		client->state = DIT_CLIENT_STATE_IDLE;
		spin_unlock_irqrestore(&dit->work_lock, flags);
		DRM_ERROR("wait2start timeout. timeout=%lujif, ret=%lu\n",
			timeout, ret);
		return -1;
	}
	dit->state = DIT_STATE_BUSSY;
	client->state = DIT_CLIENT_STATE_PREPARE;
	spin_unlock_irqrestore(&dit->work_lock, flags);

	return 0;
}

static int dit_client_put_device(struct dit_context *dit,
	struct dit_client *client)
{
	unsigned long flags;

	DRM_DEBUG("%d\n", __LINE__);

	spin_lock_irqsave(&dit->work_lock, flags);
	list_del(&client->work_node);
	dit->state = DIT_STATE_IDLE;
	if (!list_empty(&dit->work_list)) {
		struct dit_client *next = list_first_entry(&dit->work_list,
			struct dit_client, work_node);
		wake_up_interruptible(&next->wait);
	}
	spin_unlock_irqrestore(&dit->work_lock, flags);

	DRM_DEBUG("%d\n", __LINE__);

	return 0;
}

static int dit_client_exec_fb_plane(struct dit_context *dit,
	struct dit_client *client,
	struct drm_lombo_dit_exec_arg *exec_arg, int plane)
{
	uint64_t time;
	long timeout, ret;
	unsigned long flags;
	uint32_t irq_status;
	uint32_t timer_status;

	__dit_field_seq_t field_seq;
	__dit_field_pol_t field_pol;
	struct drm_framebuffer *fb;
	struct drm_gem_cma_object *buf;
	unsigned long long frame_addr[3] = {0, 0, 0};
	unsigned long long field_addr[4] = {0, 0, 0};
	uint32_t width, height, pitch; /* for in fb */
	int cpp;
	unsigned long long out_addr;
	uint32_t out_pitch;

	csp_dit_set_still_threshold(16);
	csp_dit_set_interpolation_threshold(200, 5, 20, 20);
	csp_dit_set_pixseq(0);
	csp_dit_enable_still_avg();

	field_seq = exec_arg->top_filed_first ? DIT_TFF_SEQ : DIT_BFF_SEQ;
	field_pol = exec_arg->frame_seq ? DIT_SECOND_FIELD : DIT_FIRST_FIELD;

	fb = client->in_fb0;
	buf = drm_fb_cma_get_gem_obj(fb, plane);
	if (buf == NULL) {
		DRM_ERROR("%d\n", __LINE__);
		return -EINVAL;
	}
	frame_addr[0] = buf->paddr + fb->offsets[plane];
	pitch = fb->pitches[plane];
	if (plane > 0) {
		width = fb->width / fb->format->hsub;
		height = fb->height / fb->format->vsub;
	} else {
		width = fb->width;
		height = fb->height;
	}
	cpp = fb->format->cpp[plane];
	if (cpp > 1)
		width = width * cpp;

	DRM_DEBUG("plane[%d]: pitch=%d, width=%d, height=%d, cpp=%d\n",
		plane, pitch, width, height, cpp);

	fb = client->in_fb1;
	buf = drm_fb_cma_get_gem_obj(fb, plane);
	if (buf == NULL) {
		DRM_ERROR("%d\n", __LINE__);
		return -EINVAL;
	}
	frame_addr[1] = buf->paddr + fb->offsets[plane];

	if (client->in_fb2) {
		fb = client->in_fb2;
		buf = drm_fb_cma_get_gem_obj(fb, plane);
		if (buf == NULL) {
			DRM_ERROR("%d\n", __LINE__);
			return -EINVAL;
		}
		frame_addr[2] = buf->paddr + fb->offsets[plane];
	}

	csp_dit_cal_field_addr_from_frame_addr(
		frame_addr, field_addr, pitch, field_seq, field_pol);

	fb = client->out_fb0;
	buf = drm_fb_cma_get_gem_obj(fb, plane);
	if (buf == NULL) {
		DRM_ERROR("%d\n", __LINE__);
		return -EINVAL;
	}
	out_addr = buf->paddr + fb->offsets[plane];
	out_pitch = fb->pitches[plane];

	csp_dit_enable();
	csp_dit_set_field_pol(field_pol ? 0 : 1);
	csp_dit_set_mode((cpp == 2) ? 1 : 0);
	csp_dit_set_input_addr(field_addr);
	csp_dit_set_output_addr(out_addr);
	/* width of in&out_size: in bytes, not in pixel  */
	csp_dit_set_insize(width, height);
	csp_dit_set_outsize(width, height); /* outsize as same as insize */
	csp_dit_set_input_linestride(pitch << 1);
	csp_dit_set_output_linestride(out_pitch);

	client->state = DIT_CLIENT_STATE_WAIT4FINISH;
	csp_dit_enable_irq_src(dit->irq_src);
	csp_dit_wb_start();

	time = client->timeout + 999;
	do_div(time, 1000);
	timeout = usecs_to_jiffies((uint32_t)time);
	ret = wait_event_interruptible_timeout(
		client->wait,
		client->state != DIT_CLIENT_STATE_WAIT4FINISH,
		timeout);

	spin_lock_irqsave(&dit->work_lock, flags);
	if (client->state != DIT_CLIENT_STATE_FINISH) {
		csp_dit_wb_stop();
		csp_dit_clear_irq_status(dit->irq_src);
		csp_dit_disable();
		client->state = DIT_CLIENT_STATE_IDLE;
		spin_unlock_irqrestore(&dit->work_lock, flags);
		DRM_ERROR("wait4finish timeout. timeout=%lujif, ret=%lu\n",
			timeout, ret);
		DRM_ERROR("irq_cnt=%u, work_empty_cnt=%u\n",
			dit->irq_cnt, dit->work_empty);
		return -1;
	}
	spin_unlock_irqrestore(&dit->work_lock, flags);

	/* here is DIT_CLIENT_STATE_FINISH */
	timer_status = csp_dit_get_wb_timer_status();
	csp_dit_wb_stop();
	csp_dit_disable_irq_src(dit->irq_src);
	csp_dit_disable();
	irq_status = dit->irq_status;
	dit->irq_status = 0;
	client->state = DIT_CLIENT_STATE_PREPARE;

	DRM_DEBUG("%s,%d: irq_status=0x%x, timer_status=%u\n",
		__func__, __LINE__, irq_status, timer_status);

	/* check the work status */
	if (irq_status & DIT_IRQ_WB_TIMEOUT) {
		DRM_ERROR("%d:dit wb timeout. tmr: sta=%d,thr=%d\n",
			__LINE__, timer_status,
			csp_dit_get_wb_timer_thr());
		csp_dit_reset();
	}
	if (!(irq_status & DIT_IRQ_WB_FINISH)) {
		/* should not happen */
		DRM_ERROR("%d: dit wb not finish !\n",  __LINE__);
	}

	return 0;
}

static int dit_client_exec_fb(struct dit_context *dit,
	struct dit_client *client, struct drm_lombo_dit_exec_arg *exec_arg)
{
	int plane;
	int plane_num = client->in_fb0->format->num_planes;

	for (plane = 0; plane < plane_num; plane++) {
		int ret = dit_client_exec_fb_plane(
			dit, client, exec_arg, plane);
		if (ret)
			return ret;
	}
	return 0;
}

int lombo_dit_exec_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file)
{
	struct lombo_file_private *file_priv = file->driver_priv;
	struct dit_file_private *dit_fpriv = file_priv->dit_fpriv;
	struct dit_context *dit = dit_fpriv->dit;
	struct drm_lombo_dit_exec_arg *exec_arg = data;
	struct dit_client *client;
	int ret = 0;

	DRM_DEBUG("%d\n", __LINE__);

	spin_lock(&dit->idr_lock);
	client = idr_find(&dit->client_idr, exec_arg->handle);
	if (client == NULL) {
		spin_unlock(&dit->idr_lock);
		return -EINVAL;
	}
	spin_unlock(&dit->idr_lock);

	ret = dit_client_declare_work(client);
	if (ret)
		return ret;

	ret = dit_client_get_fbs(drm, client, exec_arg);
	if (ret)
		goto out;

	ret = dit_client_get_device(dit, client);
	if (ret)
		goto out;

	ret = dit_client_exec_fb(dit, client, exec_arg);

	dit_client_put_device(dit, client);

out:
	/* guarantee unreference all fbs */
	dit_client_put_fbs(drm, client);

	dit_client_undeclare_work(client);

	return ret;
}

static irqreturn_t dit_irq_handler(int irq, void *dev_id)
{
	struct dit_context *dit = dev_id;
	struct dit_client *client;
	unsigned long flags;
	int work_empty = 0;

	dit->irq_cnt++;

	spin_lock_irqsave(&dit->work_lock, flags);
	work_empty = list_empty(&dit->work_list);
	if (!work_empty) {
		client = list_first_entry(&dit->work_list,
			struct dit_client, work_node);
		dit->irq_status = csp_dit_get_irq_status();
		csp_dit_clear_irq_status(dit->irq_status);
		client->state = DIT_CLIENT_STATE_FINISH;
		wake_up_interruptible(&client->wait);
	} else {
		dit->work_empty++;
	}
	spin_unlock_irqrestore(&dit->work_lock, flags);

	return IRQ_HANDLED;
}

static int dit_subdrv_probe(struct drm_device *drm, struct device *dev)
{
	return 0;
}

static void dit_subdrv_remove(struct drm_device *drm, struct device *dev)
{
	return;
}

static int dit_subdrv_open(struct drm_device *drm, struct device *dev,
		struct drm_file *file)
{
	struct dit_context *dit = dev_get_drvdata(dev);
	struct dit_file_private *dit_fpriv;
	struct lombo_file_private *file_priv = file->driver_priv;
	int ret = 0;

	dit_fpriv = kzalloc(sizeof(*dit_fpriv), GFP_KERNEL);
	if (!dit_fpriv) {
		DRM_ERROR("%d\n", __LINE__);
		return -ENOMEM;
	}

	dit_fpriv->dit = dit;
	spin_lock_init(&dit_fpriv->client_slock);
	INIT_LIST_HEAD(&dit_fpriv->client_list);

	file_priv->dit_fpriv = dit_fpriv;

	return ret;
}

static void dit_subdrv_close(struct drm_device *drm, struct device *dev,
		struct drm_file *file)
{
	struct lombo_file_private *file_priv = file->driver_priv;
	struct dit_file_private *dit_fpriv = file_priv->dit_fpriv;
	struct dit_context *dit = dit_fpriv->dit;
	uint32_t rm_cnt_l = 0;
	uint32_t rm_cnt_i = 0;
	uint32_t client_cnt = 0;

	spin_lock(&dit_fpriv->client_slock);
	while (!list_empty(&dit_fpriv->client_list)) {
		struct dit_client *client_l; /* client in list */
		struct dit_client *client_i; /* client in idr */

		client_l = list_first_entry(&dit_fpriv->client_list,
			struct dit_client, file_node);
		list_del(&client_l->file_node);
		spin_unlock(&dit_fpriv->client_slock);
		rm_cnt_l++;

		spin_lock(&dit->idr_lock);
		client_i = idr_find(&dit->client_idr, client_l->id);
		if ((client_i != NULL) && (client_i == client_l)) {
			idr_remove(&dit->client_idr, client_l->id);
			client_cnt = dit->client_cnt;
			dit->client_cnt--;
			rm_cnt_i++;
		}
		spin_unlock(&dit->idr_lock);

		kfree(client_l);
		dit_disable(dit);

		spin_lock(&dit_fpriv->client_slock);
	}
	spin_unlock(&dit_fpriv->client_slock);

	kfree(dit_fpriv);
	file_priv->dit_fpriv = NULL;

	if ((rm_cnt_i > 0) || (rm_cnt_l > 0))
		DRM_ERROR("dit client leak: rm_cnt[%d,%d] of cnt=%d\n",
			rm_cnt_l, rm_cnt_i, client_cnt);

	return;
}

static int dit_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct device_node *top;
	struct platform_device *top_pdev;
	struct dit_context *dit;
	struct lombo_subdrv *subdrv;
	struct resource *res;
	int ret = 0;
	u32 value;

	dev_info(dev, "dit probe start\n");

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

	dit = kzalloc(sizeof(*dit), GFP_KERNEL);
	if (!dit) {
		dev_err(dev, "failed to alloc for dit ctx\n");
		return -ENOMEM;
	}
	dit->dpu_top = &top_pdev->dev;

	/* parse register */
	dit->regs_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	dit->regs = devm_ioremap_resource(dev, dit->regs_res);
	if (IS_ERR(dit->regs)) {
		dev_err(dev, "failed to get reg for dit\n");
		return PTR_ERR(dit->regs);
	}
	csp_dit_set_register_base((uintptr_t)dit->regs,
		resource_size(dit->regs_res));

	/* parse irq */
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (res == NULL) {
		dev_err(dev, "get interrupt resource failed.\n");
		return -ENXIO;
	}
	ret = devm_request_irq(dev, res->start,
		dit_irq_handler, 0, "lombo_dit", dit);
	if (ret) {
		dev_err(dev, "request interrupt failed.\n");
		return ret;
	}
	dit->irq = res->start;
	dit->irq_src = DIT_IRQ_SRC_WB_FINISH
		| DIT_IRQ_SRC_WB_OVERFLOW
		| DIT_IRQ_SRC_WB_TIMEOUT;
	pr_debug("dit irqno=%d, irq_src=0x%x\n",
		dit->irq, dit->irq_src);

	/* get clock */
	dit->clk = lombo_dpu_get_clk(dev, dit->dpu_top, DPU_MOD_DIT);
	if (!dit->clk) {
		dev_err(dev, "get clk for dit failed\n");
		return -ENODEV;
	}
	if (!of_property_read_u32(np, "clk_rate", &value))
		lombo_dpu_clk_set_rate(dit->clk, value);
	else
		DRM_ERROR("failed to of get dit clk_rate\n");
	if (!of_property_read_u32(np, "sclk", &value))
		lombo_dpu_clk_set_parent(dit->clk, value);
	else
		lombo_dpu_clk_set_parent(dit->clk, 0);

	idr_init(&dit->client_idr);
	spin_lock_init(&dit->idr_lock);

	spin_lock_init(&dit->work_lock);
	INIT_LIST_HEAD(&dit->work_list);

	spin_lock_init(&dit->en_lock);

	subdrv = &dit->subdrv;
	subdrv->dev = dev;
	subdrv->probe = dit_subdrv_probe;
	subdrv->remove = dit_subdrv_remove;
	subdrv->open = dit_subdrv_open;
	subdrv->close = dit_subdrv_close;

	ret = dev_set_drvdata(dev, dit);
	if (ret) {
		dev_err(dev, "failed to set drvdata\n");
		return ret;
	}

	ret = lombo_subdrv_register(subdrv);
	if (ret < 0)
		DRM_ERROR("failed to register lombo dit device.\n");

	dev_info(dev, "lombo dit registered successfully.\n");

	return ret;
}

static int dit_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct dit_context *dit = dev_get_drvdata(dev);
	uint32_t client_cnt = dit->client_cnt;
	int ret = 0;

	dev_info(dev, "dit remove\n");

	if (client_cnt > 0)
		dev_err(dev, "remain dit %d client!\n", client_cnt);

	ret = lombo_subdrv_unregister(&dit->subdrv);
	if (ret)
		dev_err(dev, "dit subdrv unregister failed\n");

	lombo_dpu_put_clk(&dit->clk);

	csp_dit_set_register_base((uintptr_t)0, 0);
	dev_set_drvdata(dev, NULL);
	kfree(dit);

	return 0;
}

int lombo_dit_pm_suspend(struct device *dev)
{
	struct dit_context *dit = dev_get_drvdata(dev);

	if (dit->state == DIT_STATE_BUSSY)
		DRM_ERROR("bussy to suspend\n");

	return _dit_suspend(dit);
}

int lombo_dit_pm_resume(struct device *dev)
{
	struct dit_context *dit = dev_get_drvdata(dev);

	if (dit->client_cnt)
		return _dit_resume(dit);
	else
		return 0;
}

static const struct dev_pm_ops lombo_dit_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(lombo_dit_pm_suspend, lombo_dit_pm_resume)
};

static struct of_device_id lombo_dit_of_table[] = {
	{ .compatible = "lombo,lombo-n7v0-dit" },
	{ .compatible = "lombo,lombo-n7v1-dit" },
	{ .compatible = "lombo,lombo-n8v0-dit" },
	{ }
};
MODULE_DEVICE_TABLE(of, lombo_dit_of_table);

struct platform_driver lombo_dit_platform_driver = {
	.driver = {
		.name = "lombo-dit",
		.owner = THIS_MODULE,
		.of_match_table = lombo_dit_of_table,
		.pm = &lombo_dit_pm_ops,
	},
	.probe = dit_probe,
	.remove = dit_remove,
};
module_platform_driver(lombo_dit_platform_driver);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Lombo SoC DPU De-Interlace Driver");
MODULE_LICENSE("GPL");
