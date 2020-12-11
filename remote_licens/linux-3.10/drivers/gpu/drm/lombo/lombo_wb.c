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

#include <linux/module.h>
#include <linux/component.h>
#include <linux/of_platform.h>
#include <linux/workqueue.h>

#include <drm/drmP.h>
#include <drm/drm_crtc.h>
#include <drm/drm_fb_cma_helper.h>
#include <drm/drm_gem_cma_helper.h>
#include <drm/lombo_drm.h>

#include "lombo_subdrv.h"
#include "lombo_dpu.h"
#include "lombo_crtc.h"
#include "lombo_dc.h"
#include "csp_dpu_wb.h"

#define WB_RESAMPLE_MAX_WIDTH 720

enum {
	LOMBO_WB_STATE_STOP = 0,
	LOMBO_WB_STATE_START, /* 1, worker start */
	LOMBO_WB_STATE_WAIT_FINISH_HAS_NEXT, /* 2 */
	LOMBO_WB_STATE_WAIT_FINISH_NO_NEXT, /* 3 */
	LOMBO_WB_STATE_WORK_FINISH_HAS_NEXT, /* 4 */
	LOMBO_WB_STATE_WORK_FINISH_NO_NEXT, /* 5 */
	LOMBO_WB_STATE_WORK_FINISH2_NO_NEXT, /* 6 */
	LOMBO_WB_STATE_WORK_FINISH_ERROR, /* 7 */
};

enum {
	LOMBO_WB_IDLE_QUEUE = 0,
	LOMBO_WB_DEAL_QUEUE,
	LOMBO_WB_DONE_QUEUE,
	LOMBO_WB_FB_QUEUE_NUM,
};

struct lombo_wb_fb {
	struct drm_framebuffer *fb;
	uint64_t user_priv;
	int release_fence;
	uint32_t state;
	struct list_head node;
};

struct lombo_wb_fb_pool {
	struct lombo_wb_fb *array;
	uint32_t array_size;
	uint32_t cur_num; /* cur num fbs requested */
	uint32_t max_num; /* max num fbs for request */
};

struct lombo_wb {
	struct lombo_subdrv subdrv;
	struct device *dpu_top;

	struct lombo_dpu_clk *mod_clk;
	void __iomem *reg;
	int irq; /* irq no */

	/* fb queue */
	spinlock_t qlock; /* lock of fb queue */
	struct list_head fb_queue[LOMBO_WB_FB_QUEUE_NUM];
	struct lombo_wb_fb_pool fb_pool;
	wait_queue_head_t fb_wait;
	int wait_fb;

	/* user paras setting */
	uint32_t src_pipe;
	dpu_rectangle_t src_crop;
	uint32_t dst_width;
	uint32_t dst_height;
	/* inner paras setting */
	uint32_t src_width;
	uint32_t src_height;
	uint32_t src_csfmt;

	struct drm_lombo_wb_start_arg start_arg;
	int state;
	int irq_state;
	int irq_cnt;
	int work_quit;
	wait_queue_head_t work_wait;
	spinlock_t wlock; /* work lock */
	struct workqueue_struct *workq;
	struct work_struct work;
};

struct lombo_wb_file_private {
	struct lombo_wb *wb;
};

static int lombo_wb_convert_fmt(int fmt)
{
	switch (fmt) {
	case DRM_FORMAT_ARGB8888:
		return DPU_FMT_ARGB8888;
	case DRM_FORMAT_RGB565:
		return DPU_FMT_RGB565;
	case DRM_FORMAT_NV12:
		return DPU_FMT_YUV420SP;
	case DRM_FORMAT_NV16:
		return DPU_FMT_YUV422SP;
	default:
		DRM_ERROR("unknown fmt=0x%x\n", fmt);
		return DPU_FMT_ARGB8888;
	}
}

static inline struct lombo_wb *lombo_wb_get_from_file(
	struct drm_file *file)
{
	struct lombo_file_private *file_priv = file->driver_priv;
	struct lombo_wb_file_private *wb_fpriv = file_priv->wb_fpriv;

	return wb_fpriv->wb;
}

static int lombo_wb_reset_fb_pool(struct lombo_wb *wb, uint32_t buf_cnt)
{
	struct lombo_wb_fb_pool *pool = &wb->fb_pool;
	struct lombo_wb_fb *array;
	int i;

	if (buf_cnt <= pool->array_size) {
		if (buf_cnt < pool->cur_num)
			DRM_DEBUG_KMS("cur_num=%d more than buf_cnt=%d\n",
				pool->cur_num, buf_cnt);
		pool->max_num = buf_cnt;
		return 0;
	}

	array = kzalloc(buf_cnt * sizeof(*array), GFP_KERNEL);
	if (!array) {
		DRM_ERROR("failed to alloc fb array, count=%d\n", buf_cnt);
		return -ENOMEM;
	}

	if (pool->array_size > 0)
		memcpy((void *)array, (void *)pool->array,
			pool->array_size * sizeof(*array));
	for (i = 0; i < pool->array_size; i++) {
		INIT_LIST_HEAD(&(array[i].node));
		if (!list_empty(&(pool->array[i].node)))
			list_replace(&(pool->array[i].node), &(array[i].node));
	}
	for (i = pool->array_size; i < buf_cnt; i++) {
		array[i].release_fence = -1;
		INIT_LIST_HEAD(&(array[i].node));
	}

	kfree(pool->array);
	pool->array = array;
	pool->array_size = buf_cnt;
	pool->max_num = buf_cnt;

	DRM_DEBUG_KMS("fb array resize %u\n", buf_cnt);

	return 0;
}

static int lombo_wb_fb_alloc(struct lombo_wb *wb, struct lombo_wb_fb **fb)
{
	struct lombo_wb_fb_pool *pool = &wb->fb_pool;
	struct lombo_wb_fb *array = pool->array;
	int i;

	if (pool->cur_num >= pool->max_num) {
		DRM_ERROR("cur_num(%d) >= max_num(%d)\n",
			pool->cur_num, pool->max_num);
		return -EOVERFLOW;
	}
	for (i = 0; i < pool->max_num; i++) {
		if (list_empty(&(array[i].node))) {
			*fb = array + i;
			pool->cur_num++;
			return 0;
		}
	}
	DRM_ERROR("no availiabe fb in pool\n");
	return -ENOSPC;
}

static int lombo_wb_fb_free(struct lombo_wb *wb, struct lombo_wb_fb *fb)
{
	struct lombo_wb_fb_pool *pool = &wb->fb_pool;

	memset((void *)fb, 0, sizeof(*fb));
	fb->release_fence = -1;
	INIT_LIST_HEAD(&(fb->node));
	pool->cur_num--;

	return 0;
}

int lombo_wb_set_para_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file)
{
	struct lombo_wb *wb = lombo_wb_get_from_file(file);
	struct drm_lombo_wb_set_para *para = data;
	int ret = 0;

	if (wb->state != LOMBO_WB_STATE_STOP) {
		DRM_ERROR("can not set para when wb start\n");
		return -EBUSY;
	}

	switch (para->key) {
	case LOMBO_WB_PARA_KEY_SRC_PIPE:
		wb->src_pipe = para->value;
		break;
	case LOMBO_WB_PARA_KEY_SRC_CROP_X:
		wb->src_crop.x = para->value;
		break;
	case LOMBO_WB_PARA_KEY_SRC_CROP_Y:
		wb->src_crop.y = para->value;
		break;
	case LOMBO_WB_PARA_KEY_SRC_CROP_W:
		wb->src_crop.w = para->value;
		break;
	case LOMBO_WB_PARA_KEY_SRC_CROP_H:
		wb->src_crop.h = para->value;
		break;
	case LOMBO_WB_PARA_KEY_DST_WIDTH:
		wb->dst_width = para->value;
		break;
	case LOMBO_WB_PARA_KEY_DST_HEIGHT:
		wb->dst_height = para->value;
		break;
	case LOMBO_WB_PARA_KEY_CREATE_FB_QUEUE:
		ret = lombo_wb_reset_fb_pool(wb, (uint32_t)para->value);
		break;
	default:
		ret = -ENOKEY;
		break;
	}
	DRM_DEBUG_KMS("key=%d, value=%lld, ret=%d\n",
		para->key, para->value, ret);

	return ret;
}

int lombo_wb_queue_buf_ioctl(struct drm_device *drm, void *data,
	struct drm_file *file)
{
	struct lombo_wb *wb = lombo_wb_get_from_file(file);
	struct drm_lombo_wb_queue_buf *que_buf = data;
	struct drm_framebuffer *fb = NULL;
	struct lombo_wb_fb *wb_fb = NULL;
	int ret = 0;

	/* TODO: check fb id */
	fb = drm_framebuffer_lookup(drm, que_buf->fb);
	if (!fb) {
		DRM_ERROR("invalid fb_id=%d\n", que_buf->fb);
		return -EINVAL;
	}

	spin_lock(&wb->qlock);
	ret = lombo_wb_fb_alloc(wb, &wb_fb);
	if (ret) {
		spin_unlock(&wb->qlock);
		drm_framebuffer_unreference(fb);
		return ret;
	}

	wb_fb->fb = fb;
	wb_fb->release_fence = que_buf->release_fence;
	wb_fb->user_priv = que_buf->user_priv;
	wb_fb->state = LOMBO_WB_FB_STATE_IDLE;
	list_add_tail(&(wb_fb->node), &(wb->fb_queue[LOMBO_WB_IDLE_QUEUE]));

	if (wb->wait_fb) {
		wb->wait_fb = 0;
		wake_up_interruptible(&wb->fb_wait);
	}
	spin_unlock(&wb->qlock);

	DRM_DEBUG_KMS("queued fb=%d\n", que_buf->fb);

	return 0;
}

int lombo_wb_dequeue_buf_ioctl(struct drm_device *drm,
	void *data, struct drm_file *file)
{
	struct lombo_wb *wb = lombo_wb_get_from_file(file);
	struct drm_lombo_wb_dequeue_buf *deq_buf = data;
	struct drm_framebuffer *fb = NULL;
	struct lombo_wb_fb *wb_fb = NULL;
	int fb_id, id, wb_state;

	if (deq_buf->acquire_fence >= 0)
		DRM_ERROR("not support acquire_fence yet\n");

	spin_lock(&wb->qlock);

	for (id = LOMBO_WB_FB_QUEUE_NUM - 1; id >= 0; id--) {
		wb_fb = list_first_entry_or_null(&(wb->fb_queue[id]),
						 struct lombo_wb_fb, node);
		if (wb_fb)
			break;
	}
	if (!wb_fb) {
		spin_unlock(&wb->qlock);
		DRM_ERROR("no fb for dequeue\n");
		return -ENODATA;
	}
	fb_id = wb_fb->fb->base.id;

	deq_buf->fb = 0;
	deq_buf->fb_state = wb_fb->state;
	deq_buf->acquire_fence = -1;
	deq_buf->user_priv = wb_fb->user_priv;

	wb_state = wb->state;
	if ((wb_state == LOMBO_WB_STATE_STOP) ||
		(id == LOMBO_WB_DONE_QUEUE)) {
		fb = wb_fb->fb;
		list_del(&(wb_fb->node));
		lombo_wb_fb_free(wb, wb_fb);
	}

	spin_unlock(&wb->qlock);

	if (fb) {
		deq_buf->fb = fb->base.id;
		drm_framebuffer_unreference(fb);
		DRM_DEBUG_KMS("dequeued: fb=%d, fb_state=%d, wb_state=%d\n",
			      deq_buf->fb, deq_buf->fb_state, wb_state);
	} else {
		DRM_DEBUG_KMS("no-dequeued: fb=%d, fb_state=%d, wb_state=%d\n",
			      fb_id, deq_buf->fb_state, wb_state);
	}

	return 0;
}

/* dequeue idle_fb from idle_queue, and queue to deal_queue */
static struct lombo_wb_fb *lombo_wb_get_idle_fb(
	struct lombo_wb *wb, int wait_ms)
{
	struct list_head *idle_queue = &(wb->fb_queue[LOMBO_WB_IDLE_QUEUE]);
	struct list_head *deal_queue = &(wb->fb_queue[LOMBO_WB_DEAL_QUEUE]);
	struct lombo_wb_fb *wb_fb;
	long ret, timeout;

	spin_lock(&wb->qlock);
	wb_fb = list_first_entry_or_null(idle_queue, struct lombo_wb_fb, node);
	if (wb_fb) {
		list_del(&(wb_fb->node));
		list_add_tail(&(wb_fb->node), deal_queue);
		spin_unlock(&wb->qlock);
		DRM_DEBUG_KMS("get idle fb=%d\n", wb_fb->fb->base.id);
		return wb_fb;
	}
	if (!wait_ms) {
		spin_unlock(&wb->qlock);
		DRM_DEBUG_KMS("no wait at not get idle fb\n");
		return NULL;
	}
	wb->wait_fb = 1;
	spin_unlock(&wb->qlock);

	timeout = msecs_to_jiffies((unsigned int)wait_ms);
	ret = wait_event_interruptible_timeout(wb->fb_wait,
		wb->wait_fb == 0, timeout);
	DRM_DEBUG_KMS("wakeup: wait_fb=%d, ret=%ld, timeout=%ld\n",
		      wb->wait_fb, ret, timeout);
	return lombo_wb_get_idle_fb(wb, 0);
}

static inline void lombo_wb_queue_done_fb(
	struct lombo_wb *wb, struct lombo_wb_fb *wb_fb)
{
	spin_lock(&wb->qlock);
	list_del(&wb_fb->node);
	list_add_tail(&wb_fb->node, &wb->fb_queue[LOMBO_WB_DONE_QUEUE]);
	spin_unlock(&wb->qlock);
}

static int lombo_wb_enable(struct lombo_wb *wb)
{
	lombo_dpu_clk_prepare_enable(wb->mod_clk, 0);
	csp_wb_init();
	return 0;
}

static int lombo_wb_disable(struct lombo_wb *wb)
{
	csp_wb_exit();
	lombo_dpu_clk_disable_unprepare(wb->mod_clk);
	return 0;
}

static int lombo_wb_config_dst(
	struct drm_framebuffer *fb, dpu_fb_t *get_res)
{
	const struct drm_format_info *format = fb->format;
	dpu_fb_t dpu_fb;
	int i;

	DRM_DEBUG_KMS("fb_id=%d\n", fb->base.id);

	dpu_fb.format = lombo_wb_convert_fmt(format->format);
	dpu_fb.planes = format->num_planes;
	for (i = 0; i < dpu_fb.planes; i++) {
		struct drm_gem_cma_object *cma_obj;

		cma_obj = drm_fb_cma_get_gem_obj(fb, i);

		if (i == 0) {
			dpu_fb.width[i] = fb->width;
			dpu_fb.height[i] = fb->height;
		} else {
			dpu_fb.width[i] = fb->width / format->hsub;
			dpu_fb.height[i] = fb->height / format->vsub;
		}
		dpu_fb.linestride_bit[i] = fb->pitches[i] << 3;
		dpu_fb.addr[i] = cma_obj->paddr + fb->offsets[i];
		dpu_fb.addr[i] <<= 3; /* in bit unit */
		DRM_DEBUG_KMS("dpu_fb(%d):%dx%d, lst=%d\n", i,
			dpu_fb.width[i], dpu_fb.height[i],
			dpu_fb.linestride_bit[i]);
		DRM_DEBUG_KMS("dpu_fb(%d):paddr=0x%llx,ofs=%d, addr=0x%llx\n",
			i, (uint64_t)cma_obj->paddr,
			fb->offsets[i], dpu_fb.addr[i]);
	}
	csp_wb_set_buffer(&dpu_fb);
	csp_wb_set_dst_size(dpu_fb.width, dpu_fb.height);

	if (get_res)
		memcpy((void *)get_res, (void *)&dpu_fb, sizeof(*get_res));

	return 0;
}

static int lombo_wb_config(struct lombo_wb *wb,
	struct drm_framebuffer *fb)
{
	const struct drm_format_info *format;
	int dst_csfmt;
	dpu_csc_mode_t csc_mode = DPU_CSC_BYPASS;
	int resample_enable = 0;
	dpu_fb_t dpu_fb;

	format = fb->format;

	csp_wb_set_runing_mode(0);
	/* csp_wb_set_field_state(field_sta);  for interlace output */

	/* src */
	DRM_DEBUG_KMS("SRC:(%dx%d),crop[%d,%d,%d,%d]\n",
		wb->src_width, wb->src_height,
		wb->src_crop.x, wb->src_crop.y,
		wb->src_crop.w, wb->src_crop.h);
	csp_wb_set_src_size(wb->src_width, wb->src_height);
	csp_wb_set_crop(&wb->src_crop);

	/* dst */
	lombo_wb_config_dst(fb, &dpu_fb);

	/* csc */
	dst_csfmt = format->is_yuv ? LOMBO_CSFMT_YUV : LOMBO_CSFMT_RGB;
	if (wb->src_csfmt == dst_csfmt)
		csc_mode = DPU_CSC_BYPASS;
	else if ((wb->src_csfmt == LOMBO_CSFMT_RGB) &&
		(dst_csfmt == LOMBO_CSFMT_YUV))
		csc_mode = DPU_CSC_RGB2YUV;
	else if ((wb->src_csfmt == LOMBO_CSFMT_YUV) &&
		(dst_csfmt == LOMBO_CSFMT_RGB))
		csc_mode = DPU_CSC_YUV2RGB;
	else
		DRM_ERROR("csfmt:%d-->%d\n", wb->src_csfmt, dst_csfmt);
	DRM_DEBUG_KMS("csc_mode=%d\n", csc_mode);
	csp_wb_set_csc_coef(csc_mode, DPU_CS_BT601);

	/* resample */
	if ((wb->src_crop.w != wb->dst_width) ||
		(wb->src_crop.h != wb->dst_height) ||
		(format->hsub > 1) ||
		(format->vsub > 1)) {
		resample_enable = 1;
		if (wb->dst_width > WB_RESAMPLE_MAX_WIDTH)
			DRM_ERROR("resample: dst_width %d larger than %d\n",
				wb->dst_width, WB_RESAMPLE_MAX_WIDTH);
	}
	if (resample_enable) {
		int offset[3] = { 0, 0, 0 };

		DRM_DEBUG_KMS("resample_enable\n");
		csp_wb_set_scaling_xratio(wb->src_crop.w, dpu_fb.width);
		csp_wb_set_scaling_yratio(wb->src_crop.h, dpu_fb.height);
		csp_wb_set_scaling_xcoef(wb->src_crop.w, dpu_fb.width);
		csp_wb_set_scaling_ycoef(wb->src_crop.h, dpu_fb.height);
		csp_wb_set_initial_offset(offset, offset);/* TODO */
		csp_wb_enable_resample();
	} else {
		DRM_DEBUG_KMS("disable resample\n");
		csp_wb_disable_resample();
	}

	csp_wb_enable_irq(WB_IRQ_FINISH);
	return 0;
}

static int lombo_wb_is_finish_state(int state)
{
	return (state == LOMBO_WB_STATE_WORK_FINISH_HAS_NEXT) ||
		(state == LOMBO_WB_STATE_WORK_FINISH_NO_NEXT) ||
		(state == LOMBO_WB_STATE_WORK_FINISH2_NO_NEXT) ||
		(state == LOMBO_WB_STATE_WORK_FINISH_ERROR);
}

static void lombo_wb_worker(struct work_struct *work)
{
	struct lombo_wb *wb = container_of(work, struct lombo_wb, work);
	struct lombo_wb_fb *cur, *next = NULL;
	uint32_t count = wb->start_arg.count;
	unsigned long flags;
	long ret_wait, timeout;
	int need_quit = 0;

	cur = lombo_wb_get_idle_fb(wb, 1000);
	if (!cur) {
		spin_unlock(&wb->qlock);
		DRM_ERROR("no wb fb in queue at beginning\n");
		return;
	}
	lombo_wb_config(wb, cur->fb);
	cur->state = LOMBO_WB_FB_STATE_WRITING;
	wb->state = LOMBO_WB_STATE_WAIT_FINISH_NO_NEXT;
	csp_wb_load_dbr();
	csp_wb_start();

	DRM_DEBUG_KMS("go cfg_next\n");

cfg_next:
	if (wb->work_quit) {
		DRM_DEBUG_KMS("worker need quit\n");
		need_quit = 1;
		goto wait_finish;
	}
	if (wb->start_arg.count != 0) {
		count--;
		if (count == 0) {
			DRM_DEBUG_KMS("work to be finished to quit\n");
			need_quit = 1;
			goto wait_finish;
		}
	}

	/* go to config next wb fb of dst */
	if (!next) {
		next = lombo_wb_get_idle_fb(wb, 3000);
		if (wb->work_quit || !next) {
			DRM_ERROR("%s next wb_fb\n", next ? "has" : "no");
			need_quit = 1;
			goto wait_finish;
		}
	}
	lombo_wb_config_dst(next->fb, NULL);

	spin_lock_irqsave(&wb->wlock, flags);
	if (wb->state == LOMBO_WB_STATE_WAIT_FINISH_NO_NEXT) {
		/* no irq happen, set as _WAIT_FINISH_HAS_NEXT */
		wb->state = LOMBO_WB_STATE_WAIT_FINISH_HAS_NEXT;
	} else if (wb->state == LOMBO_WB_STATE_WORK_FINISH_NO_NEXT) {
		/* irq happened ! */
		wb->state = LOMBO_WB_STATE_WAIT_FINISH_NO_NEXT;
		spin_unlock_irqrestore(&wb->wlock, flags);
		cur->state = LOMBO_WB_FB_STATE_WRITTEN;
		lombo_wb_queue_done_fb(wb, cur);
		next->state = LOMBO_WB_FB_STATE_WRITING;
		cur = next;
		next = NULL;
		DRM_INFO("%s: skip wb irq happen, irq_cnt=%d\n",
			 __func__, wb->irq_cnt);
		csp_wb_load_dbr();
		csp_wb_start();
		goto cfg_next;
	} else {
		int cur_state = wb->state;

		spin_unlock_irqrestore(&wb->wlock, flags);
		DRM_ERROR("invalid wb state=%d\n", cur_state);
		need_quit = 1;
		goto wait_finish;
	}
	spin_unlock_irqrestore(&wb->wlock, flags);

wait_finish:
	DRM_DEBUG_KMS("wait state %d\n", wb->state);
	timeout = msecs_to_jiffies(3000); /* wait 3s */
	ret_wait = wait_event_interruptible_timeout(wb->work_wait,
			lombo_wb_is_finish_state(wb->state), timeout);
	DRM_DEBUG_KMS("wait finish state %d, irq status %d\n",
		wb->state, wb->irq_state);

	spin_lock_irqsave(&wb->wlock, flags);

	if ((wb->state == LOMBO_WB_STATE_WORK_FINISH_ERROR) ||
		!lombo_wb_is_finish_state(wb->state))
		need_quit = 1;
	if (need_quit) {
		int wb_state = wb->state;

		csp_wb_disable_irq(WB_IRQ_FINISH);
		csp_wb_clear_irq(WB_IRQ_FINISH);
		wb->state = LOMBO_WB_STATE_START;
		spin_unlock_irqrestore(&wb->wlock, flags);
		if ((wb_state == LOMBO_WB_STATE_WORK_FINISH_ERROR) ||
			!lombo_wb_is_finish_state(wb_state))
			DRM_ERROR("%d:quit err:cur_state=%d,irq_status=%d\n",
				__LINE__, wb_state, wb->irq_state);
		else
			DRM_DEBUG_KMS("goto quit without err\n");
		goto quit;
	}

	if (wb->state == LOMBO_WB_STATE_WORK_FINISH_HAS_NEXT) {
		wb->state = LOMBO_WB_STATE_WAIT_FINISH_NO_NEXT;
		spin_unlock_irqrestore(&wb->wlock, flags);
		cur->state = LOMBO_WB_FB_STATE_WRITTEN;
		lombo_wb_queue_done_fb(wb, cur);
		next->state = LOMBO_WB_FB_STATE_WRITING;
		cur = next;
		next = NULL;
	} else {
		int wb_state = wb->state;
		int irq_cnt = wb->irq_cnt;

		spin_unlock_irqrestore(&wb->wlock, flags);
		DRM_ERROR("fixme: wb_state=%d, irq_cnt=%d\n",
			  wb_state, irq_cnt);
	}

	goto cfg_next;

quit:
	/* wb reset */
	csp_wb_disable();
	csp_wb_enable();

}

static irqreturn_t lombo_wb_irq_handler(int irq, void *dev_id)
{
	struct lombo_wb *wb = dev_id;
	unsigned long flags;
	int status = 0;

	if (irq != wb->irq)
		return IRQ_NONE;

	csp_wb_get_status(&status);
	if (!csp_wb_query_and_clear_irq(WB_IRQ_FINISH))
		return IRQ_NONE;

	spin_lock_irqsave(&wb->wlock, flags);
	wb->irq_cnt++;
	wb->irq_state = status;
	if (status) {
		wb->state = LOMBO_WB_STATE_WORK_FINISH_ERROR;
	} else if (wb->state == LOMBO_WB_STATE_WAIT_FINISH_NO_NEXT) {
		wb->state = LOMBO_WB_STATE_WORK_FINISH_NO_NEXT;
	} else if (wb->state == LOMBO_WB_STATE_WAIT_FINISH_HAS_NEXT) {
		csp_wb_load_dbr();
		csp_wb_start();
		wb->state = LOMBO_WB_STATE_WORK_FINISH_HAS_NEXT;
	} else if (wb->state == LOMBO_WB_STATE_WORK_FINISH_HAS_NEXT) {
		wb->state = LOMBO_WB_STATE_WORK_FINISH2_NO_NEXT;
	}
	wake_up_interruptible(&wb->work_wait);
	spin_unlock_irqrestore(&wb->wlock, flags);

	return IRQ_HANDLED;
}

static int lombo_wb_get_check_para(struct lombo_wb *wb,
	struct drm_device *drm)
{
	struct lombo_crtc *ecrtc = NULL;
	struct drm_crtc *crtc;

	if (!wb->dst_width || !wb->dst_height) {
		DRM_ERROR("dst_width=%d, dst_height=%d\n",
			wb->dst_width, wb->dst_height);
		return -EINVAL;
	}

	/* get ecrtc by pipe */
	list_for_each_entry(crtc, &drm->mode_config.crtc_list, head) {
		if (crtc->index == wb->src_pipe) {
			ecrtc = drm_crtc_to_lombo_crtc(crtc);
			break;
		}
	}
	if (!ecrtc) {
		DRM_ERROR("not find crtc(%d)\n", wb->src_pipe);
		return -ENODEV;
	}
	if (!ecrtc->dc) {
		DRM_ERROR("not find dc on crtc(%d)\n", wb->src_pipe);
		return -ENODEV;
	}
	lombo_dc_get_scn(ecrtc->dc,
			&(wb->src_width),
			&(wb->src_height));
	if (!wb->src_width || !wb->src_height) {
		DRM_ERROR("src_w=%d, src_h=%d\n",
			wb->src_width, wb->src_height);
		return -EINVAL;
	}
	wb->src_csfmt = lombo_dc_get_csfmt(ecrtc->dc);

	if ((wb->src_crop.w == 0) ||
		(wb->src_crop.w > wb->src_width) ||
		(wb->src_crop.x > wb->src_width - wb->src_crop.w) ||
		(wb->src_crop.h == 0) ||
		(wb->src_crop.h > wb->src_height) ||
		(wb->src_crop.y > wb->src_height - wb->src_crop.h)) {
		DRM_INFO("fix invalid src_crop(%d,%d,%d,%d) on (%d,%d)\n",
			wb->src_crop.x, wb->src_crop.y,
			wb->src_crop.w, wb->src_crop.h,
			wb->src_width, wb->src_height);
		wb->src_crop.x = 0;
		wb->src_crop.y = 0;
		wb->src_crop.w = wb->src_width;
		wb->src_crop.h = wb->src_height;
	}

	if ((wb->dst_width != wb->src_crop.w) ||
		(wb->dst_height != wb->src_crop.h)) {
		if (wb->dst_width > WB_RESAMPLE_MAX_WIDTH) {
			DRM_ERROR("dst_width %d larger than %d in scale\n",
				wb->dst_width, WB_RESAMPLE_MAX_WIDTH);
			return -EINVAL;
		}
		if ((wb->dst_width > wb->src_crop.w) ||
			(wb->dst_height > wb->src_crop.h)) {
			DRM_ERROR("not support scale up (%dx%d)->(%dx%d)\n",
				wb->dst_width, wb->dst_height,
				wb->src_crop.w, wb->src_crop.h);
			return -EINVAL;
		}
	}

	return 0;
}

int lombo_wb_start_ioctl(struct drm_device *drm,
	void *data, struct drm_file *file)
{
	struct lombo_wb *wb = lombo_wb_get_from_file(file);
	struct drm_lombo_wb_start_arg *start = data;
	unsigned long flags;
	int ret = 0;

	ret = lombo_wb_get_check_para(wb, drm);
	if (ret)
		return ret;

	spin_lock_irqsave(&wb->wlock, flags);
	if (wb->state != LOMBO_WB_STATE_STOP) {
		spin_unlock_irqrestore(&wb->wlock, flags);
		DRM_DEBUG_KMS("wb started already\n");
		return 0;
	}
	wb->state = LOMBO_WB_STATE_START;
	spin_unlock_irqrestore(&wb->wlock, flags);

	lombo_wb_enable(wb);
	wb->start_arg = *start;
	queue_work(wb->workq, &wb->work);

	return 0;
}

int lombo_wb_stop_ioctl(struct drm_device *drm,
	void *data, struct drm_file *file)
{
	struct lombo_wb *wb = lombo_wb_get_from_file(file);
	unsigned long flags;

	spin_lock_irqsave(&wb->wlock, flags);
	if (wb->state == LOMBO_WB_STATE_STOP) {
		spin_unlock_irqrestore(&wb->wlock, flags);
		DRM_DEBUG_KMS("wb stop already\n");
		return 0;
	}
	spin_unlock_irqrestore(&wb->wlock, flags);

	wb->work_quit = 1;
	spin_lock(&wb->qlock);
	if (wb->wait_fb) {
		wb->wait_fb = 0;
		wake_up_interruptible(&wb->fb_wait);
	}
	spin_unlock(&wb->qlock);
	flush_work(&(wb->work));
	wb->work_quit = 0;

	lombo_wb_disable(wb);
	wb->state = LOMBO_WB_STATE_STOP;

	return 0;
}

int lombo_wb_subdrv_open(struct drm_device *drm_dev,
	struct device *dev, struct drm_file *file)
{
	struct lombo_wb *wb = dev_get_drvdata(dev);
	struct lombo_wb_file_private *wb_fpriv;
	struct lombo_file_private *file_priv = file->driver_priv;
	int ret = 0;

	wb_fpriv = kzalloc(sizeof(*wb_fpriv), GFP_KERNEL);
	if (!wb_fpriv) {
		DRM_ERROR("%d\n", __LINE__);
		return -ENOMEM;
	}

	wb_fpriv->wb = wb;
	file_priv->wb_fpriv = wb_fpriv;

	return ret;
}

void lombo_wb_subdrv_close(struct drm_device *drm_dev,
	struct device *dev, struct drm_file *file)
{
	struct lombo_file_private *file_priv = file->driver_priv;
	struct lombo_wb_file_private *wb_fpriv = file_priv->wb_fpriv;

	kfree(wb_fpriv);
	file_priv->wb_fpriv = NULL;
}

static int lombo_wb_bind(struct device *dev,
	struct device *master, void *master_data)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct device_node *np = dev->of_node;

	struct device_node *top;
	struct platform_device *top_pdev;
	struct lombo_wb *wb;
	struct resource *res;
	uint32_t value;
	int ret = 0;
	int i;

	DRM_DEBUG_DRIVER("%s\n", __func__);

	top = of_parse_phandle(np, "top", 0);
	if (!top) {
		DRM_ERROR("failed to parse phandle of top\n");
		return -EINVAL;
	}
	top_pdev = of_find_device_by_node(top);
	if (!top_pdev) {
		DRM_ERROR("failed to find device dpu top\n");
		return -EINVAL;
	}

	wb = kzalloc(sizeof(*wb), GFP_KERNEL);
	if (!wb) {
		dev_err(dev, "alloc for wb failed\n");
		return -ENOMEM;
	}
	wb->dpu_top = &top_pdev->dev;

	/* parse reg addr */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(dev, "get memory resource failed.\n");
		return -ENXIO;
	}
	wb->reg = devm_ioremap(dev, res->start,
			resource_size(res));
	if (wb->reg == NULL) {
		dev_err(dev, "register mapping failed.\n");
		return -ENXIO;
	}
	csp_wb_set_register_base((unsigned long)wb->reg,
				(u32)resource_size(res));

	/* parse irq */
	wb->irq = -1;
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (res) {
		ret = devm_request_irq(dev, res->start,
			lombo_wb_irq_handler, 0, "lombo_wb", wb);
		if (ret)
			dev_err(dev, "request interrupt failed.\n");
		else
			DRM_DEBUG_KMS("wb irq=%llu\n", (uint64_t)res->start);
		wb->irq = res->start;
	}

	/* init clock */
	wb->mod_clk = lombo_dpu_get_clk(dev, wb->dpu_top, DPU_MOD_WB);
	if (!wb->mod_clk) {
		DRM_ERROR("lombo_dpu_get_clk for wb fail\n");
		return -EINVAL;
	}
	if (!of_property_read_u32(np, "sclk", &value))
		lombo_dpu_clk_set_parent(wb->mod_clk, value);
	else
		lombo_dpu_clk_set_parent(wb->mod_clk, 0);
	if (!of_property_read_u32(np, "clk_rate", &value))
		lombo_dpu_clk_set_rate(wb->mod_clk, value);
	else
		DRM_ERROR("failed to of get wb clk_rate\n");

	for (i = 0; i < LOMBO_WB_FB_QUEUE_NUM; i++)
		INIT_LIST_HEAD(&wb->fb_queue[i]);
	spin_lock_init(&(wb->wlock));
	spin_lock_init(&(wb->qlock));
	init_waitqueue_head(&(wb->fb_wait));
	init_waitqueue_head(&(wb->work_wait));
	wb->workq = create_singlethread_workqueue("wb");
	if (!wb->workq) {
		dev_err(dev, "failed to create workqueue\n");
		return -EINVAL;
	}
	INIT_WORK(&wb->work, lombo_wb_worker);

	wb->subdrv.dev = dev;
	wb->subdrv.drm = master_data;
	wb->subdrv.open = lombo_wb_subdrv_open;
	wb->subdrv.close = lombo_wb_subdrv_close;

	dev_set_drvdata(dev, wb);

	ret = lombo_subdrv_register(&wb->subdrv);
	if (ret < 0)
		DRM_ERROR("failed to subdrv register lombo wb.\n");

	return ret;
}

static void lombo_wb_unbind(struct device *dev,
	struct device *master, void *master_data)
{
	struct lombo_wb *wb = dev_get_drvdata(dev);

	DRM_DEBUG_DRIVER("%s\n", __func__);

	cancel_work_sync(&wb->work);
	destroy_workqueue(wb->workq);

	lombo_dpu_put_clk(&wb->mod_clk);
	dev_set_drvdata(dev, NULL);
	kfree(wb);
}

static const struct component_ops lombo_wb_ops = {
	.bind	= lombo_wb_bind,
	.unbind	= lombo_wb_unbind,
};

static int lombo_wb_probe(struct platform_device *pdev)
{
	return component_add(&pdev->dev, &lombo_wb_ops);
}

static int lombo_wb_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &lombo_wb_ops);
	return 0;
}

static const struct of_device_id lombo_wb_of_table[] = {
	{ .compatible = "lombo,lombo-n8v0-wb" },
	{ .compatible = "lombo,lombo-n9v0-wb" },
	{ }
};
MODULE_DEVICE_TABLE(of, lombo_wb_of_table);

static struct platform_driver lombo_wb_platform_driver = {
	.probe		= lombo_wb_probe,
	.remove		= lombo_wb_remove,
	.driver		= {
		.owner = THIS_MODULE,
		.name		= "lombo-wb",
		.of_match_table	= lombo_wb_of_table,
	},
};
module_platform_driver(lombo_wb_platform_driver);

MODULE_AUTHOR("lomboswer <lomboswer@lombotech.com>");
MODULE_DESCRIPTION("Lombo SoC DPU WB Driver");
MODULE_LICENSE("GPL");
