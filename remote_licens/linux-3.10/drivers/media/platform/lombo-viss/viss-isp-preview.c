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


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/bug.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/pm_runtime.h>
#include <linux/list.h>
#include <linux/slab.h>

#include <linux/videodev2.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-mem2mem.h>
#include <media/videobuf2-core.h>
#include <media/videobuf2-dma-contig.h>

#include <linux/of_i2c.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <media/v4l2-of.h>

#include "media-dev.h"
#include "viss-isp-preview.h"
#include "viss.h"
#include "viss-m2m.h"

#define PRE_SET_SECOND_BUF 0

#define ISP_MEM_STATUS_MP_FRAME_END			(1 << 0)
#define ISP_MEM_STATUS_SP_FRAME_END			(1 << 1)

#define ISP_MEM_STATUS_MP_Y_FIFO_FULL			(1 << 0)
#define ISP_MEM_STATUS_MP_CB_FIFO_FULL			(1 << 1)
#define ISP_MEM_STATUS_MP_CR_FIFO_FULL			(1 << 2)
#define ISP_MEM_STATUS_SP_Y_FIFO_FULL			(1 << 4)
#define ISP_MEM_STATUS_SP_CB_FIFO_FULL			(1 << 5)
#define ISP_MEM_STATUS_SP_CR_FIFO_FULL			(1 << 6)

struct viss_is_dev *glb_viss[VISS_IS_MAX_NUM];

static struct viss_fmt viss_formats[] = {
	{
		.name		= "RGB565", /* for s3c2440 fimc0 camif test */
		.fourcc		= V4L2_PIX_FMT_RGB565,
		.depth		= { 16 },
		.color		= VISS_FMT_RGB565,
		.memplanes	= 1,
		.colplanes	= 1,
		.flags		= FMT_FLAGS_M2M | FMT_FLAGS_CAM,
	}, {
		.name		= "BGR666",
		.fourcc		= V4L2_PIX_FMT_BGR666,
		.depth		= { 32 },
		.color		= VISS_FMT_RGB666,
		.memplanes	= 1,
		.colplanes	= 1,
		.flags		= FMT_FLAGS_M2M,
	}, {
		.name		= "ARGB8888, 32 bpp",
		.fourcc		= V4L2_PIX_FMT_RGB32,
		.depth		= { 32 },
		.color		= VISS_FMT_RGB888,
		.memplanes	= 1,
		.colplanes	= 1,
		.flags		= FMT_FLAGS_M2M | FMT_HAS_ALPHA,
	}, {
		.name		= "ARGB1555",
		.fourcc		= V4L2_PIX_FMT_RGB555,
		.depth		= { 16 },
		.color		= VISS_FMT_RGB555,
		.memplanes	= 1,
		.colplanes	= 1,
		.flags		= FMT_FLAGS_M2M_OUT | FMT_HAS_ALPHA,
	}, {
		.name		= "ARGB4444",
		.fourcc		= V4L2_PIX_FMT_RGB444,
		.depth		= { 16 },
		.color		= VISS_FMT_RGB444,
		.memplanes	= 1,
		.colplanes	= 1,
		.flags		= FMT_FLAGS_M2M_OUT | FMT_HAS_ALPHA,
	}, {
		.name		= "YUV 4:4:4",
		.mbus_code	= V4L2_MBUS_FMT_YUV10_1X30,
		.flags		= FMT_FLAGS_WRITEBACK,
	}, {
		.name		= "YUV 4:2:2 packed, YCbYCr",
		.fourcc		= V4L2_PIX_FMT_YUYV,
		.depth		= { 16 },
		.color		= VISS_FMT_YCBYCR422,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_YUYV8_2X8,
		.flags		= FMT_FLAGS_M2M | FMT_FLAGS_CAM,
	}, {
		.name		= "YUV 4:2:2 packed, CbYCrY",
		.fourcc		= V4L2_PIX_FMT_UYVY,
		.depth		= { 16 },
		.color		= VISS_FMT_CBYCRY422,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_UYVY8_2X8,
		.flags		= FMT_FLAGS_M2M | FMT_FLAGS_CAM,
	}, {
		.name		= "YUV 4:2:2 packed, CrYCbY",
		.fourcc		= V4L2_PIX_FMT_VYUY,
		.depth		= { 16 },
		.color		= VISS_FMT_CRYCBY422,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_VYUY8_2X8,
		.flags		= FMT_FLAGS_M2M | FMT_FLAGS_CAM,
	}, {
		.name		= "YUV 4:2:2 packed, YCrYCb",
		.fourcc		= V4L2_PIX_FMT_YVYU,
		.depth		= { 16 },
		.color		= VISS_FMT_YCRYCB422,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_YVYU8_2X8,
		.flags		= FMT_FLAGS_M2M | FMT_FLAGS_CAM,
	}, {
		.name		= "YUV 4:2:2 planar, Y/Cb/Cr",
		.fourcc		= V4L2_PIX_FMT_YUV422P,
		.depth		= { 16 },
		.color		= VISS_FMT_YCBYCR422,
		.memplanes	= 1,
		.colplanes	= 3,
		.flags		= FMT_FLAGS_M2M,
	}, {
		.name		= "YUV 4:2:2 planar, Y/CbCr",
		.fourcc		= V4L2_PIX_FMT_NV16,
		.depth		= { 16 },
		.color		= VISS_FMT_YCBYCR422,
		.memplanes	= 1,
		.colplanes	= 2,
		.flags		= FMT_FLAGS_M2M,
	}, {
		.name		= "YUV 4:2:2 planar, Y/CbCr",
		.fourcc		= V4L2_PIX_FMT_NV16M,
		.depth		= { 8, 8 },
		.color		= VISS_FMT_YCBYCR422,
		.memplanes	= 2,
		.colplanes	= 2,
		.flags		= FMT_FLAGS_M2M,
	}, {
		.name		= "YUV 4:2:2 planar, Y/CrCb",
		.fourcc		= V4L2_PIX_FMT_NV61,
		.depth		= { 16 },
		.color		= VISS_FMT_YCRYCB422,
		.memplanes	= 1,
		.colplanes	= 2,
		.flags		= FMT_FLAGS_M2M,
	}, {
		.name		= "YUV 4:2:2 planar, Y/CrCb",
		.fourcc		= V4L2_PIX_FMT_NV61M,
		.depth		= { 8, 8 },
		.color		= VISS_FMT_YCRYCB422,
		.memplanes	= 2,
		.colplanes	= 2,
		.flags		= FMT_FLAGS_M2M,
	}, {
		.name		= "YUV 4:2:0 planar, YCbCr",
		.fourcc		= V4L2_PIX_FMT_YUV420,
		.depth		= { 12 },
		.color		= VISS_FMT_YCBCR420,
		.memplanes	= 1,
		.colplanes	= 3,
		.flags		= FMT_FLAGS_M2M,
	}, {
		.name		= "YUV 4:2:0 planar, YCrCb",
		.fourcc		= V4L2_PIX_FMT_YVU420,
		.depth		= { 12 },
		.color		= VISS_FMT_YCRCB420,
		.memplanes	= 1,
		.colplanes	= 3,
		.flags		= FMT_FLAGS_M2M,
	}, {
		.name		= "YUV 4:2:0 planar, Y/CbCr",
		.fourcc		= V4L2_PIX_FMT_NV12,
		.depth		= { 12 },
		.color		= VISS_FMT_YCBCR420,
		.memplanes	= 1,
		.colplanes	= 2,
		.flags		= FMT_FLAGS_M2M,
	}, {
		.name		= "YUV 4:2:0 non-contig. 2p, Y/CbCr",
		.fourcc		= V4L2_PIX_FMT_NV12M,
		.color		= VISS_FMT_YCBCR420,
		.depth		= { 8, 4 },
		.memplanes	= 2,
		.colplanes	= 2,
		.flags		= FMT_FLAGS_M2M,
	}, {
		.name		= "YUV 4:2:0 non-contig. 3p, Y/Cb/Cr",
		.fourcc		= V4L2_PIX_FMT_YUV420M,
		.color		= VISS_FMT_YCBCR420,
		.depth		= { 8, 2, 2 },
		.memplanes	= 3,
		.colplanes	= 3,
		.flags		= FMT_FLAGS_M2M,
	}, {
		.name		= "YUV 4:2:0 non-contig. 3p, Y/Cr/Cb",
		.fourcc		= V4L2_PIX_FMT_YVU420M,
		.color		= VISS_FMT_YCRCB420,
		.depth		= { 8, 2, 2 },
		.memplanes	= 3,
		.colplanes	= 3,
		.flags		= FMT_FLAGS_M2M,
	}, {
		.name		= "YUV 4:2:0 non-contig. 2p, tiled",
		.fourcc		= V4L2_PIX_FMT_NV12MT,
		.color		= VISS_FMT_YCBCR420,
		.depth		= { 8, 4 },
		.memplanes	= 2,
		.colplanes	= 2,
		.flags		= FMT_FLAGS_M2M,
	},
};

/**
 * viss_find_format - lookup viss color format by fourcc or media bus format
 * @pixelformat: fourcc to match, ignored if null
 * @mbus_code: media bus code to match, ignored if null
 * @mask: the color flags to match
 * @index: offset in the viss_formats array, ignored if negative
 */
static struct viss_fmt *viss_find_format(const u32 *pixelformat,
			const u32 *mbus_code, unsigned int mask, int index)
{
	struct viss_fmt *fmt, *def_fmt = NULL;
	unsigned int i;
	int id = 0;

	if (index >= (int)ARRAY_SIZE(viss_formats))
		return NULL;

	for (i = 0; i < ARRAY_SIZE(viss_formats); ++i) {
		fmt = &viss_formats[i];
		if (!(fmt->flags & mask))
			continue;
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

/**
 * viss_adjust_format - adjust bytesperline/sizeimage for each plane
 * @fmt: viss pixel format description (input)
 * @width: requested pixel width
 * @height: requested pixel height
 * @pix: multi-plane format to adjust
 */
static void viss_adjust_format(struct viss_fmt *fmt, u32 width,
		u32 height, struct v4l2_pix_format_mplane *pix)
{
	u32 bytesperline = 0;
	int i;

	PRT_TRACE_BEGIN("\n");

	pix->colorspace = V4L2_COLORSPACE_JPEG;
	pix->field = V4L2_FIELD_NONE;
	pix->num_planes = fmt->memplanes;
	pix->pixelformat = fmt->fourcc;
	pix->height = height;
	pix->width = width;

	PRT_DBG("request set fmt->name: %s, pix->num_planes: %d.\n",
		fmt->name, pix->num_planes);
	PRT_DBG("width:%d, height: %d.\n", pix->width, pix->height);

	for (i = 0; i < pix->num_planes; ++i) {
		struct v4l2_plane_pix_format *plane_fmt = &pix->plane_fmt[i];
		u32 bpl = plane_fmt->bytesperline;

		if (fmt->colplanes > 1 && (bpl == 0 || bpl < pix->width))
			bpl = pix->width; /* Planar */

		if (fmt->colplanes == 1 && /* Packed */
		    (bpl == 0 || ((bpl * 8) / fmt->depth[i]) < pix->width))
			bpl = (pix->width * fmt->depth[0]) / 8;
		/*
		 * Currently bytesperline for each plane is same, except
		 * V4L2_PIX_FMT_YUV420M format. This calculation may need
		 * to be changed when other multi-planar formats are added
		 * to the viss_formats[] array.
		 */
		if (i == 0)
			bytesperline = bpl;
		else if (i == 1 && fmt->memplanes == 3)
			bytesperline /= 2;

		plane_fmt->bytesperline = bytesperline;
		plane_fmt->sizeimage = max((pix->width * pix->height *
				   fmt->depth[i]) / 8, plane_fmt->sizeimage);
	}
	PRT_TRACE_END("\n");
}

/* The color format (colplanes, memplanes) must be already configured. */
static int viss_prepare_addr(struct vb2_buffer *vb,
		      struct viss_is_frame *frame, struct viss_addr *paddr)
{
	int ret = 0;

	PRT_TRACE_BEGIN("\n");

	if (vb == NULL || frame == NULL)
		return -EINVAL;

	frame->yuv_size[0] = frame->width * frame->height;
	switch (frame->fmt->colplanes) {
	case 2:
		if (VISS_FMT_YCBCR420 == frame->fmt->color ||
		    VISS_FMT_YCRCB420 == frame->fmt->color)
			frame->yuv_size[1] = frame->yuv_size[0] / 2;
		else
			frame->yuv_size[1] = frame->yuv_size[0];
		frame->yuv_size[2] = 0;
		break;
	case 3:
		if (VISS_FMT_YCBCR420 == frame->fmt->color ||
		    VISS_FMT_YCRCB420 == frame->fmt->color) {
			frame->yuv_size[1] = frame->yuv_size[0] / 4;
			frame->yuv_size[2] = frame->yuv_size[0] / 4;
		} else {
			frame->yuv_size[1] = frame->yuv_size[0] / 2;
			frame->yuv_size[2] = frame->yuv_size[0] / 2;
		}
		break;
	default:
		frame->yuv_size[1] = 0;
		frame->yuv_size[2] = 0;
		return -EINVAL;
	}

	PRT_DBG("memplanes = %d, colplanes = %d, yuv_size = %d %d %d\n",
		frame->fmt->memplanes, frame->fmt->colplanes,
		frame->yuv_size[0], frame->yuv_size[1], frame->yuv_size[2]);

	if (vb->v4l2_buf.memory == V4L2_MEMORY_DMABUF)
		paddr->y = vb2_dma_contig_plane_dma_addr(vb, 0) +
					vb->v4l2_planes[0].data_offset;
	else
		paddr->y = vb2_dma_contig_plane_dma_addr(vb, 0);

	if (frame->fmt->memplanes == 1) {
		switch (frame->fmt->colplanes) {
		case 1:
			paddr->cb = 0;
			paddr->cr = 0;
			break;
		case 2:
			/* decompose Y into Y/Cb */
			paddr->cb = (u32)(paddr->y + frame->yuv_size[0]);
			paddr->cr = 0;
			break;
		case 3:
			/* decompose Y into Y/Cb/Cr */
			paddr->cb = (u32)(paddr->y + frame->yuv_size[0]);
			paddr->cr = (u32)(paddr->cb + frame->yuv_size[1]);
			break;
		default:
			return -EINVAL;
		}
	} else if (!frame->fmt->mdataplanes) {
		if (frame->fmt->memplanes >= 2) {
			paddr->cb = vb2_dma_contig_plane_dma_addr(vb, 1);
			if (vb->v4l2_buf.memory == V4L2_MEMORY_DMABUF)
				paddr->cb = paddr->cb +
					vb->v4l2_planes[1].data_offset;
		}

		if (frame->fmt->memplanes == 3) {
			paddr->cr = vb2_dma_contig_plane_dma_addr(vb, 2);
			if (vb->v4l2_buf.memory == V4L2_MEMORY_DMABUF)
				paddr->cr =  paddr->cr +
					vb->v4l2_planes[2].data_offset;
		}
	}

	PRT_DBG("PHYS_ADDR: y=0x%X,cb=0x%X,cr=0x%X,ret=%d\n",
	    paddr->y, paddr->cb, paddr->cr, ret);
	PRT_TRACE_END("\n");
	return ret;
}


/*
 * Reinitialize the driver so it is ready to start the streaming again.
 * Set vv->state to indicate stream off and the hardware shut down state.
 * If not suspending (@suspend is false), return any buffers to videobuf2.
 * Otherwise put any owned buffers onto the pending buffers queue, so they
 * can be re-spun when the device is being resumed. Also perform VISS
 * software reset and disable streaming on the whole pipeline if required.
 */
static int viss_preview_state_cleanup(struct viss_is_dev *viss,
		struct viss_video *vv, bool suspend)
{
	struct viss_vid_buffer *buf;
	unsigned long flags;

	PRT_TRACE_BEGIN("\n");

	spin_lock_irqsave(&viss->slock, flags);

	vv->state &= ~(1 << ST_PREV_RUN | 1 << ST_PREV_SHUT |
			 1 << ST_PREV_STREAM);
	if (suspend)
		vv->state |= (1 << ST_PREV_SUSPENDED);
	else
		vv->state &= ~(1 << ST_PREV_SUSPENDED);

	/* Release unused buffers */
	while (!suspend && !list_empty(&vv->pending_buf_q)) {
		buf = viss_pending_queue_pop(vv);
		vb2_buffer_done(&buf->vb, VB2_BUF_STATE_ERROR);
	}
	/* If suspending put unused buffers onto pending queue */
	while (!list_empty(&vv->active_buf_q)) {
		buf = viss_active_queue_pop_tail(vv);
		if (suspend)
			viss_pending_queue_add_front(vv, buf);
		else
			vb2_buffer_done(&buf->vb, VB2_BUF_STATE_ERROR);
	}

	/* Note: hardware reset */
	spin_unlock_irqrestore(&viss->slock, flags);
	PRT_TRACE_END("\n");
	/* we will stream off until there is only one path streaming */
	if (viss->vv[0].streaming && viss->vv[1].streaming)
		return 0;
	if (!viss->vv[0].streaming && !viss->vv[1].streaming)
		return 0;
	return viss_pipeline_call(viss, set_stream, &viss->pipeline, 0);
}

static int viss_stop_preview(struct viss_is_dev *viss,
		struct viss_video *vv, bool suspend)
{
	unsigned long flags;
	void *base;

	if (isp_offline)
		base = viss->reg_save;
	else
		base = viss->regs;

	PRT_TRACE_BEGIN("\n");
	if (!viss_preview_active(viss, vv))
		return 0;

	spin_lock_irqsave(&viss->slock, flags);
	set_bit(ST_PREV_SHUT, &vv->state);

	if (vv->path == 0)
		csp_isp_dma_main_path_enable(base, 0);
	else
		csp_isp_dma_sub_path_enable(base, 0);

	spin_unlock_irqrestore(&viss->slock, flags);

	wait_event_timeout(viss->irq_queue,
			   !test_bit(ST_PREV_SHUT, &vv->state),
			   VISS_SHUTDOWN_TIMEOUT); /* 200 ms */
	PRT_TRACE_END("\n");

	return viss_preview_state_cleanup(viss, vv, suspend);
}

static int viss_set_next_buf(struct viss_is_dev *viss,
		struct viss_video *vv)
{
	struct viss_vid_buffer *vbuf;
	u32	y, u, v;
	void *base;

	PRT_TRACE_BEGIN("\n");
	if (isp_offline)
		base = viss->reg_save;
	else
		base = viss->regs;

	if (!list_empty(&vv->pending_buf_q)) {
		/* prepare next dma address */
		vbuf = viss_pending_queue_pop(vv);
		if (vv->ctx->d_frame.fmt->fourcc == V4L2_PIX_FMT_YVU420 ||
		    vv->ctx->d_frame.fmt->fourcc == V4L2_PIX_FMT_YVU420M) {
			y = vbuf->paddr.y;
			u = vbuf->paddr.cr;
			v = vbuf->paddr.cb;
		} else {
			y = vbuf->paddr.y;
			u = vbuf->paddr.cb;
			v = vbuf->paddr.cr;
		}
		if (vv->path == 0)
			csp_isp_set_dma_main_path_add(base, y, u, v);
		else
			csp_isp_set_dma_sub_path_add(base, y, u, v);

		viss_active_queue_add(vv, vbuf);
	}

	PRT_TRACE_END("\n");
	return 0;
}

static int viss_start_preview(struct viss_is_dev *viss,
			struct viss_video *vv)
{
	int ret = 0;
	struct viss_is_frame *in = &vv->ctx->s_frame;
	struct viss_is_frame *out = &vv->ctx->d_frame;
	void *base;

	vv->lost_count = 0;
	vv->frame_count = 0;
	vv->first_frame = true;

	if (isp_offline)
		base = viss->reg_save;
	else
		base = viss->regs;

	if (!test_and_set_bit(ST_PREV_STREAM, &vv->state)) {
		/* set first frame address */
		viss_set_next_buf(viss, vv);
		csp_isp_dma_config_update(base);

		if (!viss->vv[0].streaming && !viss->vv[1].streaming)
			ret = viss_pipeline_call(viss, set_stream,
					&viss->pipeline, 1);
		if (vv->path == 0) {
			csp_isp_set_main_path_scale(base, in->width,
				in->height, out->width, out->height);
			csp_isp_set_dma_main_path_y_size(base,
				out->yuv_size[0],
				out->yuv_size[1],
				out->yuv_size[2]);
			csp_isp_dma_main_path_enable(base, 1);
		} else {
			csp_isp_set_sub_path_scale(base, in->width,
				in->height, out->width, out->height);
			csp_isp_set_dma_sub_path_y_size(base,
				out->yuv_size[0],
				out->yuv_size[1],
				out->yuv_size[2]);
			csp_isp_dma_sub_path_enable(base, 1);
		}
#if PRE_SET_SECOND_BUF
		viss_set_second_buf(viss, vv);
#endif
	}
	return ret;
}

static int start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct viss_video *vv = q->drv_priv;
	struct viss_is_dev *viss = vv->ctx->viss;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");
	ret = viss_start_preview(viss, vv);
	PRT_TRACE_END("\n");

	return ret;
}

static int stop_streaming(struct vb2_queue *q)
{
	struct viss_video *vv = q->drv_priv;
	struct viss_is_dev *viss = vv->ctx->viss;

	PRT_TRACE_BEGIN("\n");

	if (!viss_preview_active(viss, vv))
		return -EINVAL;

	PRT_TRACE_END("\n");
	return viss_stop_preview(viss, vv, false);
}

static int queue_setup(struct vb2_queue *vq, const struct v4l2_format *pfmt,
		       unsigned int *num_buffers, unsigned int *num_planes,
		       unsigned int sizes[], void *allocators[])
{
	const struct v4l2_pix_format_mplane *pixm = NULL;
	struct viss_video *vv = vq->drv_priv;
	struct viss_ctx *ctx = vv->ctx;
	struct viss_is_frame *frame = &ctx->d_frame;
	struct viss_fmt *fmt = frame->fmt;
	unsigned long wh;
	int i;

	PRT_TRACE_BEGIN("\n");

	if (pfmt) {
		pixm = &pfmt->fmt.pix_mp;
		fmt = viss_find_format(&pixm->pixelformat, NULL, 0, -1);
		wh = pixm->width * pixm->height;
	} else {
		wh = frame->width * frame->height;
	}

	if (fmt == NULL)
		return -EINVAL;

	*num_planes = fmt->memplanes;

	for (i = 0; i < fmt->memplanes; i++) {
		unsigned int size = (wh * fmt->depth[i]) / 8;
		if (pixm)
			sizes[i] = max(size, pixm->plane_fmt[i].sizeimage);
		else
			sizes[i] = size;
		allocators[i] = ctx->viss->alloc_ctx;
	}
	PRT_TRACE_END("\n");

	return 0;
}

static int buffer_prepare(struct vb2_buffer *vb)
{
	struct viss_video *vv = vb2_get_drv_priv(vb->vb2_queue);
	struct viss_ctx *ctx = vv->ctx;
	int i;

	PRT_TRACE_BEGIN("\n");

	if (ctx->d_frame.fmt == NULL)
		return -EINVAL;

	for (i = 0; i < ctx->d_frame.fmt->memplanes; i++) {
		unsigned long size = ctx->d_frame.payload[i];

		if (vb2_plane_size(vb, i) < size) {
			PRT_DBG("User buffer too small (%ld < %ld)\n",
				 vb2_plane_size(vb, i), size);
			return -EINVAL;
		}
		vb2_set_plane_payload(vb, i, size);
	}
	PRT_TRACE_END("\n");

	return 0;
}

static void buffer_queue(struct vb2_buffer *vb)
{
	struct viss_vid_buffer *buf =
		container_of(vb, struct viss_vid_buffer, vb);
	struct viss_video *vv = vb2_get_drv_priv(vb->vb2_queue);
	struct viss_ctx *ctx = vv->ctx;
	struct viss_is_dev *viss = ctx->viss;
	unsigned long flags;

	PRT_TRACE_BEGIN("\n");
	spin_lock_irqsave(&viss->slock, flags);
	viss_prepare_addr(&buf->vb, &ctx->d_frame, &buf->paddr);
	viss_pending_queue_add(vv, buf);

	if (vb2_is_streaming(&vv->vbq) && list_is_singular(&vv->active_buf_q))
		viss_set_next_buf(viss, vv);

	spin_unlock_irqrestore(&viss->slock, flags);
	PRT_TRACE_END("\n");
}

static struct vb2_ops viss_preview_qops = {
	.queue_setup		= queue_setup,
	.buf_prepare		= buffer_prepare,
	.buf_queue		= buffer_queue,
	.wait_prepare		= vb2_ops_wait_prepare,
	.wait_finish		= vb2_ops_wait_finish,
	.start_streaming	= start_streaming,
	.stop_streaming		= stop_streaming,
};

/*
 * V4L2 controls handling
 */

static int __viss_s_ctrl(struct viss_ctx *ctx, struct v4l2_ctrl *ctrl)
{
	return 0;
}

static int viss_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct viss_ctx *ctx = ctrl_to_ctx(ctrl);
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&ctx->viss->slock, flags);
	ret = __viss_s_ctrl(ctx, ctrl);
	spin_unlock_irqrestore(&ctx->viss->slock, flags);

	return ret;
}

static const struct v4l2_ctrl_ops viss_ctrl_ops = {
	.s_ctrl = viss_s_ctrl,
};

static int viss_ctrls_create(struct viss_ctx *ctx)
{
	struct viss_ctrls *ctrls = &ctx->ctrls;
	struct v4l2_ctrl_handler *handler = &ctrls->handler;

	PRT_TRACE_BEGIN("\n");
	if (ctx->ctrls.ready)
		return 0;

	v4l2_ctrl_handler_init(handler, 3);

	ctrls->rotate = v4l2_ctrl_new_std(handler, &viss_ctrl_ops,
					V4L2_CID_ROTATE, 0, 270, 90, 0);
	ctrls->hflip = v4l2_ctrl_new_std(handler, &viss_ctrl_ops,
					V4L2_CID_HFLIP, 0, 1, 1, 0);
	ctrls->vflip = v4l2_ctrl_new_std(handler, &viss_ctrl_ops,
					V4L2_CID_VFLIP, 0, 1, 1, 0);
	if (!handler->error)
		ctrls->ready = true;
	PRT_TRACE_END("\n");

	return handler->error;
}

static void viss_ctrls_delete(struct viss_ctx *ctx)
{
	struct viss_ctrls *ctrls = &ctx->ctrls;

	if (ctrls->ready) {
		v4l2_ctrl_handler_free(&ctrls->handler);
		ctrls->ready = false;
	}
}

/**
 * viss_preview_ctrls_create - initialize the control handler
 * Initialize the preview video node control handler and fill it
 * with the VISS controls. Inherit any sensor's controls if the
 * 'user_subdev_api' flag is false (default behaviour).
 * This function need to be called with the graph mutex held.
 */
int viss_preview_ctrls_create(struct viss_is_dev *viss,
			struct viss_video *vv)
{
	struct v4l2_subdev *sensor = viss->pipeline.subdevs[IDX_SENSOR];
	struct v4l2_subdev *isp = viss->pipeline.subdevs[IDX_IS_ISP];
	int ret;

	PRT_TRACE_BEGIN("\n");

	if (WARN_ON(vv->ctx == NULL))
		return -ENXIO;
	if (vv->ctx->ctrls.ready)
		return 0;

	ret = viss_ctrls_create(vv->ctx);

	if (ret || !sensor || !vv->ctx->ctrls.ready)
		return ret;
	PRT_TRACE_END("\n");

	return v4l2_ctrl_add_handler(&vv->ctx->ctrls.handler,
				     isp->ctrl_handler, NULL);
}

static int viss_preview_open(struct file *file)
{
	struct viss_video *vv = video_drvdata(file);
	struct viss_is_dev *viss = vv->ctx->viss;
	int ret = -EBUSY;
	PRT_TRACE_BEGIN("\n");

	PRT_DBG("pid=%d,state=0x%lx", task_pid_nr(current), vv->state);

	mutex_lock(&vv->vfd.entity.parent->graph_mutex);
	mutex_lock(&viss->lock);

	PRT_DBG("viss name=%s\n", viss->pdev->name);

	set_bit(ST_PREV_BUSY, &vv->state);
	ret = pm_runtime_get_sync(&viss->pdev->dev);
	if (ret < 0)
		goto unlock;

	ret = v4l2_fh_open(file);
	if (ret) {
		pm_runtime_put(&viss->pdev->dev);
		goto unlock;
	}

	if (v4l2_fh_is_singular_file(file)) {
		if ((vv->path == 0 && !viss_preview_busy(&viss->vv[1])) ||
		    (vv->path == 1 && !viss_preview_busy(&viss->vv[0])))
			ret = viss_pipeline_call(viss, open, &viss->pipeline,
						&vv->vfd.entity, true);
		if (!ret)
			ret = viss_preview_ctrls_create(viss, vv);

		if (ret < 0) {
			clear_bit(ST_PREV_BUSY, &vv->state);
			pm_runtime_put_sync(&viss->pdev->dev);
			v4l2_fh_release(file);
		}
	}
	PRT_TRACE_END("\n");
unlock:
	mutex_unlock(&viss->lock);
	mutex_unlock(&vv->vfd.entity.parent->graph_mutex);
	return ret;
}

static int viss_preview_release(struct file *file)
{
	struct viss_video *vv = video_drvdata(file);
	struct viss_is_dev *viss = vv->ctx->viss;
	int ret;

	PRT_TRACE_BEGIN("viss_isp_preview.\n");

	PRT_DBG("pid=%d,state=0x%lx", task_pid_nr(current), vv->state);

	mutex_lock(&viss->lock);

	if (v4l2_fh_is_singular_file(file)) {
		if (vv->streaming) {
			media_entity_pipeline_stop(&vv->vfd.entity);
			viss_stop_preview(viss, vv, false);
			vv->streaming = false;
		}
		if ((vv->path == 0 && !viss_preview_busy(&viss->vv[1])) ||
		    (vv->path == 1 && !viss_preview_busy(&viss->vv[0])))
			viss_pipeline_call(viss, close, &viss->pipeline);
		clear_bit(ST_PREV_BUSY, &vv->state);
		clear_bit(ST_PREV_SUSPENDED, &vv->state);
	}

	pm_runtime_put(&viss->pdev->dev);

	if (v4l2_fh_is_singular_file(file))
		viss_ctrls_delete(vv->ctx);

	ret = vb2_fop_release(file);
	mutex_unlock(&viss->lock);
	PRT_TRACE_END("\n");

	return ret;
}

static const struct v4l2_file_operations viss_preview_fops = {
	.owner		= THIS_MODULE,
	.open		= viss_preview_open,
	.release	= viss_preview_release,
	.poll		= vb2_fop_poll,
	.unlocked_ioctl	= video_ioctl2,
	.mmap		= vb2_fop_mmap,
};

/*
 * Format and crop negotiation helpers
 */

static struct viss_fmt *viss_preview_try_format(struct viss_ctx *ctx,
						u32 *width, u32 *height,
						u32 *code, u32 *fourcc)
{
	u32 mask = FMT_FLAGS_CAM;
	struct viss_fmt *ffmt;

	PRT_TRACE_BEGIN("\n");

	PRT_DBG("try to set request width:%d, height:%d\n", *width, *height);

	if ((fourcc && *fourcc != V4L2_PIX_FMT_JPEG)) {
		PRT_DBG("\n");
		mask |= FMT_FLAGS_M2M;
	}

	ffmt = viss_find_format(fourcc, code, mask, 0);
	if (WARN_ON(!ffmt))
		return NULL;
	PRT_DBG("find format: ffmt->name:%s\n", ffmt->name);

	if (code) {
		PRT_DBG("ffmt->mbus_code:0x%x\n", ffmt->mbus_code);
		*code = ffmt->mbus_code;
	}

	if (fourcc) {
		PRT_DBG("ffmt->fourcc:0x%x\n", ffmt->fourcc);
		*fourcc = ffmt->fourcc;
	}

	v4l_bound_align_image(width, VISS_ISP_SINK_WIDTH_MIN,
		VISS_ISP_SINK_WIDTH_MAX, 0,
		height, VISS_ISP_SINK_HEIGHT_MIN,
		VISS_ISP_SINK_HEIGHT_MAX, 0, 0);

	PRT_DBG("code: 0x%x, target try set format: %dx%d.\n",
		code ? *code : 0, *width, *height);
	PRT_TRACE_END("\n");

	return ffmt;
}

/*
 * The video node ioctl operations
 */
static int viss_prev_querycap(struct file *file, void *priv,
					struct v4l2_capability *cap)
{
	struct viss_video *vv = video_drvdata(file);
	struct viss_is_dev *viss = vv->ctx->viss;
	struct device *dev = &viss->pdev->dev;

	PRT_TRACE_BEGIN("\n");

	strlcpy(cap->driver, dev->driver->name, sizeof(cap->driver));
	strlcpy(cap->card, dev->driver->name, sizeof(cap->card));
	snprintf(cap->bus_info, sizeof(cap->bus_info),
				"platform:%s", dev_name(dev));
	cap->device_caps = V4L2_CAP_STREAMING |	V4L2_CAP_VIDEO_CAPTURE_MPLANE;
	cap->capabilities = cap->device_caps | V4L2_CAP_DEVICE_CAPS;

	PRT_TRACE_END("\n");
	return 0;
}

static int viss_prev_enum_fmt_mplane(struct file *file, void *priv,
				    struct v4l2_fmtdesc *f)
{
	struct viss_fmt *fmt;

	PRT_TRACE_BEGIN("\n");

	fmt = viss_find_format(NULL, NULL, FMT_FLAGS_CAM | FMT_FLAGS_M2M,
			       f->index);
	if (!fmt)
		return -EINVAL;
	strncpy(f->description, fmt->name, sizeof(f->description) - 1);
	f->pixelformat = fmt->fourcc;
	if (fmt->fourcc == V4L2_MBUS_FMT_JPEG_1X8)
		f->flags |= V4L2_FMT_FLAG_COMPRESSED;
	PRT_TRACE_END("\n");
	return 0;
}

/**
 * viss_pipeline_try_format - negotiate and/or set formats at pipeline
 *                            elements
 * @vv: VISS preview context
 * @tfmt: media bus format to try/set on subdevs
 * @fmt_id: viss pixel format id corresponding to returned @tfmt (output)
 * @set: true to set format on subdevs, false to try only
 */
static int viss_pipeline_try_format(struct viss_video *vv,
				    struct v4l2_mbus_framefmt *tfmt,
				    struct viss_fmt **fmt_id,
				    bool set)
{
	struct viss_ctx *ctx = vv->ctx;
	struct viss_is_dev *viss = ctx->viss;
	struct v4l2_subdev *sd = viss->pipeline.subdevs[IDX_SENSOR];
	struct v4l2_subdev_format sfmt;
	struct v4l2_mbus_framefmt *mf = &sfmt.format;
	struct media_entity *me;
	struct viss_fmt *ffmt;
	struct media_pad *pad;
	int ret, i = 1;
	u32 fcc;

	PRT_TRACE_BEGIN("\n");
	if (vv->path == 1 && viss->vv[0].streaming) {
		/* path 0 and 1 have same source */
		vv->ctx->s_frame = viss->vv[0].ctx->s_frame;

		/* sub path can output fullsize */
		if (vv->ctx->s_frame.width == tfmt->width &&
		    vv->ctx->s_frame.height == tfmt->height)
			return 0;

		/* sub path can output scale down size <= 720p */
		v4l_bound_align_image(
			&tfmt->width, 0, VISS_ISP_SUBPATH_WIDTH_MAX, 0,
			&tfmt->height, 0, vv->ctx->s_frame.height, 0, 0);
		return 0;
	} else if (vv->path == 0 && viss->vv[1].streaming) {
		vv->ctx->s_frame = viss->vv[1].ctx->s_frame;
		v4l_bound_align_image(
			&tfmt->width, 0, vv->ctx->s_frame.width, 0,
			&tfmt->height, 0, vv->ctx->s_frame.height, 0, 0);
		return 0;
	}

	if (WARN_ON(!sd || !tfmt))
		return -EINVAL;

	memset(&sfmt, 0, sizeof(sfmt));
	sfmt.format = *tfmt;
	sfmt.which = set ? V4L2_SUBDEV_FORMAT_ACTIVE : V4L2_SUBDEV_FORMAT_TRY;

	PRT_DBG("sfmt.which:%d, code:0x%x, width:%d, height:%d.\n", sfmt.which,
		sfmt.format.code, sfmt.format.width, sfmt.format.height);

	me = viss_pipeline_get_head(&sd->entity);

	while (1) {
		ffmt = viss_find_format(NULL, mf->code != 0 ? &mf->code : NULL,
					FMT_FLAGS_CAM, i++);
		if (ffmt == NULL) {
			/*
			 * Notify user-space if common pixel code for
			 * host and sensor does not exist.
			 */
			PRT_ERR("\n");
			return -EINVAL;
		}
		mf->code = tfmt->code = ffmt->mbus_code;

		/* set format on all pipeline subdevs */
		while (me != &vv->vfd.entity) {
			sd = media_entity_to_v4l2_subdev(me);

			PRT_DBG("sd->entity->name:%s\n", sd->entity.name);
			sfmt.pad = 0;
			ret = v4l2_subdev_call(sd, pad, set_fmt, NULL, &sfmt);
			if (ret) {
				PRT_ERR("\n");
				return ret;
			}
			PRT_DBG("name:%s, pads=%d, in: %ux%u, out: %ux%u.\n",
				me->name, me->num_pads, sfmt.format.width,
				sfmt.format.height, tfmt->width, tfmt->height);

			if (me->pads[0].flags & MEDIA_PAD_FL_SINK) {
				sfmt.pad = me->num_pads - 1;
				mf->code = tfmt->code;
				if (sd->grp_id == GRP_ID_VISS_IS) {
					sfmt.format = *tfmt;
					sfmt.pad = vv->path + 1;
					/*
					VISS_ISP_SD_PAD_SRC_P0
					VISS_ISP_SD_PAD_SRC_P1
					*/
				}
				ret = v4l2_subdev_call(sd, pad, set_fmt, NULL,
									&sfmt);
				if (ret) {
					PRT_ERR("\n");
					return ret;
				}
			} else {
				/* get resolution from sensor */
				set_frame_crop(&ctx->s_frame, 0, 0,
					sfmt.format.width,
					sfmt.format.height);
			}

			pad = media_entity_remote_source(&me->pads[sfmt.pad]);
			if (!pad) {
				PRT_ERR("\n");
				return -EINVAL;
			}
			me = pad->entity;
		}

		if (mf->code != tfmt->code) {
			PRT_DBG("\n");
			continue;
		}

		fcc = ffmt->fourcc;
		tfmt->width  = mf->width;
		tfmt->height = mf->height;

		ffmt = viss_preview_try_format(ctx, &tfmt->width, &tfmt->height,
					NULL, &fcc);
		if (ffmt && ffmt->mbus_code)
			mf->code = ffmt->mbus_code;

		if (mf->width != tfmt->width || mf->height != tfmt->height) {
			PRT_DBG("continue..........\n");
			continue;
		}
		tfmt->code = mf->code;
		break;
	}

	if (fmt_id && ffmt)
		*fmt_id = ffmt;
	*tfmt = *mf;
	PRT_TRACE_END("\n");

	return 0;
}

static int viss_prev_g_fmt_mplane(struct file *file, void *fh,
				 struct v4l2_format *f)
{
	struct viss_video *vv = video_drvdata(file);
	struct viss_is_frame *out = &vv->ctx->d_frame;
	struct v4l2_pix_format_mplane *pix = &f->fmt.pix_mp;
	int i;

	PRT_TRACE_BEGIN("\n");
	pix->width = out->width;
	pix->height = out->height;
	pix->field = V4L2_FIELD_NONE;
	pix->pixelformat = out->fmt->fourcc;
	pix->colorspace = V4L2_COLORSPACE_JPEG;
	pix->num_planes = out->fmt->memplanes;

	for (i = 0; i < pix->num_planes; ++i) {
		pix->plane_fmt[i].bytesperline = out->bytesperline[i];
		pix->plane_fmt[i].sizeimage = out->payload[i];
	}

	PRT_TRACE_END("\n");
	return 0;
}

static int viss_prev_try_fmt_mplane(struct file *file, void *fh,
				   struct v4l2_format *f)
{
	struct viss_video *vv = video_drvdata(file);
	struct viss_is_dev *viss = vv->ctx->viss;
	struct viss_ctx *ctx = vv->ctx;
	struct v4l2_pix_format_mplane *pix = &f->fmt.pix_mp;
	struct v4l2_mbus_framefmt mf;
	struct viss_fmt *ffmt = NULL;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");

	mutex_lock(&vv->vfd.entity.parent->graph_mutex);
	mutex_lock(&viss->lock);

	PRT_DBG("before try, width:%d, height:%d\n", pix->width, pix->height);

	ffmt = viss_preview_try_format(ctx, &pix->width, &pix->height,
				       NULL, &pix->pixelformat);
	if (!ffmt) {
		ret = -EINVAL;
		goto unlock;
	}

	mf.width = pix->width;
	mf.height = pix->height;
	mf.code = ffmt->mbus_code;
	viss_pipeline_try_format(vv, &mf, &ffmt, false);
	if (ffmt)
		pix->pixelformat = ffmt->fourcc;

	PRT_DBG("after try, width:%d, height:%d\n", pix->width, pix->height);

	viss_adjust_format(ffmt, mf.width, mf.height, pix);

unlock:
	mutex_unlock(&viss->lock);
	mutex_unlock(&vv->vfd.entity.parent->graph_mutex);
	PRT_TRACE_END("\n");

	return ret;
}

static int viss_prev_s_fmt_mplane(struct file *file, void *priv,
				 struct v4l2_format *f)
{
	struct viss_video *vv = video_drvdata(file);
	struct viss_is_dev *viss = vv->ctx->viss;
	struct viss_ctx *ctx = vv->ctx;
	struct v4l2_pix_format_mplane *pix = &f->fmt.pix_mp;
	struct v4l2_mbus_framefmt mf;
	struct viss_is_frame *out = &ctx->d_frame;
	struct viss_fmt *s_fmt = NULL;
	int ret, i;

	PRT_TRACE_BEGIN("\n");

	mutex_lock(&vv->vfd.entity.parent->graph_mutex);
	mutex_lock(&viss->lock);
	/*
	 * The graph is walked within viss_pipeline_try_format() to set
	 * the format at subdevs thus the graph mutex needs to be held at
	 * this point and acquired before the video mutex, to avoid  AB-BA
	 * deadlock when viss_md_link_notify() is called by other thread.
	 * Ideally the graph walking and setting format at the whole pipeline
	 * should be removed from this driver and handled in userspace only.
	 */
	if (vb2_is_busy(&vv->vbq)) {
		ret = -EBUSY;
		goto unlock;
	}

	PRT_DBG("before try, width:%d, height:%d\n", pix->width, pix->height);

	/* Try the format at the scaler and the DMA output */
	out->fmt = viss_preview_try_format(ctx, &pix->width, &pix->height,
					  NULL, &pix->pixelformat);
	if (!out->fmt) {
		ret = -EINVAL;
		goto unlock;
	}

	/* Try to match format at the host and the sensor */
	mf.code   = out->fmt->mbus_code;
	mf.width  = pix->width;
	mf.height = pix->height;
	ret = viss_pipeline_try_format(vv, &mf, &s_fmt, true);
	if (ret)
		goto unlock;

	ctx->s_frame.fmt = s_fmt;

	viss_adjust_format(out->fmt, mf.width, mf.height, pix);

	for (i = 0; i < out->fmt->memplanes; i++) {
		out->bytesperline[i] = pix->plane_fmt[i].bytesperline;
		out->payload[i] = pix->plane_fmt[i].sizeimage;
	}

	PRT_DBG("after try, width:%d, height:%d\n", pix->width, pix->height);

	/* set output resolution */
	set_frame_crop(out, 0, 0, pix->width, pix->height);

unlock:
	mutex_unlock(&viss->lock);
	mutex_unlock(&vv->vfd.entity.parent->graph_mutex);

	PRT_TRACE_END("\n");
	return ret;
}

static int viss_prev_enum_input(struct file *file, void *priv,
			       struct v4l2_input *i)
{
	struct viss_video *vv = video_drvdata(file);
	struct viss_is_dev *viss = vv->ctx->viss;
	struct v4l2_subdev *sd = viss->pipeline.subdevs[IDX_SENSOR];

	PRT_TRACE_BEGIN("\n");

	if (i->index != 0)
		return -EINVAL;

	i->type = V4L2_INPUT_TYPE_CAMERA;
	if (sd)
		strlcpy(i->name, sd->name, sizeof(i->name));

	PRT_TRACE_END("\n");
	return 0;
}

static int viss_prev_s_input(struct file *file, void *priv, unsigned int i)
{
	return i == 0 ? i : -EINVAL;
}

static int viss_prev_g_input(struct file *file, void *priv, unsigned int *i)
{
	*i = 0;
	return 0;
}

static int viss_prev_enum_framesizes(struct file *file, void *fh,
				struct v4l2_frmsizeenum *fsize)
{
	struct viss_video *vv = video_drvdata(file);
	struct viss_is_dev *viss = vv->ctx->viss;
	struct v4l2_subdev *sd = viss->pipeline.subdevs[IDX_SENSOR];

	PRT_DBG("sd->name=%s\n", sd->name);
	return v4l2_subdev_call(sd, video, enum_framesizes, fsize);
}

static int viss_prev_enum_frameintervals(struct file *file, void *fh,
				struct v4l2_frmivalenum *fival)
{
	struct viss_video *vv = video_drvdata(file);
	struct viss_is_dev *viss = vv->ctx->viss;
	struct v4l2_subdev *sd = viss->pipeline.subdevs[IDX_SENSOR];

	PRT_DBG("sd->name=%s\n", sd->name);
	return v4l2_subdev_call(sd, video, enum_frameintervals, fival);
}

static int viss_prev_g_parm(struct file *file, void *fh,
			struct v4l2_streamparm *a)
{
	struct viss_video *vv = video_drvdata(file);
	struct viss_is_dev *viss = vv->ctx->viss;
	struct v4l2_subdev *sd = viss->pipeline.subdevs[IDX_SENSOR];

	PRT_DBG("sd->name=%s\n", sd->name);
	return v4l2_subdev_call(sd, video, g_parm, a);
}

static int viss_prev_s_parm(struct file *file, void *fh,
			struct v4l2_streamparm *a)
{
	struct viss_video *vv = video_drvdata(file);
	struct viss_is_dev *viss = vv->ctx->viss;
	struct v4l2_subdev *sd = viss->pipeline.subdevs[IDX_SENSOR];

	PRT_DBG("sd->name=%s\n", sd->name);
	return v4l2_subdev_call(sd, video, s_parm, a);
}

static int viss_prev_streamon(struct file *file, void *priv,
			     enum v4l2_buf_type type)
{
	struct viss_video *vv = video_drvdata(file);
	struct viss_is_dev *viss = vv->ctx->viss;
	struct viss_pipeline *p = &viss->pipeline;
	int ret;

	PRT_TRACE_BEGIN("isp.\n");
	if (viss_preview_active(viss, vv)) {
		PRT_ERR("isp path%d is busy!\n", vv->path);
		return -EBUSY;
	}

	ret = media_entity_pipeline_start(&vv->vfd.entity, p->m_pipeline);
	if (ret < 0)
		return ret;

	ret = vb2_ioctl_streamon(file, priv, type);
	if (!ret) {
		PRT_TRACE_END("isp.\n");
		vv->streaming = true;
		return ret;
	}

	media_entity_pipeline_stop(&vv->vfd.entity);
	return ret;
}

static int viss_prev_streamoff(struct file *file, void *priv,
			    enum v4l2_buf_type type)
{
	struct viss_video *vv = video_drvdata(file);
	int ret;

	PRT_TRACE_BEGIN("\n");

	ret = vb2_ioctl_streamoff(file, priv, type);
	if (ret < 0)
		return ret;

	media_entity_pipeline_stop(&vv->vfd.entity);
	vv->streaming = false;

	PRT_TRACE_END("\n");
	return 0;
}

static int viss_prev_reqbufs(struct file *file, void *priv,
			    struct v4l2_requestbuffers *reqbufs)
{
	struct viss_video *vv = video_drvdata(file);
	int ret;

	PRT_TRACE_BEGIN("\n");

	ret = vb2_ioctl_reqbufs(file, priv, reqbufs);
	if (!ret)
		vv->reqbufs_count = reqbufs->count;
	PRT_TRACE_END("\n");

	return ret;
}

static int viss_prev_g_selection(struct file *file, void *fh,
				struct v4l2_selection *s)
{
	struct viss_video *vv = video_drvdata(file);
	struct viss_ctx *ctx = vv->ctx;
	struct viss_is_frame *f = &ctx->s_frame;

	PRT_TRACE_BEGIN("\n");

	if (s->type != V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE)
		return -EINVAL;

	switch (s->target) {
	case V4L2_SEL_TGT_COMPOSE_DEFAULT:
	case V4L2_SEL_TGT_COMPOSE_BOUNDS:
		f = &ctx->d_frame;
	case V4L2_SEL_TGT_CROP_BOUNDS:
	case V4L2_SEL_TGT_CROP_DEFAULT:
		s->r.left = 0;
		s->r.top = 0;
		s->r.width = f->width;
		s->r.height = f->height;
		return 0;
	case V4L2_SEL_TGT_COMPOSE:
		f = &ctx->d_frame;
	case V4L2_SEL_TGT_CROP:
		s->r.left = f->offs_h;
		s->r.top = f->offs_v;
		s->r.width = f->width;
		s->r.height = f->height;
		return 0;
	}
	PRT_TRACE_END("\n");

	return -EINVAL;
}

/* Return 1 if rectangle a is enclosed in rectangle b, or 0 otherwise. */
static int enclosed_rectangle(struct v4l2_rect *a, struct v4l2_rect *b)
{
	if (a->left < b->left || a->top < b->top)
		return 0;
	if (a->left + a->width > b->left + b->width)
		return 0;
	if (a->top + a->height > b->top + b->height)
		return 0;

	return 1;
}

static int viss_prev_s_selection(struct file *file, void *fh,
				struct v4l2_selection *s)
{
	struct viss_video *vv = video_drvdata(file);
	struct viss_is_dev *viss = vv->ctx->viss;
	struct viss_ctx *ctx = vv->ctx;
	struct v4l2_rect rect = s->r;
	struct viss_is_frame *f;
	unsigned long flags;

	if (s->type != V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE)
		return -EINVAL;

	if (s->target == V4L2_SEL_TGT_COMPOSE)
		f = &ctx->d_frame;
	else if (s->target == V4L2_SEL_TGT_CROP)
		f = &ctx->s_frame;
	else
		return -EINVAL;

	if (s->flags & V4L2_SEL_FLAG_LE &&
	    !enclosed_rectangle(&rect, &s->r))
		return -ERANGE;

	if (s->flags & V4L2_SEL_FLAG_GE &&
	    !enclosed_rectangle(&s->r, &rect))
		return -ERANGE;

	s->r = rect;
	spin_lock_irqsave(&viss->slock, flags);
	set_frame_crop(f, s->r.left, s->r.top, s->r.width, s->r.height);
	spin_unlock_irqrestore(&viss->slock, flags);
	return 0;
}

static int viss_prev_get_isp_dev(struct file *file, void *fh,
				int *arg)
{
	struct viss_video *vv = video_drvdata(file);
	struct viss_is_dev *viss = vv->ctx->viss;
	struct v4l2_subdev *sd = viss->pipeline.subdevs[IDX_IS_ISP];

	PRT_TRACE_BEGIN("\n");

	if (WARN_ON(!sd)) {
		*arg = -1;
		return -EINVAL;
	}

	*arg = sd->devnode->num;

	PRT_INFO("%s: subdev num %d\n", sd->name, sd->devnode->num);
	PRT_TRACE_END("\n");

	return 0;
}

static int viss_prev_get_sensor_dev(struct file *file, void *fh,
				int *arg)
{
	struct viss_video *vv = video_drvdata(file);
	struct viss_is_dev *viss = vv->ctx->viss;
	struct v4l2_subdev *sd = viss->pipeline.subdevs[IDX_SENSOR];

	PRT_TRACE_BEGIN("\n");

	if (WARN_ON(!sd)) {
		*arg = -1;
		return -EINVAL;
	}
	*arg = sd->devnode->num;

	PRT_INFO("%s: subdev num %d\n", sd->name, sd->devnode->num);
	PRT_TRACE_END("\n");

	return 0;
}

static long viss_prev_default(struct file *file, void *fh,
		bool valid_prio, unsigned int cmd, void *arg)
{
	int ret = 0;

	switch (cmd) {
	case VIDIOC_VISS_GET_ISP_DEV:
		ret = viss_prev_get_isp_dev(file, fh, arg);
		break;
	case VIDIOC_VISS_GET_SENSOR_DEV:
		ret = viss_prev_get_sensor_dev(file, fh, arg);
		break;
	default:
		return -ENOIOCTLCMD;
	}

	return ret;
}

static const struct v4l2_ioctl_ops viss_preview_ioctl_ops = {
	.vidioc_querycap		= viss_prev_querycap,

	.vidioc_enum_fmt_vid_cap_mplane	= viss_prev_enum_fmt_mplane,
	.vidioc_try_fmt_vid_cap_mplane	= viss_prev_try_fmt_mplane,
	.vidioc_s_fmt_vid_cap_mplane	= viss_prev_s_fmt_mplane,
	.vidioc_g_fmt_vid_cap_mplane	= viss_prev_g_fmt_mplane,

	.vidioc_enum_framesizes		= viss_prev_enum_framesizes,
	.vidioc_enum_frameintervals	= viss_prev_enum_frameintervals,

	.vidioc_g_parm			= viss_prev_g_parm,
	.vidioc_s_parm			= viss_prev_s_parm,

	.vidioc_reqbufs			= viss_prev_reqbufs,
	.vidioc_querybuf		= vb2_ioctl_querybuf,
	.vidioc_qbuf			= vb2_ioctl_qbuf,
	.vidioc_dqbuf			= vb2_ioctl_dqbuf,
	.vidioc_expbuf			= vb2_ioctl_expbuf,
	.vidioc_prepare_buf		= vb2_ioctl_prepare_buf,
	.vidioc_create_bufs		= vb2_ioctl_create_bufs,

	.vidioc_streamon		= viss_prev_streamon,
	.vidioc_streamoff		= viss_prev_streamoff,

	.vidioc_g_selection		= viss_prev_g_selection,
	.vidioc_s_selection		= viss_prev_s_selection,

	.vidioc_enum_input		= viss_prev_enum_input,
	.vidioc_s_input			= viss_prev_s_input,
	.vidioc_g_input			= viss_prev_g_input,

	.vidioc_default			= viss_prev_default,
};

/* viss->lock must be already initialized */
int __register_video_device(struct viss_is_dev *viss,
		struct viss_video *vv, struct v4l2_device *v4l2_dev)
{
	struct video_device *vfd = &vv->vfd;
	struct vb2_queue *q = &vv->vbq;
	struct viss_ctx *ctx;
	int ret = -ENOMEM;

	PRT_TRACE_BEGIN("\n");
	ctx = kzalloc(sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->viss	 = viss;
	ctx->in_path	 = VISS_IO_CAMERA;
	ctx->out_path	 = VISS_IO_DMA;
	ctx->s_frame.fmt = viss_find_format(NULL, NULL, FMT_FLAGS_CAM, 0);
	PRT_DBG("ctx->s_frame.fmt->name:%s\n", ctx->s_frame.fmt->name);
	ctx->d_frame.fmt = ctx->s_frame.fmt;

	memset(vfd, 0, sizeof(*vfd));
	snprintf(vfd->name, sizeof(vfd->name), "viss-isp%d.path%d",
			viss->id, vv->path);

	vfd->fops	= &viss_preview_fops;
	vfd->ioctl_ops	= &viss_preview_ioctl_ops;
	vfd->v4l2_dev	= v4l2_dev;
	vfd->minor	= -1;
	vfd->release	= video_device_release_empty;
	vfd->queue	= q;
	vfd->lock	= &viss->lock;

	video_set_drvdata(vfd, vv);
	vv->reqbufs_count = 0;
	vv->ctx = ctx;

	INIT_LIST_HEAD(&vv->pending_buf_q);
	INIT_LIST_HEAD(&vv->active_buf_q);

	memset(q, 0, sizeof(*q));
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF;
	q->drv_priv = vv;
	q->ops = &viss_preview_qops;
	q->mem_ops = &vb2_dma_contig_memops;
	q->buf_struct_size = sizeof(struct viss_vid_buffer);
	q->timestamp_type = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &viss->lock;

	ret = vb2_queue_init(q);
	if (ret)
		goto err_ent;

	vv->vd_pad.flags = MEDIA_PAD_FL_SINK;
	ret = media_entity_init(&vfd->entity, 1, &vv->vd_pad, 0);
	if (ret)
		goto err_ent;
	/*
	 * For proper order of acquiring/releasing the video
	 * and the graph mutex.
	 */
	v4l2_disable_ioctl_locking(vfd, VIDIOC_TRY_FMT);
	v4l2_disable_ioctl_locking(vfd, VIDIOC_S_FMT);

	ret = video_register_device(vfd, VFL_TYPE_GRABBER, vv->nr);
	if (ret)
		goto err_vd;

	PRT_INFO("Registered %s as /dev/%s\n",
			vfd->name, video_device_node_name(vfd));

	vfd->ctrl_handler = &ctx->ctrls.handler;
	PRT_TRACE_END("\n");
	return 0;

err_vd:
	media_entity_cleanup(&vfd->entity);
err_ent:
	kfree(ctx);
	return ret;
}

int viss_register_video_device(struct viss_is_dev *viss,
		struct v4l2_device *v4l2_dev)
{
	int ret = 0;

	viss->vv[0].path = 0;
	viss->vv[1].path = 1;
	ret = __register_video_device(viss, &viss->vv[0], v4l2_dev);
	if (ret)
		return ret;
	return __register_video_device(viss, &viss->vv[1], v4l2_dev);
}

void viss_unregister_video_device(struct viss_is_dev *viss)
{
	if (viss == NULL)
		return;

	PRT_TRACE_BEGIN("\n");
	if (video_is_registered(&viss->vv[0].vfd)) {
		video_unregister_device(&viss->vv[0].vfd);
		media_entity_cleanup(&viss->vv[0].vfd.entity);
		viss->pipeline_ops = NULL;
	}
	kfree(viss->vv[0].ctx);
	viss->vv[0].ctx = NULL;

	if (video_is_registered(&viss->vv[1].vfd)) {
		video_unregister_device(&viss->vv[1].vfd);
		media_entity_cleanup(&viss->vv[1].vfd.entity);
		viss->pipeline_ops = NULL;
	}
	kfree(viss->vv[1].ctx);
	viss->vv[1].ctx = NULL;
	PRT_TRACE_END("\n");
}

void viss_set_second_buf(struct viss_is_dev *viss,
		struct viss_video *vv)
{
	if (vv->first_frame) {
		vv->first_frame = false;
		/* if not pre set second frame address, then set in first irq */
		viss_set_next_buf(viss, vv);
	}
}

void viss_preview_irq_handler(struct viss_is_dev *viss,
		struct viss_video *vv)
{
	struct viss_vid_buffer *vbuf;
	struct timeval *tv;
	struct timespec ts;
	struct viss_isp *isp = NULL;

	PRT_TRACE_BEGIN("\n");

	if (test_and_clear_bit(ST_PREV_SHUT, &vv->state)) {
		wake_up(&viss->irq_queue);
		goto done;
	}

	if (!list_empty(&vv->active_buf_q) &&
	    test_bit(ST_PREV_RUN, &vv->state) &&
	    !list_is_singular(&vv->active_buf_q)) {

		if (isp_offline)
			ts = isp_offline_ts;
		else
			ktime_get_real_ts(&ts);

		/* send current buffer */
		vbuf = viss_active_queue_pop(vv);
		tv = &vbuf->vb.v4l2_buf.timestamp;
		tv->tv_sec = ts.tv_sec;
		tv->tv_usec = ts.tv_nsec / NSEC_PER_USEC;
		vbuf->vb.v4l2_buf.sequence = vv->frame_count++;

		/* save frame address for isp tool, use main path first */
		isp = v4l2_get_subdevdata(viss->pipeline.subdevs[IDX_IS_ISP]);
		if (isp && ((vv->path == 0) ||
		   (vv->path == 1 && !viss->vv[0].streaming))) {
			isp->save_yuv[0].paddr = vbuf->paddr.y;
			isp->save_yuv[1].paddr = vbuf->paddr.cb;
			isp->save_yuv[2].paddr = vbuf->paddr.cr;
			isp->save_yuv[0].size = vbuf->vb.v4l2_planes[0].length;
			isp->save_yuv[1].size = vbuf->vb.v4l2_planes[1].length;
			isp->save_yuv[2].size = vbuf->vb.v4l2_planes[2].length;
			isp->frame_time = (ts.tv_sec*NSEC_PER_SEC + ts.tv_nsec -
				isp->ts.tv_sec*NSEC_PER_SEC - isp->ts.tv_nsec);
			isp->ts = ts;
		}
		vb2_buffer_done(&vbuf->vb, VB2_BUF_STATE_DONE);

		/* set next buffer */
		viss_set_next_buf(viss, vv);
	}

	viss_set_second_buf(viss, vv);

	if (list_is_singular(&vv->active_buf_q)) {
		/* will set next buffer in buf_queue when usr QBUF */
		vv->lost_count++;
		PRT_DBG("frame lost\n");
	}
done:
	set_bit(ST_PREV_RUN, &vv->state);
	PRT_TRACE_END("\n");
}

int viss_preview_suspend(struct viss_is_dev *viss,
			struct viss_video *vv)
{
	int ret = 0;

	PRT_DBG("viss%d.path%d: state=0x%lx", viss->id, vv->path, vv->state);
	if (!viss_preview_busy(vv))
		return 0;
	ret = viss_stop_preview(viss, vv, true);
	if (ret)
		return ret;
	vv->streaming = false;

	return 0;
}

int viss_preview_resume(struct viss_is_dev *viss,
			struct viss_video *vv)

{
	PRT_DBG("viss%d.path%d: state=0x%lx", viss->id, vv->path, vv->state);
	if (!viss_preview_busy(vv))
		return 0;
	if (!test_bit(ST_PREV_SUSPENDED, &vv->state))
		return 0;

	vv->lost_count = 0;
	vv->frame_count = 0;
	vv->first_frame = true;

	INIT_LIST_HEAD(&vv->active_buf_q);

	if (vb2_is_streaming(&vv->vbq)) {
		viss_start_preview(viss, vv);
		vv->streaming = true;
	}
	clear_bit(ST_PREV_SUSPENDED, &vv->state);
	return 0;
}

static irqreturn_t viss_irq_handler(int irq, void *priv)
{
	struct viss_is_dev *viss = priv;
	struct viss_video *vv = &viss->vv[0];
	unsigned int fifo_full;
	unsigned int status;
	unsigned long flags;

	PRT_TRACE_BEGIN("\n");

	if (isp_offline) {
		if (isp_offline_id < 0) {
			PRT_ERR("offline fifo is empty!\n");
			csp_isp_disable_dma_int(viss->regs);
			return IRQ_HANDLED;
		}
		viss = glb_viss[isp_offline_id];
		vv = &viss->vv[0];
	}

	spin_lock_irqsave(&viss->slock, flags);

	fifo_full = csp_isp_get_dma_status(viss->regs);
	csp_isp_clr_dma_status(viss->regs, fifo_full);

	if (fifo_full & ISP_MEM_STATUS_MP_Y_FIFO_FULL)
		PRT_DBG("ISP warning MP Y fifo full!\n");
	if (fifo_full & ISP_MEM_STATUS_MP_CB_FIFO_FULL)
		PRT_DBG("ISP warning MP CB fifo full!\n");
	if (fifo_full & ISP_MEM_STATUS_MP_CR_FIFO_FULL)
		PRT_DBG("ISP warning MP CR fifo full!\n");
	if (fifo_full & ISP_MEM_STATUS_SP_Y_FIFO_FULL)
		PRT_DBG("ISP warning SP Y fifo full!\n");
	if (fifo_full & ISP_MEM_STATUS_SP_CB_FIFO_FULL)
		PRT_DBG("ISP warning SP CB fifo full!\n");
	if (fifo_full & ISP_MEM_STATUS_SP_CR_FIFO_FULL)
		PRT_DBG("ISP warning SP CR fifo full!\n");

	status = csp_isp_get_dma_int_status(viss->regs);
	csp_isp_clr_dma_int(viss->regs, status & ~0x3);

	if (status & ISP_MEM_STATUS_MP_FRAME_END) {
		csp_isp_clr_dma_int(viss->regs,	ISP_MEM_STATUS_MP_FRAME_END);
		PRT_DBG("main path interrupt.\n");
		vv = &viss->vv[0];
		if (fifo_full)
			clear_bit(ST_PREV_RUN, &vv->state);
		else
			set_bit(ST_PREV_RUN, &vv->state);
		if (test_bit(ST_PREV_STREAM, &vv->state))
			viss_preview_irq_handler(viss, vv);
	}

	if (status & ISP_MEM_STATUS_SP_FRAME_END) {
		csp_isp_clr_dma_int(viss->regs,	ISP_MEM_STATUS_SP_FRAME_END);
		PRT_DBG("sub path interrupt.\n");
		vv = &viss->vv[1];
		if (fifo_full)
			clear_bit(ST_PREV_RUN, &vv->state);
		else
			set_bit(ST_PREV_RUN, &vv->state);
		if (test_bit(ST_PREV_STREAM, &vv->state))
			viss_preview_irq_handler(viss, vv);
	}

	offline_fifo_pop();
	offline_fifo_process();

	spin_unlock_irqrestore(&viss->slock, flags);
	PRT_TRACE_END("\n");
	return IRQ_HANDLED;
}

static int viss_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct viss_is_dev *viss;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");
	viss = devm_kzalloc(dev, sizeof(*viss), GFP_KERNEL);
	if (!viss) {
		PRT_ERR("create struct viss_is failed!\n");
		return -ENOMEM;
	}

	viss->pdev = pdev;
	viss->id = of_alias_get_id(dev->of_node, "viss");

	viss->vv[MAIN_PATH].nr = of_alias_get_id(dev->of_node, "video");
	viss->vv[SUB_PATH].nr = (viss->vv[MAIN_PATH].nr) + 1;

	init_waitqueue_head(&viss->irq_queue);
	spin_lock_init(&viss->slock);
	mutex_init(&viss->lock);

	viss->regs = of_iomap(dev->of_node, 0);
	if (!viss->regs) {
		if (viss->id == 0) {
			PRT_ERR("map viss%d reg failed!\n", viss->id);
			return -EINVAL;
		} else {
			viss->regs = glb_viss[0]->regs;
			goto skip_irq;
		}
	}

	viss->irq = irq_of_parse_and_map(dev->of_node, 0);
	if (viss->irq < 0) {
		PRT_ERR("viss%d no irq found\n", viss->id);
		return viss->irq;
	}
	PRT_DBG("viss%d irq %d.\n", viss->id, viss->irq);

	ret = request_irq(viss->irq, viss_irq_handler, 0, dev_name(dev), viss);
	if (ret < 0) {
		PRT_ERR("irq%d request failed\n", viss->irq);
		return -EINVAL;
	}

skip_irq:
	platform_set_drvdata(pdev, viss);
	pm_runtime_enable(dev);
	ret = pm_runtime_get_sync(dev);
	if (ret < 0)
		goto err_irq;
	/* Initialize contiguous memory allocator */
	viss->alloc_ctx = vb2_dma_contig_init_ctx(dev);
	if (IS_ERR(viss->alloc_ctx)) {
		ret = PTR_ERR(viss->alloc_ctx);
		goto err_pm;
	}
	pm_runtime_put(dev);
	glb_viss[viss->id] = viss;
	PRT_TRACE_END("viss.%d registered successfully\n", viss->id);
	return 0;
err_pm:
	pm_runtime_put(dev);
err_irq:
	free_irq(viss->irq, viss);
	return ret;
}

static int viss_remove(struct platform_device *pdev)
{
	struct viss_is_dev *viss = platform_get_drvdata(pdev);

	PRT_TRACE_BEGIN("\n");
	pm_runtime_disable(&pdev->dev);
	pm_runtime_set_suspended(&pdev->dev);

	vb2_dma_contig_cleanup_ctx(viss->alloc_ctx);
	free_irq(viss->irq, viss);
	PRT_TRACE_END("\n");
	return 0;
}

#ifdef CONFIG_PM_RUNTIME
static int viss_runtime_resume(struct device *dev)
{
	int ret = 0;
#if 0
	struct viss_is_dev *viss = dev_get_drvdata(dev);

	PRT_TRACE_BEGIN("\n");
	if (viss_preview_busy(&viss->vv[0]))
		viss_pipeline_call(viss, open, &viss->pipeline,
			&viss->vv[0].vfd.entity, false);
	else
		viss_pipeline_call(viss, open, &viss->pipeline,
			&viss->vv[1].vfd.entity, false);

	ret = viss_preview_resume(viss, &viss->vv[0]);
	if (ret)
		return ret;

	ret = viss_preview_resume(viss, &viss->vv[1]);
	if (ret)
		return ret;
#endif
	PRT_TRACE_END("\n");
	return ret;
}

static int viss_runtime_suspend(struct device *dev)
{
	int ret = 0;
#if 0
	struct viss_is_dev *viss = dev_get_drvdata(dev);

	PRT_TRACE_BEGIN("\n");

	ret = viss_preview_suspend(viss, &viss->vv[0]);
	if (ret)
		return ret;
	ret = viss_preview_suspend(viss, &viss->vv[1]);
	if (ret)
		return ret;
	PRT_TRACE_END("\n");
	if (!viss_preview_busy(&viss->vv[0]) &&
	    !viss_preview_busy(&viss->vv[1]))
		return 0;
	ret =  viss_pipeline_call(viss, close, &viss->pipeline);
#endif
	PRT_TRACE_END("\n");
	return ret;
}
#endif

#ifdef CONFIG_PM_SLEEP
static int viss_resume(struct device *dev)
{
	struct viss_is_dev *viss = dev_get_drvdata(dev);
	unsigned long flags;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");
	/* Do not resume if the device was idle before system suspend */
	spin_lock_irqsave(&viss->slock, flags);
	if (!test_and_clear_bit(ST_LPM, &viss->vv[0].state) &&
	    !test_and_clear_bit(ST_LPM, &viss->vv[1].state)) {
		spin_unlock_irqrestore(&viss->slock, flags);
		return 0;
	}
	spin_unlock_irqrestore(&viss->slock, flags);

	if (viss_preview_busy(&viss->vv[0]))
		viss_pipeline_call(viss, open, &viss->pipeline,
			&viss->vv[0].vfd.entity, false);
	else
		viss_pipeline_call(viss, open, &viss->pipeline,
			&viss->vv[1].vfd.entity, false);

	ret = viss_preview_resume(viss, &viss->vv[0]);
	if (ret)
		return ret;

	ret = viss_preview_resume(viss, &viss->vv[1]);
	if (ret)
		return ret;
	PRT_TRACE_END("\n");
	return ret;
}

static int viss_suspend(struct device *dev)
{
	struct viss_is_dev *viss = dev_get_drvdata(dev);
	int ret = 0;

	PRT_TRACE_BEGIN("\n");
	if (test_and_set_bit(ST_LPM, &viss->vv[0].state) &&
	    test_and_set_bit(ST_LPM, &viss->vv[1].state))
		return 0;

	ret = viss_preview_suspend(viss, &viss->vv[0]);
	if (ret)
		return ret;
	ret = viss_preview_suspend(viss, &viss->vv[1]);
	if (ret)
		return ret;
	PRT_TRACE_END("\n");
	if (!viss_preview_busy(&viss->vv[0]) &&
	    !viss_preview_busy(&viss->vv[1]))
		return 0;
	return viss_pipeline_call(viss, close, &viss->pipeline);
}
#endif /* CONFIG_PM_SLEEP */

static const struct of_device_id viss_of_match[] = {
	{
		.compatible = "lombo,n7-viss",
	},
};

static const struct dev_pm_ops viss_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(viss_suspend, viss_resume)
	SET_RUNTIME_PM_OPS(viss_runtime_suspend, viss_runtime_resume, NULL)
};

static struct platform_driver viss_driver = {
	.probe		= viss_probe,
	.remove		= viss_remove,
	.driver = {
		.of_match_table = viss_of_match,
		.name		= VISS_DRIVER_NAME,
		.owner		= THIS_MODULE,
		.pm		= &viss_pm_ops,
	}
};

module_platform_driver(viss_driver);
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" VISS_DRIVER_NAME);

