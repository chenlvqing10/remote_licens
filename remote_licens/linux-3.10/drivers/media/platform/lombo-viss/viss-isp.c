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

#include <linux/of_i2c.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <media/v4l2-of.h>

#include "media-dev.h"
#include "viss-isp.h"
#include "viss-m2m.h"

#define MEM_ALIGN 32

struct viss_is *glb_is[VISS_IS_MAX_NUM];

static int viss_post_irq(struct viss_is *is,	int isp_status)
{
	struct video_device *vdev = is->isp.subdev.devnode;
	struct v4l2_event event;
	struct viss_isp_stat_event_status *status = (void *)event.u.data;

	PRT_TRACE_BEGIN("\n");

	if (NULL == vdev) {
		PRT_ERR("\n");
		return -EFAULT;
	}

	memset(&event, 0, sizeof(struct v4l2_event));
	status->isp_status = isp_status;

	event.type = V4L2_EVENT_ISP_IRQ;
	v4l2_event_queue(vdev, &event);

	PRT_TRACE_END("%d\n");

	return 0;
}

void isp_offline_process(void)
{
	struct viss_is *is;
	struct offline_buf *buf;

	if (!isp_offline)
		return;

	if (isp_offline_busy)
		return;
	else
		isp_offline_busy = 1;

	buf = list_entry(offline_fifo.next, struct offline_buf, list);
	is = glb_is[buf->isp_id];
	isp_offline_id = buf->isp_id;
	isp_offline_ts = buf->ts;

	if (!test_bit(IS_ST_STREAM, &is->state)) {
		cancel_delayed_work(&is->watchdog);
		/* streaming finished irq */
		viss_post_irq(is, 0x1);
		offline_fifo_pop();
		return;
	}

	/* ISP reset start */
	csp_viss_top_isp_clk_enable(0);
	csp_viss_top_isp_clk_enable(1);
	csp_isp_restore_regs(is->regs, is->viss->reg_save);

	csp_isp_set_tnr_input_add(is->regs, is->isp.tnr_mem_y.paddr,
				is->isp.tnr_mem_cb.paddr,
				is->isp.tnr_mem_cr.paddr);

	csp_isp_set_tnr_output_add(is->regs, is->isp.tnr_mem_y.paddr,
				is->isp.tnr_mem_cb.paddr,
				is->isp.tnr_mem_cr.paddr);

	csp_isp_set_bgm_stat_add(is->regs, is->isp.stat[BGM_STAT_BUF].paddr);
	csp_isp_set_anti_shake_address(is->regs,
		is->isp.stat[ISM_STAT_BUF].paddr);

	csp_isp_dma_ctrl(is->regs, is->isp.output_format);
	csp_isp_enable(is->regs, 1);

	csp_isp_ippu_ctrl(is->regs);
	csp_isp_enable_ippu(is->regs, 0);

	csp_isp_en_data_loss(is->regs, 1);

	if (is->viss->vv[0].streaming) {
		csp_isp_set_dma_main_path_y_size(is->regs,
			is->viss->vv[0].ctx->d_frame.yuv_size[0],
			is->viss->vv[0].ctx->d_frame.yuv_size[1],
			is->viss->vv[0].ctx->d_frame.yuv_size[2]);
		csp_isp_dma_main_path_enable(is->regs, 1);
	}
	if (is->viss->vv[1].streaming) {
		csp_isp_set_dma_sub_path_y_size(is->regs,
			is->viss->vv[1].ctx->d_frame.yuv_size[0],
			is->viss->vv[1].ctx->d_frame.yuv_size[1],
			is->viss->vv[1].ctx->d_frame.yuv_size[2]);
		csp_isp_dma_sub_path_enable(is->regs, 1);
	}
	/* ISP reset end */

	csp_isp_set_dma_y_pic_width(is->regs, buf->win.width);
	csp_isp_set_dma_y_pic_len(is->regs, buf->win.width);
	csp_isp_set_dma_y_pic_size(is->regs, buf->win.width*buf->win.height);

	csp_isp_set_dma_y_pic_start_add(is->regs, buf->dma.paddr);
	csp_isp_data_path_ctrl(is->regs);
	csp_isp_acq_cfg(is->regs, 0x8005);
	csp_isp_dma_start_ctrl(is->regs, 0x9a | (buf->raw_depth << 12));
	csp_isp_dma_config_update(is->regs);

	/* isp offline start */
	csp_isp_dma_start(is->regs);
}

static int viss_isp_write_dma_add(struct viss_is *is)
{
	int frame_size = is->isp.inp_frame.width * is->isp.inp_frame.height;
	void *base;

	if (isp_offline)
		base = is->viss->reg_save;
	else
		base = is->regs;

	if (!csp_isp_ippu_is_enabled(base))
		goto reg_set;

	if (is->isp.tnr_mem_y.size != frame_size) {
		if (is->isp.tnr_mem_y.size != 0)
			dma_free_coherent(&is->pdev->dev,
				is->isp.tnr_mem_y.size,
				is->isp.tnr_mem_y.vaddr,
				is->isp.tnr_mem_y.paddr);
		is->isp.tnr_mem_y.size = frame_size;
		is->isp.tnr_mem_y.vaddr = dma_alloc_coherent(&is->pdev->dev,
			is->isp.tnr_mem_y.size,
			&is->isp.tnr_mem_y.paddr, GFP_KERNEL);
		if (!is->isp.tnr_mem_y.vaddr) {
			PRT_ERR("memory alloc failed, size %d\n",
				is->isp.tnr_mem_y.size);
			return -ENOMEM;
		}
		memset(is->isp.tnr_mem_y.vaddr, 0, is->isp.tnr_mem_y.size);
	}

	if (is->isp.tnr_mem_cb.size != frame_size) {
		if (is->isp.tnr_mem_cb.size != 0)
			dma_free_coherent(&is->pdev->dev,
				is->isp.tnr_mem_cb.size,
				is->isp.tnr_mem_cb.vaddr,
				is->isp.tnr_mem_cb.paddr);
		is->isp.tnr_mem_cb.size = frame_size;
		is->isp.tnr_mem_cb.vaddr = dma_alloc_coherent(&is->pdev->dev,
			is->isp.tnr_mem_cb.size,
			&is->isp.tnr_mem_cb.paddr, GFP_KERNEL);
		if (!is->isp.tnr_mem_cb.vaddr) {
			PRT_ERR("memory alloc failed, size %d\n",
				is->isp.tnr_mem_cb.size);
			return -ENOMEM;
		}
		memset(is->isp.tnr_mem_cb.vaddr, 0, is->isp.tnr_mem_cb.size);
	}
reg_set:
	csp_isp_set_tnr_input_add(base, is->isp.tnr_mem_y.paddr,
				is->isp.tnr_mem_cb.paddr,
				is->isp.tnr_mem_cr.paddr);

	csp_isp_set_tnr_output_add(base, is->isp.tnr_mem_y.paddr,
				is->isp.tnr_mem_cb.paddr,
				is->isp.tnr_mem_cr.paddr);

	csp_isp_set_bgm_stat_add(base, is->isp.stat[BGM_STAT_BUF].paddr);
	csp_isp_set_anti_shake_address(base,
		is->isp.stat[ISM_STAT_BUF].paddr);

	csp_isp_dma_ctrl(base, is->isp.output_format);
	csp_isp_enable(base, 1);

	csp_isp_ippu_ctrl(base);

	schedule_delayed_work(&is->watchdog, msecs_to_jiffies(1000));

	return 0;
}

static void *viss_buf_map(dma_addr_t phys_addr, size_t size)
{
	int i, npages = PAGE_ALIGN(size) / PAGE_SIZE;
	struct page **pages = NULL;
	struct page *page, **tmp = pages;
	void *vaddr;

	if (npages == 0)
		return NULL;

	/* make sure addr is page aligned */
	BUG_ON((unsigned long)phys_addr & (PAGE_SIZE - 1));

	page = phys_to_page((unsigned long)phys_addr);
	if (!PageHighMem(page))
		return phys_to_virt((unsigned long)phys_addr);

	pages = vmalloc(sizeof(struct page *) * npages);
	if (!pages) {
		pr_err("vmalloc failed\n");
		return NULL;
	}
	tmp = pages;

	for (i = 0; i < npages; i++)
		*(tmp++) = page++;

	vaddr = vmap(pages, npages, VM_MAP, PAGE_KERNEL);/* cached map */
	/* vaddr = vmap(pages, npages, VM_MAP,
		pgprot_writecombine(PAGE_KERNEL)); */	/*uncached map */
	vfree(pages);

	if (vaddr == NULL) {
		pr_err("vmap failed\n");
		return ERR_PTR(-ENOMEM);
	}

	return vaddr;
}

static void viss_buf_unmap(dma_addr_t phys_addr, void *virt_addr)
{
	struct page *page;

	page = phys_to_page((unsigned long)phys_addr);
	if (!PageHighMem(page))
		return;

	vunmap(virt_addr);
}

int viss_post_event(struct viss_is *is,
			struct v4l2_event *event, int timeout)
{
	int rc = 0;
	struct video_device *vdev = is->isp.subdev.devnode;

	PRT_TRACE_BEGIN("\n");

	if (NULL == vdev) {
		PRT_ERR("\n");
		return -EFAULT;
	}

	/*re-init wait_complete before event post */
	if (timeout > 0)
		INIT_COMPLETION(is->cmd_ack.wait_complete);

	v4l2_event_queue(vdev, event);

	if (timeout <= 0)
		return 0;

	/* should wait on session based condition */
	rc = wait_for_completion_timeout(&is->cmd_ack.wait_complete,
			msecs_to_jiffies(timeout));
	if (rc == 0) {
		char *type_name = "event";
		if (event->type == V4L2_EVENT_ISP_STREAMON)
			type_name = "streamon";
		else if (event->type == V4L2_EVENT_ISP_STREAMOFF)
			type_name = "streamoff";
		else if (event->type == V4L2_EVENT_ISP_SET_FMT)
			type_name = "set_fmt";
		PRT_ERR("isp%d wait %s ack timeout!\n", is->isp.id, type_name);
		return -ETIMEDOUT;
	}

	PRT_TRACE_END("rc: %d\n", rc);

	return 0;
}

static const struct viss_fmt viss_isp_formats[VISS_ISP_NUM_FORMATS] = {
	{
		.name		= "RAW8 (GRBG)",
		.fourcc		= V4L2_PIX_FMT_SGRBG8,
		.depth		= { 8 },
		.color		= VISS_FMT_RAW8,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_SGRBG8_1X8,
	}, {
		.name		= "RAW10 (GRBG)",
		.fourcc		= V4L2_PIX_FMT_SGRBG10,
		.depth		= { 10 },
		.color		= VISS_FMT_RAW10,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_SGRBG10_1X10,
	}, {
		.name		= "RAW12 (GRBG)",
		.fourcc		= V4L2_PIX_FMT_SGRBG12,
		.depth		= { 12 },
		.color		= VISS_FMT_RAW12,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_SGRBG12_1X12,
	}, {
		.name		= "YUV 4:2:2 planar, Y/CbCr",
		.fourcc		= V4L2_PIX_FMT_NV16,
		.depth		= { 8, 8 },
		.color		= VISS_FMT_YCBYCR422,
		.memplanes	= 2,
		.colplanes	= 2,
		.mbus_code	= V4L2_MBUS_FMT_YVYU8_2X8,
	}, {
		.name		= "YUV 4:2:2 planar, Y/CrCb",
		.fourcc		= V4L2_PIX_FMT_NV61,
		.depth		= { 8, 8 },
		.color		= VISS_FMT_YCRYCB422,
		.memplanes	= 2,
		.colplanes	= 2,
		.mbus_code	= V4L2_MBUS_FMT_YUYV8_2X8,
	},
};

/**
 * viss_isp_find_format - lookup color format by fourcc or media bus code
 * @pixelformat: fourcc to match, ignored if null
 * @mbus_code: media bus code to match, ignored if null
 * @index: index to the viss_isp_formats array, ignored if negative
 */
const struct viss_fmt *viss_isp_find_format(const u32 *pixelformat,
					const u32 *mbus_code, int index)
{
	const struct viss_fmt *fmt, *def_fmt = NULL;
	unsigned int i;
	int id = 0;

	if (index >= (int)ARRAY_SIZE(viss_isp_formats))
		return NULL;

	for (i = 0; i < ARRAY_SIZE(viss_isp_formats); ++i) {
		fmt = &viss_isp_formats[i];
		if (pixelformat != NULL)
			PRT_DBG("i: %d %u------%u.\n",
				i, *pixelformat, fmt->fourcc);
		else
			PRT_DBG("i: %d pixelformat is NULL.\n", i);
		if (pixelformat && fmt->fourcc == *pixelformat)
			return fmt;
		if (mbus_code && fmt->mbus_code == *mbus_code)
			return fmt;
		if (index == id)
			def_fmt = fmt;
		id++;
	}
	return def_fmt;
}

/* Capture subdev media entity operations */
static int viss_is_link_setup(struct media_entity *entity,
				const struct media_pad *local,
				const struct media_pad *remote, u32 flags)
{
	return 0;
}

static const struct media_entity_operations viss_is_subdev_media_ops = {
	.link_setup = viss_is_link_setup,
};

static int viss_is_subdev_enum_mbus_code(struct v4l2_subdev *sd,
				struct v4l2_subdev_fh *fh,
				struct v4l2_subdev_mbus_code_enum *code)
{
	const struct viss_fmt *fmt;

	fmt = viss_isp_find_format(NULL, NULL, code->index);
	if (!fmt)
		return -EINVAL;
	code->code = fmt->mbus_code;
	return 0;
}

static int viss_isp_subdev_get_fmt(struct v4l2_subdev *sd,
				   struct v4l2_subdev_fh *fh,
				   struct v4l2_subdev_format *fmt)
{
	struct viss_isp *isp = v4l2_get_subdevdata(sd);
	struct v4l2_mbus_framefmt *mf = &fmt->format;
	PRT_TRACE_BEGIN("\n");

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		mf = v4l2_subdev_get_try_format(fh, fmt->pad);
		fmt->format = *mf;
		return 0;
	}

	mf->colorspace = V4L2_COLORSPACE_SRGB;

	mutex_lock(&isp->subdev_lock);

	if (fmt->pad == VISS_ISP_SD_PAD_SINK) {
		/* full camera input frame size */
		mf->width = isp->inp_frame.width;
		mf->height = isp->inp_frame.height;
		mf->code = isp->subdev_fmt.code;
	} else {
		/* crop size */
		mf->width = isp->out_frame[0].width;
		mf->height = isp->out_frame[0].height;
		mf->code = isp->subdev_fmt.code;
	}

	mutex_unlock(&isp->subdev_lock);

	PRT_TRACE_END("pad%d: fmt: 0x%x, %dx%d\n", fmt->pad,
		mf->code, mf->width, mf->height);

	return 0;
}

static void __isp_subdev_try_format(struct viss_isp *isp,
				   struct v4l2_subdev_format *fmt)
{
	struct v4l2_mbus_framefmt *mf = &fmt->format;

	v4l_bound_align_image(&mf->width, VISS_ISP_SINK_WIDTH_MIN,
			VISS_ISP_SINK_WIDTH_MAX, 0,
			&mf->height, VISS_ISP_SINK_HEIGHT_MIN,
			VISS_ISP_SINK_HEIGHT_MAX, 0, 0);
	isp->subdev_fmt = *mf;
}

static int viss_isp_subdev_set_fmt(struct v4l2_subdev *sd,
				   struct v4l2_subdev_fh *fh,
				   struct v4l2_subdev_format *fmt)
{
	struct viss_isp *isp = v4l2_get_subdevdata(sd);
	struct viss_is *is = viss_isp_to_is(isp);
	struct v4l2_mbus_framefmt *mf = &fmt->format;
	struct v4l2_event event;
	struct viss_v4l2_event_data *event_data =
		(struct viss_v4l2_event_data *)&event.u.data[0];
	int ret = 0;

	PRT_TRACE_BEGIN("pad%d: code: 0x%x, %dx%d\n",
		 fmt->pad, mf->code, mf->width, mf->height);

	mf->colorspace = V4L2_COLORSPACE_SRGB;

	mutex_lock(&isp->subdev_lock);
	__isp_subdev_try_format(isp, fmt);

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		mf = v4l2_subdev_get_try_format(fh, fmt->pad);
		*mf = fmt->format;
		mutex_unlock(&isp->subdev_lock);
		return 0;
	}

	if (fmt->pad == VISS_ISP_SD_PAD_SINK) {
		isp->inp_frame.width = mf->width;
		isp->inp_frame.height = mf->height;
	} else {
		u32 out_fmt;

		if (fmt->pad == VISS_ISP_SD_PAD_SRC_P0) {
			isp->out_frame[0].width = mf->width;
			isp->out_frame[0].height = mf->height;
			isp->out_frame[1].width = VISS_ISP_SUBPATH_WIDTH_MAX;
			isp->out_frame[1].height = VISS_ISP_SUBPATH_HEIGHT_MAX;
			out_fmt = is->viss->vv[0].ctx->d_frame.fmt->fourcc;
		} else {
			isp->out_frame[0].width = isp->inp_frame.width;
			isp->out_frame[0].height = isp->inp_frame.height;
			isp->out_frame[1].width = mf->width;
			isp->out_frame[1].height = mf->height;
			out_fmt = is->viss->vv[1].ctx->d_frame.fmt->fourcc;
		}

		switch (out_fmt) {
		case V4L2_PIX_FMT_YUV420:
		case V4L2_PIX_FMT_YVU420:
		case V4L2_PIX_FMT_YUV420M:
		case V4L2_PIX_FMT_YVU420M:
		case V4L2_PIX_FMT_YUV422P:
			isp->output_format = PLANAR;
			break;
		case V4L2_PIX_FMT_NV12:
		case V4L2_PIX_FMT_NV12M:
		case V4L2_PIX_FMT_NV16:
		case V4L2_PIX_FMT_NV16M:
			isp->output_format = SEMIPLANAR;
			break;
		default:
			isp->output_format = SEMIPLANAR;
			break;
		}

		isp_offline = isp_is_offline(
			is->viss->pipeline.subdevs[IDX_SENSOR]);
		if (isp_offline) {
			if (isp_offline_set == 0) {
				csp_viss_top_isp_clk_enable(0);
				csp_viss_top_isp_clk_enable(1);
				csp_isp_dump_regs(is->regs,
					glb_viss[0]->reg_save);
				if (glb_viss[1]) {
					csp_isp_dump_regs(is->regs,
						glb_viss[1]->reg_save);
				} else {
					glb_viss[1] = glb_viss[0];
				}
			}
			isp_offline_set++;
		} else {
			csp_viss_top_isp_clk_enable(0);
			csp_viss_top_isp_clk_enable(1);
		}

		memset(&event, 0, sizeof(event));
		event.type = V4L2_EVENT_ISP_SET_FMT;

		event_data->main_path_width = isp->out_frame[0].width;
		event_data->main_path_height = isp->out_frame[0].height;
		event_data->sub_path_width = isp->out_frame[1].width;
		event_data->sub_path_height = isp->out_frame[1].height;
		event_data->nop6 = out_fmt;

		ret = viss_post_event(is, &event, 6000);
		PRT_DBG("viss_isp_subdev_set_fmt ret : %d.\n", ret);
	}

	ret = 0;
	mutex_unlock(&isp->subdev_lock);
	PRT_TRACE_END("\n");
	return ret;
}

static int viss_isp_subdev_s_stream(struct v4l2_subdev *sd, int on)
{
	struct viss_isp *isp = v4l2_get_subdevdata(sd);
	struct viss_is *is = viss_isp_to_is(isp);
	struct v4l2_event event;
	int ret = 0;

	PRT_TRACE_BEGIN("on : %d.\n", on);

	mutex_lock(&isp->subdev_lock);

	if (on) {
		if (test_and_set_bit(IS_ST_STREAM, &is->state))
			return 0;

		if (test_bit(ST_PREV_SUSPENDED, &is->viss->vv[0].state) ||
		    test_bit(ST_PREV_SUSPENDED, &is->viss->vv[1].state)) {
			csp_viss_top_isp_clk_enable(0);
			csp_viss_top_isp_clk_enable(1);
			csp_isp_restore_regs(is->regs, is->reg_data);
		} else {
			memset(&event, 0, sizeof(event));
			event.type = V4L2_EVENT_ISP_STREAMON;
			ret = viss_post_event(is, &event, 6000);
		}
		PRT_DBG("ret : %d.\n", ret);

		ret = viss_isp_write_dma_add(is);
		isp_reg_set(is->regs, 0x5bc, 0x40);
		is->isp.vstart_cnt = 0;
	} else {
		void *base;

		cancel_delayed_work(&is->watchdog);
		clear_bit(IS_ST_STREAM, &is->state);

		if (test_bit(ST_PREV_SUSPENDED, &is->viss->vv[0].state) ||
		    test_bit(ST_PREV_SUSPENDED, &is->viss->vv[1].state)) {
			csp_isp_dump_regs(is->regs, is->reg_data);
		} else {
			memset(&event, 0, sizeof(event));
			event.type = V4L2_EVENT_ISP_STREAMOFF;
#if 0
			if (isp_offline &&
			   (isp_offline_id < 0 || isp_offline_id != isp->id)) {
				viss_post_event(is, &event, 0);
				/* streaming finished irq */
				viss_post_irq(is, 0x1);
			} else {
				ret = viss_post_event(is, &event, 2000);
			}
#else
			if (isp_offline &&
			   (isp_offline_id < 0 || isp_offline_id != isp->id)) {
				schedule_delayed_work(&is->watchdog,
					msecs_to_jiffies(100));
			}
			ret = viss_post_event(is, &event, 6000);
#endif
		}

		if (isp_offline) {
			isp_offline_set--;
			offline_fifo_release(isp->id);
			base = is->viss->reg_save;
		} else {
			base = is->regs;
		}

		csp_isp_disable_isp_int(base);
		csp_isp_disable_dma_int(base);
		csp_isp_enable(base, 0);
		csp_isp_dma_main_path_enable(base, 0);
		csp_isp_dma_sub_path_enable(base, 0);
		csp_isp_enable_ippu(base, 0);
	}

	mutex_unlock(&isp->subdev_lock);

	PRT_TRACE_END("\n");
	return ret;
}

static int viss_isp_subdev_s_power(struct v4l2_subdev *sd, int on)
{
	int ret = 0;

	PRT_DBG("on: %d.\n", on);
	return ret;
}

static int viss_isp_read_reg(struct viss_is *is,
				struct viss_isp_reg_data *data)
{
	void *base;

	if (isp_offline)
		base = is->viss->reg_save;
	else
		base = is->regs;

	data->reg_value = isp_reg_read(base, data->reg_add & 0xffff);

	PRT_DBG("add: 0x%08x, val: 0x%08x.\n", data->reg_add, data->reg_value);

	return 0;
}

static int viss_isp_write_reg(struct viss_is *is,
				struct viss_isp_reg_data *data)
{
	void *base;

	if (isp_offline)
		base = is->viss->reg_save;
	else
		base = is->regs;

	PRT_DBG("add: 0x%08x, val: 0x%08x.\n", data->reg_add, data->reg_value);

	isp_reg_write(base, data->reg_add & 0xffff, data->reg_value);

	return 0;
}

static int viss_isp_read_seqreg(struct viss_is *is,
				struct viss_isp_reg_seqdata *data)
{
	int ret = 0;
	int i = 0;
	void *base;

	if (isp_offline)
		base = is->viss->reg_save;
	else
		base = is->regs;

	PRT_TRACE_BEGIN("\n");

	ret = copy_from_user(is->reg_add, data->reg_add,
			sizeof(unsigned int)*(data->reg_size));
	if (ret < 0) {
		PRT_ERR("copy_from_user error: is->reg_data.\n");
		return ret;
	}
	for (i = 0; i < data->reg_size; i++) {
		is->reg_data[i] = isp_reg_read(base,
			is->reg_add[i] & 0xffff);
		PRT_DBG("add: 0x%08x, val: 0x%08x.\n",
			is->reg_add[i], is->reg_data[i]);
	}

	ret = copy_to_user(data->reg_value, is->reg_data,
			sizeof(unsigned int)*(data->reg_size));
	if (ret)
		PRT_ERR("copy_to_user error: reg_value %d.\n", ret);
	PRT_TRACE_END("\n");

	return ret;
}

static int viss_isp_write_seqreg(struct viss_is *is,
				struct viss_isp_reg_seqdata *data)
{
	int ret = 0;
	int i = 0;
	void *base;

	if (isp_offline)
		base = is->viss->reg_save;
	else
		base = is->regs;

	PRT_TRACE_BEGIN("\n");

	ret = copy_from_user(is->reg_add, data->reg_add,
			sizeof(unsigned	int)*(data->reg_size));
	if (ret < 0) {
		PRT_ERR("copy_from_user error: is->reg_data.\n");
		return ret;
	}
	ret = copy_from_user(is->reg_data, data->reg_value,
			sizeof(unsigned	int)*(data->reg_size));
	if (ret < 0) {
		PRT_ERR("copy_from_user error: is->reg_add.\n");
		return ret;
	}
	PRT_DBG("0x%08x.\n", data->reg_size);

	for (i = 0; i < data->reg_size; i++) {
		if (is->reg_add[i] & ISP_SET_CMD_FLAG) {
			PRT_DBG("viss_isp_write_cmd i: %d  0x%08x: 0x%08x.\n",
				i, is->reg_add[i], is->reg_data[i]);
			switch (is->reg_add[i]) {
			case ISP_SET_CMD_EXPOSURE_TIME:
				is->exif.exposure_time = is->reg_data[i];
				break;
			case ISP_SET_CMD_SHUTTER_SPEED:
				is->exif.shutter_speed = is->reg_data[i];
				break;
			case ISP_SET_CMD_APERTURE:
				is->exif.aperture = is->reg_data[i];
				break;
			case ISP_SET_CMD_BRIGHTNESS:
				is->exif.brightness = is->reg_data[i];
				break;
			case ISP_SET_CMD_EXPOSURE_BIAS:
				is->exif.exposure_bias = is->reg_data[i];
				break;
			case ISP_SET_CMD_ISO_SPEED:
				is->exif.iso_speed = is->reg_data[i];
				break;
			case ISP_SET_CMD_FLASH:
				is->exif.flash = is->reg_data[i];
				break;
			case ISP_SET_CMD_ILLU_ID:
				is->exif.illu_id = is->reg_data[i];
				break;
			case ISP_SET_CMD_BACK_SCORE:
				is->exif.back_score = is->reg_data[i];
				break;
			case ISP_SET_CMD_OBJ_LUM:
				is->exif.res[0] = is->reg_data[i];
				break;
			case ISP_SET_CMD_BACK_LUM:
				is->exif.res[1] = is->reg_data[i];
				break;
			case ISP_SET_CMD_AVG_LUM:
				is->exif.res[2] = is->reg_data[i];
				break;
			case ISP_SET_CMD_ORG_TARGET:
				is->exif.res[3] = is->reg_data[i];
				break;
			case ISP_SET_CMD_FINAL_TARGET:
				is->exif.res[4] = is->reg_data[i];
				break;
			case ISP_SET_CMD_AWB_STAT:
				memcpy(is->awb_stat, &is->reg_data[i],
					4 * (is->reg_data[i] * 3 + 1));
				i += is->reg_data[i] * 3 + 1;
				break;
			default:
				ret = -EINVAL;
			}
		} else {
			PRT_DBG("write_seqreg i: %d 0x%08x: 0x%08x.\n",
				i, is->reg_add[i], is->reg_data[i]);

			isp_reg_write(base, is->reg_add[i] & 0xffff,
				is->reg_data[i]);
		}
	}
	PRT_TRACE_END("\n");
	return ret;
}

static int viss_isp_get_stat(struct viss_is *is,
				struct viss_isp_stat_data *data)
{
	int ret = 0;

	PRT_TRACE_BEGIN("\n");

	if (data->buf == NULL) {
		PRT_ERR("stat_buf is NULL.\n");
		return -EINVAL;
	}

	ret = copy_to_user(data->buf, is->isp.stat[data->buf_type].vaddr,
				data->buf_size);
	if (ret)
		PRT_ERR("copy_to_user error %d.\n", ret);
	PRT_TRACE_END("\n");
	return ret;
}

static int viss_isp_get_frame(struct viss_is *is,
				struct viss_isp_frame_data *data)
{
	int ret = 0;
	int size = 0;

	PRT_TRACE_BEGIN("\n");

	size = is->isp.save_yuv[0].size + is->isp.save_yuv[1].size +
		is->isp.save_yuv[2].size;

	if (NULL == data->frame_buf) {
		PRT_ERR("frame_buf is NULL.\n");
		return -EINVAL;
	} else if (size < data->frame_size) {
		PRT_ERR("frame_size is large than buf size.\n");
		return -EINVAL;
	} else {
		struct viss_isp_memory *buf = &is->isp.save_yuv[0];

		buf[0].vaddr = viss_buf_map(buf[0].paddr, buf[0].size);
		if (is->isp.save_yuv[0].vaddr == NULL) {
			PRT_ERR("yuv addr is NULL, read image failed!\n");
			return -1;
		}
		ret = copy_to_user(data->frame_buf, is->isp.save_yuv[0].vaddr,
					data->frame_size);
		viss_buf_unmap(buf[0].paddr, buf[0].vaddr);
		if (ret)
			PRT_ERR("isp frame copy_to_user error %d.\n", ret);
	}

	PRT_TRACE_END("\n");
	return ret;
}

int viss_isp_restart(struct viss_is *is)
{
	int ret = 0;
#if 0
	struct v4l2_event event;

	memset(&event, 0, sizeof(event));
	event.type = V4L2_EVENT_ISP_RESTART;

	ret = viss_post_event(&is->isp, &is->cmd_ack, &event, 6*1000);
#else
	csp_isp_dump_regs(is->regs, is->reg_data);
	csp_viss_top_isp_clk_enable(0);
	csp_viss_top_isp_clk_enable(1);
	csp_isp_restore_regs(is->regs, is->reg_data);
#endif
	PRT_DBG("ret : %d.\n", ret);

	return ret;
}

int viss_isp_reparse(struct viss_isp *isp, int cmd)
{
	struct viss_is *is = viss_isp_to_is(isp);
	struct video_device *vdev = isp->subdev.devnode;
	struct v4l2_event event;
	struct viss_v4l2_event_data *event_data =
		(struct viss_v4l2_event_data *)&event.u.data[0];

	PRT_TRACE_BEGIN("cmd : %d.\n", cmd);

	if (is == NULL || vdev == NULL) {
		PRT_ERR("viss_is is NULL.\n");
		return -EINVAL;
	}

	memset(&event, 0, sizeof(event));
	event.type = V4L2_EVENT_ISP_REPARSE;
	event_data->nop6 = cmd;
	v4l2_event_queue(vdev, &event);
	if (cmd == 0)
		is->isp_reparse = 1;

	PRT_TRACE_END("\n");
	return 0;
}

int viss_isp_usb_read(void *cmd, void *data)
{
	struct isp_usb_cmd *c = (struct isp_usb_cmd *)cmd;
	struct viss_is *is = glb_is[c->res[0]];
	struct video_device *vdev = is->isp.subdev.devnode;
	struct v4l2_event event;
	struct v4l2_subdev *sd = is->viss->pipeline.subdevs[IDX_SENSOR];

	if (c->res[1] == ISP_USB_CMD_GET_IMAGE) {
		struct viss_isp_memory *buf = &is->isp.save_yuv[0];
		int w, h, f, diff;

		if (is->viss->vv[0].streaming) {
			w = is->viss->vv[0].ctx->d_frame.width;
			h = is->viss->vv[0].ctx->d_frame.height;
			f = is->viss->vv[0].ctx->d_frame.fmt->fourcc;
		} else {
			w = is->viss->vv[1].ctx->d_frame.width;
			h = is->viss->vv[1].ctx->d_frame.height;
			f = is->viss->vv[1].ctx->d_frame.fmt->fourcc;
		}

		buf[0].vaddr = viss_buf_map(buf[0].paddr, buf[0].size);
		if (is->isp.save_yuv[0].vaddr == NULL) {
			PRT_ERR("yuv addr is NULL!\n");
			return -1;
		}

		if (f == V4L2_PIX_FMT_NV12M) {
			buf[1].vaddr = viss_buf_map(buf[1].paddr, buf[1].size);
			if (is->isp.save_yuv[1].vaddr == NULL) {
				PRT_ERR("yuv addr is NULL!\n");
				return -1;
			}

			diff = w * h - c->addr;
			if (diff >= (int)c->len) {
				memcpy(data, (void *)(buf[0].vaddr + c->addr),
					c->len);
			} else if (diff > 0) {
				memcpy(data, (void *)(buf[0].vaddr + c->addr),
					diff);
				memcpy(data+diff, (void *)(buf[1].vaddr),
					c->len - diff);
			} else {
				memcpy(data, (void *)(buf[1].vaddr - diff),
					c->len);
			}

			viss_buf_unmap(buf[1].paddr, buf[1].vaddr);
		} else if (f == V4L2_PIX_FMT_NV12) {
			memcpy(data, buf[0].vaddr + c->addr, c->len);
		} else {
			PRT_ERR("isp tool cannot support fmt 0x%x now!\n", f);
		}
		viss_buf_unmap(buf[0].paddr, buf[0].vaddr);
		if (w * h > PAGE_ALIGN(c->addr + c->len))
			return 0;
	} else if (c->res[1] == ISP_USB_CMD_GET_JSON) {
		struct file *fp;
		mm_segment_t fs;
		loff_t pos = 0;

		fp = filp_open(is->json, O_RDWR, 0644);
		if (IS_ERR(fp)) {
			PRT_ERR("open %s error\n", is->json);
			return -1;
		}
		fs = get_fs();
		set_fs(KERNEL_DS);
		/*min(fp->f_inode->i_size, c->len);*/
		vfs_read(fp, data, c->len, &pos);
		filp_close(fp, NULL);
		set_fs(fs);
	} else if (c->res[1] == ISP_USB_CMD_GET_SENSOR_INFO) {
		struct viss_isp_sensor_info *info = data;
		struct viss_isp_sensor_fmt_info sensor_info;
		struct media_entity *me = NULL;
		struct kstat st;
		int ret = 0;

		v4l2_subdev_call(sd, core, ioctl,
			VIDIOC_VISS_ISP_SENSOR_FMT_INFO, &sensor_info);
		me = viss_pipeline_get_head(&is->isp.subdev.entity);
		sprintf(is->json, "/etc/isp/%s_%d_%d_%d.json", me->name,
			sensor_info.width, sensor_info.height,
			sensor_info.pclk/sensor_info.hts/sensor_info.vts);
		ret = vfs_stat(is->json, &st);
		if (ret < 0) {
			sprintf(is->json, "/etc/isp/%s.json", me->name);
			ret = vfs_stat(is->json, &st);
			if (ret < 0) {
				PRT_ERR("there is no match json!!!\n");
				return -1;
			}
		}

		memcpy(info->sensor_name, me->name, 32);
		if (is->viss->vv[0].streaming) {
			info->width = is->viss->vv[0].ctx->d_frame.width;
			info->height = is->viss->vv[0].ctx->d_frame.height;
		} else {
			info->width = is->viss->vv[1].ctx->d_frame.width;
			info->height = is->viss->vv[1].ctx->d_frame.height;
		}
		info->json_len = st.size;
		info->res[0] = is->exif.exposure_time;
		info->res[1] = is->exif.iso_speed;
		info->res[2] = is->exif.brightness;
		info->res[3] = is->exif.back_score;
		info->res[4] = is->exif.res[0];
		info->res[5] = is->exif.res[1];
		info->res[6] = is->exif.res[2];
		info->res[7] = is->exif.res[3];
		info->res[8] = is->exif.res[4];

		info->res[9] = is->exif.illu_id;
		info->res[10] = is->awb_stat[0];
		memcpy(&info->res[11], &is->awb_stat[1], 12*is->awb_stat[0]);
	} else if (c->res[1] == ISP_USB_CMD_GET_SENSOR_REG) {
		struct viss_isp_reg_data reg;
		__u32 *reg_value = data;

		reg.reg_add = c->addr;
		v4l2_subdev_call(sd, core, ioctl, VIDIOC_VISS_GET_SENSOR, &reg);
		*reg_value = reg.reg_value;
	}

	memset(&event, 0, sizeof(event));
	event.type = V4L2_EVENT_ISP_USB_RD;
	event.u.data[0] = c->res[1];

	v4l2_event_queue(vdev, &event);

	return 0;
}
EXPORT_SYMBOL(viss_isp_usb_read);

int viss_isp_usb_write(void *cmd, void *data)
{
	struct isp_usb_cmd *c = (struct isp_usb_cmd *)cmd;
	struct viss_is *is = glb_is[c->res[0]];
	struct video_device *vdev = is->isp.subdev.devnode;
	struct v4l2_event event;

	if (c->res[1] == ISP_USB_CMD_SET_JSON ||
	    c->res[1] == ISP_USB_CMD_SET_JSON_RELOAD) {
		struct file *fp;
		mm_segment_t fs;
		loff_t pos = 0;

		fp = filp_open(is->json, O_RDWR | O_CREAT | O_TRUNC, 0644);
		if (IS_ERR(fp)) {
			PRT_ERR("open %s error\n", is->json);
			return -1;
		}
		fs = get_fs();
		set_fs(KERNEL_DS);
		vfs_write(fp, data, c->len, &pos);
		filp_close(fp, NULL);
		set_fs(fs);
	} else if (c->res[1] == ISP_USB_CMD_SET_SENSOR_REG) {
		struct v4l2_subdev *sd = is->viss->pipeline.subdevs[IDX_SENSOR];
		struct viss_isp_reg_data reg;
		__u32 *reg_value = data;

		reg.reg_add = c->addr;
		reg.reg_value = *reg_value;
		v4l2_subdev_call(sd, core, ioctl, VIDIOC_VISS_SET_SENSOR, &reg);
	}

	if (c->res[1] == ISP_USB_CMD_SET_RELOAD ||
	    c->res[1] == ISP_USB_CMD_SET_JSON_RELOAD) {
		viss_isp_reparse(&is->isp, 0);
	}

	memset(&event, 0, sizeof(event));
	event.type = V4L2_EVENT_ISP_USB_WR;
	event.u.data[0] = c->res[1];

	v4l2_event_queue(vdev, &event);

	return 0;
}
EXPORT_SYMBOL(viss_isp_usb_write);

static void viss_isp_get_exif(struct viss_is *is, struct viss_isp_exif *exif)
{
	*exif = is->exif;
}

static long viss_isp_ioctl(struct v4l2_subdev *sd, unsigned int cmd,
					void *arg)
{
	struct viss_isp *isp = v4l2_get_subdevdata(sd);
	struct viss_is *is = viss_isp_to_is(isp);

	switch (cmd) {
	case VIDIOC_VISS_ISP_STREAMON_ACK:
		PRT_DBG("VIDIOC_VISS_ISP_STREAMON_ACK\n");
		complete(&is->cmd_ack.wait_complete);
		break;
	case VIDIOC_VISS_ISP_STREAMOFF_ACK:
		PRT_DBG("VIDIOC_VISS_ISP_STREAMOFF_ACK\n");
		complete(&is->cmd_ack.wait_complete);
		break;
	case VIDIOC_VISS_ISP_OPEN_ACK:
		PRT_DBG("VIDIOC_VISS_ISP_OPEN_ACK\n");
		complete(&is->cmd_ack.wait_complete);
		break;
	case VIDIOC_VISS_ISP_CLOSE_ACK:
		PRT_DBG("VIDIOC_VISS_ISP_CLOSE_ACK\n");
		complete(&is->cmd_ack.wait_complete);
		break;
	case VIDIOC_VISS_ISP_IRQ_ACK:
		PRT_DBG("VIDIOC_VISS_ISP_IRQ_ACK\n");
		break;
	case VIDIOC_VISS_ISP_SET_FMT_ACK:
		PRT_DBG("VIDIOC_VISS_ISP_SET_FMT_ACK\n");
		complete(&is->cmd_ack.wait_complete);
		break;
	case VIDIOC_VISS_ISP_READ_REG:
		PRT_DBG("VIDIOC_VISS_ISP_READ_REG\n");
		viss_isp_read_reg(is, arg);
		break;
	case VIDIOC_VISS_ISP_WRITE_REG:
		PRT_DBG("VIDIOC_VISS_ISP_WRITE_REG\n");
		viss_isp_write_reg(is, arg);
		break;
	case VIDIOC_VISS_ISP_READ_SEQREG:
		PRT_DBG("VIDIOC_VISS_ISP_READ_SEQREG\n");
		viss_isp_read_seqreg(is, arg);
		break;
	case VIDIOC_VISS_ISP_WRITE_SEQREG:
		PRT_DBG("VIDIOC_VISS_ISP_WRITE_SEQREG\n");
		viss_isp_write_seqreg(is, arg);
		break;
	case VIDIOC_VISS_ISP_STAT_REQ:
		PRT_DBG("VIDIOC_VISS_ISP_STAT_REQ\n");
		viss_isp_get_stat(is, arg);
		break;
	case VIDIOC_VISS_ISP_RESET:
		PRT_DBG("VIDIOC_VISS_ISP_RESET\n");
		csp_viss_top_isp_clk_enable(0);
		csp_viss_top_isp_clk_enable(1);
		break;
	case VIDIOC_VISS_ISP_RESTART_ACK:
		PRT_DBG("VIDIOC_VISS_ISP_RESTART_ACK\n");
		complete(&is->cmd_ack.wait_complete);
		break;
	case VIDIOC_VISS_ISP_EXIF_REQ:
		viss_isp_get_exif(is,
			(struct viss_isp_exif *)arg);
		break;
	case VIDIOC_VISS_ISP_REPARSE_ACK:
		PRT_DBG("VIDIOC_VISS_ISP_REPARSE_ACK\n");
		is->isp_reparse = 0;
		break;
	case VIDIOC_VISS_ISP_GET_FRAME:
		PRT_DBG("VIDIOC_VISS_ISP_GET_FRAME\n");
		viss_isp_get_frame(is, arg);
		break;
	default:
		return -ENOIOCTLCMD;
	}

	return 0;
}

static int viss_isp_subscribe_event(struct v4l2_subdev *subdev,
	struct v4l2_fh *fh, struct v4l2_event_subscription *sub)
{
	PRT_DBG("\n");
	if (sub->type == V4L2_EVENT_CTRL)
		return v4l2_ctrl_subscribe_event(fh, sub);
	else
		return v4l2_event_subscribe(fh, sub, 8, NULL);
}

static int viss_isp_unsubscribe_event(struct v4l2_subdev *subdev,
				    struct v4l2_fh *fh,
				    struct v4l2_event_subscription *sub)
{
	return v4l2_event_unsubscribe(fh, sub);
}

static int viss_isp_subdev_open(struct v4l2_subdev *sd,
				struct v4l2_subdev_fh *fh)
{
	return 0;
}

static int viss_isp_subdev_registered(struct v4l2_subdev *sd)
{
	return 0;
}

static void viss_isp_subdev_unregistered(struct v4l2_subdev *sd)
{

}

static const struct v4l2_subdev_internal_ops viss_is_subdev_internal_ops = {
	.registered = viss_isp_subdev_registered,
	.unregistered = viss_isp_subdev_unregistered,
	.open = viss_isp_subdev_open,
};

static const struct v4l2_subdev_pad_ops viss_is_subdev_pad_ops = {
	.enum_mbus_code = viss_is_subdev_enum_mbus_code,
	.get_fmt = viss_isp_subdev_get_fmt,
	.set_fmt = viss_isp_subdev_set_fmt,
};

static const struct v4l2_subdev_video_ops viss_is_subdev_video_ops = {
	.s_stream = viss_isp_subdev_s_stream,
};

static const struct v4l2_subdev_core_ops viss_is_core_ops = {
	.s_power = viss_isp_subdev_s_power,
	.ioctl = viss_isp_ioctl,
	.subscribe_event = viss_isp_subscribe_event,
	.unsubscribe_event = viss_isp_unsubscribe_event,
};

static struct v4l2_subdev_ops viss_is_subdev_ops = {
	.core = &viss_is_core_ops,
	.video = &viss_is_subdev_video_ops,
	.pad = &viss_is_subdev_pad_ops,
};

/* Supported manual ISO values */
static const s64 iso_qmenu[] = {
	50, 100, 200, 400, 800,
};

static const s64 exp_bias_qmenu[] = {
	-3, -2, -1, 0, 1, 2, 3
};

static int viss_is_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct viss_isp *isp = ctrl_to_viss_isp(ctrl);
	struct v4l2_event event;
	struct viss_v4l2_event_data *event_data;
	int ret = 0;

	memset(&event, 0, sizeof(event));
	event_data = (struct viss_v4l2_event_data *)&event.u.data[0];
	event.type = V4L2_EVENT_ISP_SET_ADJUST;

	switch (ctrl->id) {
	case V4L2_CID_CONTRAST:
		event_data->isp_adjust_cmd = ISP_ADJUST_CMD_MANUAL_CONTRAST;
		event_data->isp_adjust_val = (unsigned int)(ctrl->val + 10);
		break;
	case V4L2_CID_SATURATION:
		event_data->isp_adjust_cmd = ISP_ADJUST_CMD_MANUAL_SATURATION;
		event_data->isp_adjust_val = (unsigned int)(ctrl->val + 10);
		break;
	case V4L2_CID_SHARPNESS:
		event_data->isp_adjust_cmd = ISP_ADJUST_CMD_MANUAL_SHARPNESS;
		event_data->isp_adjust_val = (unsigned int)(ctrl->val + 10);
		break;
	case V4L2_CID_EXPOSURE_ABSOLUTE:
		break;
	case V4L2_CID_BRIGHTNESS:
		event_data->isp_adjust_cmd = ISP_ADJUST_CMD_MANUAL_BRIGHTNESS;
		event_data->isp_adjust_val = (unsigned int)(ctrl->val + 10);
		break;
	case V4L2_CID_HUE:
		event_data->isp_adjust_cmd = ISP_ADJUST_CMD_MANUAL_HUE;
		event_data->isp_adjust_val = (unsigned int)(ctrl->val + 10);
		break;
	case V4L2_CID_EXPOSURE_METERING:
		event_data->isp_adjust_cmd = ISP_ADJUST_CMD_EXP_METERING;
		event_data->isp_adjust_val = (unsigned int)ctrl->val;
		break;
	case V4L2_CID_WIN_POS_X:
		event_data->isp_adjust_cmd = ISP_ADJUST_CMD_WIN_POS_X;
		event_data->isp_adjust_val = (unsigned int)ctrl->val;
		break;
	case V4L2_CID_WIN_POS_Y:
		event_data->isp_adjust_cmd = ISP_ADJUST_CMD_WIN_POS_Y;
		event_data->isp_adjust_val = (unsigned int)ctrl->val;
		break;
	case V4L2_CID_WIN_WIDTH:
		event_data->isp_adjust_cmd = ISP_ADJUST_CMD_WIN_WIDTH;
		event_data->isp_adjust_val = (unsigned int)ctrl->val;
		break;
	case V4L2_CID_WIN_HEIGHT:
		event_data->isp_adjust_cmd = ISP_ADJUST_CMD_WIN_HEIGHT;
		event_data->isp_adjust_val = (unsigned int)ctrl->val;
		break;
	case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
		event_data->isp_adjust_cmd = ISP_ADJUST_CMD_AWB_MODE;
		event_data->isp_adjust_val = (unsigned int)ctrl->val;
		break;
	case V4L2_CID_WIDE_DYNAMIC_RANGE:
		event_data->isp_adjust_cmd = ISP_ADJUST_CMD_WDR_MODE;
		event_data->isp_adjust_val = (unsigned int)ctrl->val;
		break;
	case V4L2_CID_3A_LOCK:
		break;
	case V4L2_CID_ISO_SENSITIVITY_AUTO:
		break;
	case V4L2_CID_POWER_LINE_FREQUENCY:
		break;
	case V4L2_CID_COLORFX:
		break;
	case V4L2_CID_AUTO_EXPOSURE_BIAS:
		event_data->isp_adjust_cmd = ISP_ADJUST_CMD_EXP_BIAS;
		event_data->isp_adjust_val = (unsigned int)ctrl->val;
		break;
	default:
		ret = -EINVAL;
		break;
	}

	if (ret < 0) {
		PRT_ERR("s_ctrl failed: %s (%d)\n", ctrl->name, ctrl->val);
		return ret;
	}
	v4l2_event_queue(isp->subdev.devnode, &event);

	return 0;
}

static const struct v4l2_ctrl_ops viss_isp_ctrl_ops = {
	.s_ctrl	= viss_is_s_ctrl,
};

static const struct v4l2_ctrl_config isp_custom_ctrl[] = {
	{
	.ops	= &viss_isp_ctrl_ops,
	.id	= V4L2_CID_WIN_POS_X,
	.type	= V4L2_CTRL_TYPE_INTEGER,
	.name	= "windows start x position",
	.min = 0,
	.max = 4096,
	.step = 1,
	.def = 0,
	}, {
	.ops	= &viss_isp_ctrl_ops,
	.id	= V4L2_CID_WIN_POS_Y,
	.type	= V4L2_CTRL_TYPE_INTEGER,
	.name	= "windows start y position",
	.min = 0,
	.max = 4096,
	.step = 1,
	.def = 0,
	}, {
	.ops	= &viss_isp_ctrl_ops,
	.id	= V4L2_CID_WIN_WIDTH,
	.type	= V4L2_CTRL_TYPE_INTEGER,
	.name	= "windows width",
	.min = 0,
	.max = 4096,
	.step = 1,
	.def = 0,
	}, {
	.ops	= &viss_isp_ctrl_ops,
	.id	= V4L2_CID_WIN_HEIGHT,
	.type	= V4L2_CTRL_TYPE_INTEGER,
	.name	= "windows height",
	.min = 0,
	.max = 4096,
	.step = 1,
	.def = 0,
	}

};

static int viss_alloc_mem(struct viss_isp *isp)
{
	int i;

	isp->tnr_mem_cr.size = 100*1024;
	isp->tnr_mem_cr.vaddr = dma_alloc_coherent(&isp->pdev->dev,
		isp->tnr_mem_cr.size, &isp->tnr_mem_cr.paddr, GFP_KERNEL);
	if (!isp->tnr_mem_cr.vaddr) {
		PRT_ERR("memory alloc failed, size %d\n", isp->tnr_mem_cr.size);
		return -ENOMEM;
	}
	memset(isp->tnr_mem_cr.vaddr, 0, isp->tnr_mem_cr.size);

	/* statistic buffer */
	isp->stat[BGM_STAT_BUF].size = ALIGN(32*32*4*4, MEM_ALIGN);
	isp->stat[ISM_STAT_BUF].size = ALIGN(13*1024, MEM_ALIGN);
	isp->stat_total.size = 0;
	for (i = 0; i < MAX_STAT; i++)
		isp->stat_total.size +=	isp->stat[i].size;
	isp->stat_total.vaddr = dma_alloc_coherent(&isp->pdev->dev,
		isp->stat_total.size, &isp->stat_total.paddr, GFP_KERNEL);
	if (!isp->stat_total.vaddr) {
		PRT_ERR("memory alloc failed, size %d\n", isp->stat_total.size);
		return -ENOMEM;
	}
	memset(isp->stat_total.vaddr, 0, isp->stat_total.size);

	isp->stat[BGM_STAT_BUF].paddr = isp->stat_total.paddr;
	for (i = 1; i < MAX_STAT; i++) {
		isp->stat[i].paddr = isp->stat[i - 1].paddr +
			isp->stat[i - 1].size;
	}

	isp->stat[BGM_STAT_BUF].vaddr = isp->stat_total.vaddr;
	for (i = 1; i < MAX_STAT; i++) {
		isp->stat[i].vaddr = isp->stat[i - 1].vaddr +
			isp->stat[i - 1].size;
	}
	return 0;
}

static int viss_free_mem(struct viss_isp *isp)
{
	dma_free_coherent(&isp->pdev->dev, isp->tnr_mem_y.size,
		isp->tnr_mem_y.vaddr, isp->tnr_mem_y.paddr);

	dma_free_coherent(&isp->pdev->dev, isp->tnr_mem_cb.size,
		isp->tnr_mem_cb.vaddr, isp->tnr_mem_cb.paddr);

	dma_free_coherent(&isp->pdev->dev, isp->tnr_mem_cr.size,
		isp->tnr_mem_cr.vaddr, isp->tnr_mem_cr.paddr);

	dma_free_coherent(&isp->pdev->dev, isp->stat_total.size,
		isp->stat_total.vaddr, isp->stat_total.paddr);

	return 0;
}

static int viss_isp_subdev_create(struct viss_isp *isp)
{
	const struct v4l2_ctrl_ops *ops = &viss_isp_ctrl_ops;
	struct v4l2_ctrl_handler *handler = &isp->ctrls.handler;
	struct v4l2_subdev *sd = &isp->subdev;
	struct viss_isp_ctrls *ctrls = &isp->ctrls;
	int ret;

	PRT_TRACE_BEGIN("\n");

	ret = viss_alloc_mem(isp);
	if (ret)
		return ret;
	mutex_init(&isp->subdev_lock);

	v4l2_subdev_init(sd, &viss_is_subdev_ops);
	sd->grp_id = GRP_ID_VISS_IS;
	sd->flags |= V4L2_SUBDEV_FL_HAS_EVENTS | V4L2_SUBDEV_FL_HAS_DEVNODE;
	snprintf(sd->name, sizeof(sd->name), "viss-isp%d", isp->id);

	isp->subdev_pads[VISS_ISP_SD_PAD_SINK].flags = MEDIA_PAD_FL_SINK;
	isp->subdev_pads[VISS_ISP_SD_PAD_SRC_P0].flags = MEDIA_PAD_FL_SOURCE;
	isp->subdev_pads[VISS_ISP_SD_PAD_SRC_P1].flags = MEDIA_PAD_FL_SOURCE;
	ret = media_entity_init(&sd->entity, VISS_ISP_SD_PADS_NUM,
				isp->subdev_pads, 0);
	if (ret)
		return ret;

	v4l2_ctrl_handler_init(handler, 20 + ARRAY_SIZE(isp_custom_ctrl));

	ctrls->saturation = v4l2_ctrl_new_std(handler, ops, V4L2_CID_SATURATION,
						-10, 10, 1, 0);
	ctrls->brightness = v4l2_ctrl_new_std(handler, ops, V4L2_CID_BRIGHTNESS,
						-10, 10, 1, 0);
	ctrls->contrast = v4l2_ctrl_new_std(handler, ops, V4L2_CID_CONTRAST,
						-10, 10, 1, 0);
	ctrls->sharpness = v4l2_ctrl_new_std(handler, ops, V4L2_CID_SHARPNESS,
						-10, 10, 1, 0);
	ctrls->hue = v4l2_ctrl_new_std(handler, ops, V4L2_CID_HUE,
						-10, 10, 1, 0);

	ctrls->auto_wb = v4l2_ctrl_new_std_menu(handler, ops,
					V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE,
					V4L2_WHITE_BALANCE_SHADE, 0,
					V4L2_WHITE_BALANCE_AUTO);

	ctrls->exposure = v4l2_ctrl_new_std(handler, ops,
					V4L2_CID_EXPOSURE_ABSOLUTE,
					-4, 4, 1, 0);

	ctrls->exp_metering = v4l2_ctrl_new_std_menu(handler, ops,
					V4L2_CID_EXPOSURE_METERING, 3,
					0, V4L2_EXPOSURE_METERING_AVERAGE);

	v4l2_ctrl_new_std_menu(handler, ops, V4L2_CID_POWER_LINE_FREQUENCY,
					V4L2_CID_POWER_LINE_FREQUENCY_AUTO, 0,
					V4L2_CID_POWER_LINE_FREQUENCY_AUTO);
	/* ISO sensitivity */
	ctrls->auto_iso = v4l2_ctrl_new_std_menu(handler, ops,
			V4L2_CID_ISO_SENSITIVITY_AUTO, 1, 0,
			V4L2_ISO_SENSITIVITY_AUTO);

	ctrls->iso = v4l2_ctrl_new_int_menu(handler, ops,
			V4L2_CID_ISO_SENSITIVITY, ARRAY_SIZE(iso_qmenu) - 1,
			ARRAY_SIZE(iso_qmenu)/2 - 1, iso_qmenu);

	ctrls->exp_bias = v4l2_ctrl_new_int_menu(handler, ops,
		V4L2_CID_AUTO_EXPOSURE_BIAS, ARRAY_SIZE(exp_bias_qmenu) - 1,
		ARRAY_SIZE(exp_bias_qmenu)/2 - 1, exp_bias_qmenu);

	ctrls->aewb_lock = v4l2_ctrl_new_std(handler, ops,
					V4L2_CID_3A_LOCK, 0, 0x3, 0, 0);

	ctrls->wdr = v4l2_ctrl_new_std(handler, ops,
				V4L2_CID_WIDE_DYNAMIC_RANGE, 0, 1, 1, 1);

	ctrls->win_pos_x = v4l2_ctrl_new_custom(handler,
			&isp_custom_ctrl[0], NULL);
	ctrls->win_pos_y = v4l2_ctrl_new_custom(handler,
			&isp_custom_ctrl[1], NULL);
	ctrls->win_widht = v4l2_ctrl_new_custom(handler,
			&isp_custom_ctrl[2], NULL);
	ctrls->win_height = v4l2_ctrl_new_custom(handler,
			&isp_custom_ctrl[3], NULL);

	/* TODO: Add support for NEGATIVE_COLOR option */
	ctrls->colorfx = v4l2_ctrl_new_std_menu(handler, ops, V4L2_CID_COLORFX,
			V4L2_COLORFX_SET_CBCR, 0, V4L2_COLORFX_NONE);

	if (handler->error) {
		PRT_ERR("isp controls init failed!\n");
		media_entity_cleanup(&sd->entity);
		return handler->error;
	}

	v4l2_ctrl_auto_cluster(2, &ctrls->auto_iso,
			V4L2_ISO_SENSITIVITY_MANUAL, false);

	sd->ctrl_handler = handler;
	sd->internal_ops = &viss_is_subdev_internal_ops;
	sd->entity.ops = &viss_is_subdev_media_ops;
	v4l2_set_subdevdata(sd, isp);
	PRT_TRACE_END("\n");

	return 0;
}

static void viss_isp_subdev_destroy(struct viss_isp *isp)
{
	struct v4l2_subdev *sd = &isp->subdev;

	viss_free_mem(isp);
	v4l2_device_unregister_subdev(sd);
	media_entity_cleanup(&sd->entity);
	v4l2_ctrl_handler_free(&isp->ctrls.handler);
	v4l2_set_subdevdata(sd, NULL);
}

static ssize_t viss_is_reparse_sysfs_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct viss_is *is = platform_get_drvdata(pdev);
	struct viss_vid_buffer *vvb;
	int count = 0;
	int pending_buf0_cnt = 0;
	int pending_buf1_cnt = 0;
	int active_buf0_cnt = 0;
	int active_buf1_cnt = 0;

	list_for_each_entry(vvb, &is->viss->vv[0].pending_buf_q, list)
		pending_buf0_cnt++;
	list_for_each_entry(vvb, &is->viss->vv[1].pending_buf_q, list)
		pending_buf1_cnt++;

	list_for_each_entry(vvb, &is->viss->vv[0].active_buf_q, list)
		active_buf0_cnt++;
	list_for_each_entry(vvb, &is->viss->vv[1].active_buf_q, list)
		active_buf1_cnt++;

	count += sprintf(buf + count, "isp%d state:\n", is->isp.id);
	count += sprintf(buf + count, "frame time %ld(ms)\n",
			is->isp.frame_time/NSEC_PER_MSEC);
	count += sprintf(buf + count, "frame ts: %ld.%ld\n",
			is->isp.ts.tv_sec, is->isp.ts.tv_nsec);
	count += sprintf(buf + count, "frame count main %d, sub: %d\n",
			is->viss->vv[0].frame_count,
			is->viss->vv[1].frame_count);
	count += sprintf(buf + count, "lost count main %d, sub: %d\n",
			is->viss->vv[0].lost_count,
			is->viss->vv[1].lost_count);
	count += sprintf(buf + count, "requset buf count main %d, sub: %d\n",
			is->viss->vv[0].reqbufs_count,
			is->viss->vv[1].reqbufs_count);
	count += sprintf(buf + count, "pending buf count main %d, sub: %d\n",
			pending_buf0_cnt, pending_buf1_cnt);
	count += sprintf(buf + count, "active buf count main %d, sub: %d\n",
			active_buf0_cnt, active_buf1_cnt);
	count += sprintf(buf + count, "frame size input: %dx%d\n",
		is->isp.inp_frame.width, is->isp.inp_frame.height);
	count += sprintf(buf + count, "frame size main: %dx%d, sub: %dx%d\n",
		is->viss->vv[0].ctx->d_frame.width,
		is->viss->vv[0].ctx->d_frame.height,
		is->viss->vv[1].ctx->d_frame.width,
		is->viss->vv[1].ctx->d_frame.height);

	if (isp_offline) {
		struct offline_buf *fifo;
		int offline_fifo_cnt = 0;

		list_for_each_entry(fifo, &offline_fifo, list)
			offline_fifo_cnt++;
		count += sprintf(buf + count, "isp_offline_busy %d\n",
			isp_offline_busy);
		count += sprintf(buf + count, "offline_fifo_cnt %d\n",
			offline_fifo_cnt);
	}

	return count;
}

static ssize_t viss_is_reparse_sysfs_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct viss_is *is = platform_get_drvdata(pdev);
	int cmd, n;

	n = sscanf(buf, "cmd%d", &cmd);
	if (n != 1) {
		PRT_ERR("Invalid cmd (%s)\n", buf);
		return count;
	}
	PRT_DBG("%s.\n", buf);
	viss_isp_reparse(&is->isp, cmd);

	return count;
}

static DEVICE_ATTR(is_reparse, S_IWUSR | S_IRUGO,
	viss_is_reparse_sysfs_show, viss_is_reparse_sysfs_store);

static void viss_is_reset_isp(struct viss_is *is)
{
	struct viss_is_dev *viss = is->viss;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");
	usleep_range(1000, 2000);
	mutex_lock(&is->lock);
	mutex_lock(&viss->lock);

	if (!viss->vv[0].streaming && !viss->vv[1].streaming)
		goto unlock;

	viss_pipeline_call(viss, restart, &viss->pipeline, 0);

	usleep_range(1000, 2000);
	ret = viss_isp_restart(is);
	if (ret) {
		PRT_WARN("pipeline reset event ack timeout!\n");
		goto unlock;
	}

	viss_isp_write_dma_add(is);
	csp_isp_en_data_loss(is->regs, 1);

	if (viss->vv[0].streaming) {
		csp_isp_set_dma_main_path_y_size(viss->regs,
			viss->vv[0].ctx->d_frame.yuv_size[0],
			viss->vv[0].ctx->d_frame.yuv_size[1],
			viss->vv[0].ctx->d_frame.yuv_size[2]);
		csp_isp_dma_main_path_enable(is->regs, 1);
	}
	if (viss->vv[1].streaming) {
		csp_isp_set_dma_sub_path_y_size(viss->regs,
			viss->vv[1].ctx->d_frame.yuv_size[0],
			viss->vv[1].ctx->d_frame.yuv_size[1],
			viss->vv[1].ctx->d_frame.yuv_size[2]);
		csp_isp_dma_sub_path_enable(is->regs, 1);
	}

	viss_pipeline_call(viss, restart, &viss->pipeline, 1);
unlock:
	mutex_unlock(&viss->lock);
	mutex_unlock(&is->lock);
	PRT_TRACE_END("\n");
}

static void viss_is_restart_isp(struct work_struct *work)
{
	struct viss_is *is = container_of(work, struct viss_is, work);

	PRT_ERR("isp%d ^_^!\n", is->isp.id);
	if (isp_offline) {
		offline_fifo_release(is->isp.id);
		if (isp_offline_id == is->isp.id) {
			isp_offline_id = -1;
			isp_offline_busy = 0;
		}
		if (!test_bit(IS_ST_STREAM, &is->state)) {
			/* streaming finished irq */
			viss_post_irq(is, 0x1);
		} else {
			schedule_delayed_work(&is->watchdog,
				msecs_to_jiffies(1000));
		}
		return;
	}
	viss_is_reset_isp(is);
}

static void viss_is_watchdog(struct work_struct *work)
{
	struct viss_is *is = container_of(to_delayed_work(work),
						struct viss_is, watchdog);
	PRT_ERR("isp%d ^_^!\n", is->isp.id);
	if (isp_offline) {
		offline_fifo_release(is->isp.id);
		if (isp_offline_id == is->isp.id) {
			isp_offline_id = -1;
			isp_offline_busy = 0;
		}
		if (!test_bit(IS_ST_STREAM, &is->state)) {
			/* streaming finished irq */
			viss_post_irq(is, 0x1);
		} else {
			schedule_delayed_work(&is->watchdog,
				msecs_to_jiffies(1000));
		}
		return;
	}
	viss_is_reset_isp(is);
}

static irqreturn_t viss_is_irq_handler(int irq, void *priv)
{
	struct viss_is *is = priv;
	struct video_device *vdev = is->isp.subdev.devnode;
	unsigned long flags;
	unsigned int isp_status;
	struct v4l2_event event;

	PRT_TRACE_BEGIN("\n");

	if (isp_offline) {
		if (isp_offline_id < 0) {
			PRT_ERR("offline fifo is empty!\n");
			csp_isp_disable_isp_int(is->regs);
			return IRQ_HANDLED;
		}
		is = glb_is[isp_offline_id];
		vdev = is->isp.subdev.devnode;
	}

	spin_lock_irqsave(&is->slock, flags);

	cancel_delayed_work(&is->watchdog);

	isp_status = csp_isp_get_isp_int_status(is->regs);
	csp_isp_clr_isp_int(is->regs, isp_status);

	if (is->isp_reparse) {
		spin_unlock_irqrestore(&is->slock, flags);
		return IRQ_HANDLED;
	}

	if (isp_offline) {
		if (!test_bit(IS_ST_STREAM, &is->state)) {
			/* streaming finished irq */
			viss_post_irq(is, 0x1);
			spin_unlock_irqrestore(&is->slock, flags);
			return IRQ_HANDLED;
		}
	}

	if (!(isp_status & 0x1))
		schedule_delayed_work(&is->watchdog, msecs_to_jiffies(1000));

	if (isp_status & 0x4) {
		PRT_ERR("data loss error, status(0x%08x)\n", isp_status);
		isp_status = 0x4;
		csp_isp_en_data_loss(is->regs, 0);
		schedule_work(&is->work);
	}

	if (isp_status & 0x2) {
		memset(&event, 0, sizeof(event));
		event.type = V4L2_EVENT_ISP_FRAME_COMPLETE;
		v4l2_event_queue(vdev, &event);
	}

	if (isp_status & 0x40) {
		is->isp.vstart_cnt++;
		if (!isp_offline && is->isp.vstart_cnt >= 2) {
			if (is->viss->vv[0].streaming)
				viss_set_second_buf(is->viss, &is->viss->vv[0]);
			if (is->viss->vv[1].streaming)
				viss_set_second_buf(is->viss, &is->viss->vv[1]);
		}
		memset(&event, 0, sizeof(event));
		event.type = V4L2_EVENT_VSYNC;
		v4l2_event_queue(vdev, &event);
	}

	/* get isp statistic */
	if (isp_offline)
		csp_isp_dump_stat(is->regs, is->viss->reg_save);

	viss_post_irq(is, isp_status);

	spin_unlock_irqrestore(&is->slock, flags);

	PRT_TRACE_END("\n");

	return IRQ_HANDLED;
}

static int viss_is_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct viss_is *is;
	int ret;

	PRT_TRACE_BEGIN("\n");

	is = devm_kzalloc(&pdev->dev, sizeof(*is), GFP_KERNEL);
	if (!is) {
		PRT_ERR("create struct viss_is failed!\n");
		return -ENOMEM;
	}

	is->pdev = pdev;
	is->isp.pdev = pdev;

	spin_lock_init(&is->slock);
	INIT_WORK(&is->work, viss_is_restart_isp);
	INIT_DELAYED_WORK(&is->watchdog, viss_is_watchdog);
	mutex_init(&is->lock);
	init_completion(&is->cmd_ack.wait_complete);

	is->isp.id = of_alias_get_id(dev->of_node, "viss_is");

	is->regs = of_iomap(dev->of_node, 0);
	if (!is->regs) {
		if (is->isp.id == 0) {
			PRT_ERR("map isp%d reg failed!\n", is->isp.id);
			return PTR_ERR(is->regs);
		} else {
			is->regs = glb_is[0]->regs;
			goto skip_irq;
		}
	}

	is->irq = irq_of_parse_and_map(dev->of_node, 0);
	if (is->irq < 0) {
		PRT_ERR("isp%d no irq found\n", is->isp.id);
		return is->irq;
	}
	PRT_DBG("isp%d irq %d.\n", is->isp.id, is->irq);

	ret = request_irq(is->irq, viss_is_irq_handler, 0, dev_name(dev), is);
	if (ret < 0) {
		PRT_ERR("irq%d request failed\n", is->irq);
		return -EINVAL;
	}

skip_irq:
	platform_set_drvdata(pdev, is);

	ret = viss_isp_subdev_create(&is->isp);
	if (ret < 0)
		goto err_irq;

	ret = device_create_file(&pdev->dev, &dev_attr_is_reparse);
	if (ret)
		goto err_vb;

	INIT_LIST_HEAD(&offline_fifo);

	glb_is[is->isp.id] = is;
	PRT_TRACE_END("viss_is%d registered successfully.\n", is->isp.id);
	return 0;
err_vb:
	viss_isp_subdev_destroy(&is->isp);
err_irq:
	free_irq(is->irq, is);
	return ret;
}

static int viss_is_remove(struct platform_device *pdev)
{
	struct viss_is *is = platform_get_drvdata(pdev);

	flush_work(&is->work);
	cancel_delayed_work(&is->watchdog);
	pm_runtime_disable(&pdev->dev);
	pm_runtime_set_suspended(&pdev->dev);
	free_irq(is->irq, is);
	viss_isp_subdev_destroy(&is->isp);

	return 0;
}

#ifdef CONFIG_PM_RUNTIME
static int viss_is_runtime_resume(struct device *dev)
{
	/* TODO */
	return 0;
}

static int viss_is_runtime_suspend(struct device *dev)
{
	return 0;
}
#endif

#ifdef CONFIG_PM_SLEEP
static int viss_is_resume(struct device *dev)
{
	/* TODO: */
	return 0;
}

static int viss_is_suspend(struct device *dev)
{
#if 0
	struct viss_is *is = dev_get_drvdata(dev);

	/* TODO: */
#endif
	return 0;
}
#endif /* CONFIG_PM_SLEEP */

static const struct dev_pm_ops viss_is_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(viss_is_suspend, viss_is_resume)
	SET_RUNTIME_PM_OPS(viss_is_runtime_suspend, viss_is_runtime_resume,
			   NULL)
};

static const struct of_device_id viss_is_of_match[] = {
	{
		.compatible = "lombo,viss-is",
	},
};

MODULE_DEVICE_TABLE(of, viss_is_of_match);

static struct platform_driver viss_is_driver = {
	.probe		= viss_is_probe,
	.remove		= viss_is_remove,
	.driver = {
		.of_match_table	= viss_is_of_match,
		.name		= VISS_IS_DRV_NAME,
		.owner		= THIS_MODULE,
		.pm		= &viss_is_pm_ops,
	}
};

module_platform_driver(viss_is_driver);
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" VISS_IS_DRV_NAME);

