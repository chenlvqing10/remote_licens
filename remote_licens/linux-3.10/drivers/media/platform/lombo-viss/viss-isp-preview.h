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

#ifndef VISS_ISP_PREVIEW_H_
#define VISS_ISP_PREVIEW_H_

#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/mfd/syscon.h>
#include <linux/types.h>
#include <linux/videodev2.h>
#include <linux/io.h>
#include <linux/sizes.h>

#include <media/media-entity.h>
#include <media/videobuf2-core.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-mem2mem.h>
#include <media/v4l2-mediabus.h>
#include "viss.h"

#include <mach/debug.h>

/* Time to wait for next frame VSYNC interrupt while stopping operation. */
#define VISS_SHUTDOWN_TIMEOUT	((200*HZ)/1000)
#define VISS_DRIVER_NAME	"viss"

/* for lombo viss */
#define VISS_ISP_NUM		2
#define VISS_MAX_PLANES		3

#define viss_preview_running(dev) test_bit(ST_PREV_RUN, &(dev)->state)
#define viss_preview_busy(dev) test_bit(ST_PREV_BUSY, &(dev)->state)

#define viss_fmt_is_user_defined(x) (!!((x) & 0x180))
#define viss_fmt_is_rgb(x) (!!((x) & 0x10))

enum {
	MAIN_PATH,
	SUB_PATH,
	MAX_PATH,
};

/**
 * struct viss_vid_buffer - the driver's video buffer
 * @vb:    v4l videobuf buffer
 * @list:  linked list structure for buffer queue
 * @paddr: precalculated physical address set
 * @index: buffer index for the output DMA engine
 */
struct viss_vid_buffer {
	struct vb2_buffer	vb;
	struct list_head	list;
	struct viss_addr	paddr;
};

/**
 * struct viss_frame - source/target frame properties
 * @offs_h:	image horizontal pixel offset
 * @offs_v:	image vertical pixel offset
 * @width:	image pixel width
 * @height:	image pixel weight
 * @payload:	image size in bytes (w x h x bpp)
 * @bytesperline: bytesperline value for each plane
 * @paddr:	image frame buffer physical addresses
 * @fmt:	viss color format pointer
 */
struct viss_is_frame {
	u32	offs_h;
	u32	offs_v;
	u32	width;
	u32	height;
	unsigned int		yuv_size[3];
	unsigned int		payload[VIDEO_MAX_PLANES];
	unsigned int		bytesperline[VIDEO_MAX_PLANES];
	struct viss_addr	paddr;
	struct viss_fmt		*fmt;
};

/**
 * struct viss_video - viss video device information
 * @ctx: hardware context data
 * @vfd: video device node for camera preview mode
 * @vd_pad: viss video preview node pad
 * @pending_buf_q: the pending buffer queue head
 * @active_buf_q: the queue head of buffers scheduled in hardware
 * @vbq: the preview am video buffer queue
 * @frame_count: the frame counter for statistics
 * @reqbufs_count: the number of buffers requested in REQBUFS ioctl
 * @user_subdev_api: true if subdevs are not configured by the host driver
 * @nr: video id
 */
struct viss_video {
	struct viss_ctx			*ctx;
	struct video_device		vfd;
	struct media_pad		vd_pad;
	struct list_head		pending_buf_q;
	struct list_head		active_buf_q;
	struct vb2_queue		vbq;
	unsigned long			state;
	unsigned int			frame_count;
	unsigned int			lost_count;
	unsigned int			reqbufs_count;
	int				path;
	bool				streaming;
	bool				first_frame;
	int				nr;
};

struct viss_ctx;

/**
 * struct viss_is_dev - abstraction for VISS entity
 * @slock:	the spinlock protecting this data structure
 * @lock:	the mutex protecting this data structure
 * @pdev:	pointer to the VISS platform device
 * @id:		VISS device index (0..VISS_ISP_NUM)
 * @regs:	the mapped hardware registers
 * @irq_queue:	interrupt handler waitqueue
 * @vid_prev:	viss video info
 * @alloc_ctx:	videobuf2 memory allocator context
 * @pipeline:	viss video preview pipeline data structure
 */
struct viss_is_dev {
	spinlock_t			slock;
	struct mutex			lock;
	struct platform_device		*pdev;
	int				id;
	int				irq;
	int				reg_save[0x3000/4];
	void __iomem			*regs;
	wait_queue_head_t		irq_queue;
	struct viss_video		vv[MAX_PATH];/* two path */
	struct vb2_alloc_ctx		*alloc_ctx;
	struct viss_pipeline		pipeline;
	const struct viss_pipeline_ops	*pipeline_ops;
};

/**
 * struct viss_ctrls - v4l2 controls structure
 * @handler: the control handler
 * @rotate: image rotation control
 * @hflip: horizontal flip control
 * @vflip: vertical flip control
 * @ready: true if @handler is initialized
 */
struct viss_ctrls {
	struct v4l2_ctrl_handler handler;
	struct v4l2_ctrl *rotate;
	struct v4l2_ctrl *hflip;
	struct v4l2_ctrl *vflip;
	bool ready;
};

#define ctrl_to_ctx(ctrl) \
	container_of((ctrl)->handler, struct viss_ctx, ctrls.handler)

/**
 * viss_ctx - the device context data
 * @s_frame:		source frame properties
 * @d_frame:		destination frame properties
 * @in_path:		input mode (DMA or camera)
 * @out_path:		output mode (DMA or FIFO)
 * @rotation:		image clockwise rotation in degrees
 * @hflip:		indicates image horizontal flip if set
 * @vflip:		indicates image vertical flip if set
 * @flags:		additional flags for image conversion
 * @state:		flags to keep track of user configuration
 * @viss:		the VISS device this context applies to
 * @fh:			v4l2 file handle
 * @ctrls:		v4l2 controls structure
 */
struct viss_ctx {
	struct viss_is_frame	s_frame;
	struct viss_is_frame	d_frame;
	enum viss_datapath	in_path;
	enum viss_datapath	out_path;
	int			rotation;
	unsigned int		hflip:1;
	unsigned int		vflip:1;
	u32			flags;
	u32			state;
	struct viss_is_dev	*viss;
	struct v4l2_fh		fh;
	struct viss_ctrls	ctrls;
};

static inline void set_frame_crop(struct viss_is_frame *f,
				  u32 left, u32 top, u32 width, u32 height)
{
	f->offs_h = left;
	f->offs_v = top;
	f->width  = width;
	f->height = height;
}

static inline bool viss_preview_active(struct viss_is_dev *viss,
		struct viss_video *vv)
{
	unsigned long flags;
	bool ret;

	spin_lock_irqsave(&viss->slock, flags);
	ret = !!(vv->state & (1 << ST_PREV_RUN) ||
		 vv->state & (1 << ST_PREV_STREAM));
	spin_unlock_irqrestore(&viss->slock, flags);
	return ret;
}

int viss_register_video_device(struct viss_is_dev *viss,
				 struct v4l2_device *v4l2_dev);
void viss_unregister_video_device(struct viss_is_dev *viss);
void viss_set_second_buf(struct viss_is_dev *viss,
		struct viss_video *vv);

/*
 * Buffer list manipulation functions. Must be called with viss.slock held.
 */

/**
 * viss_active_queue_add - add buffer to the preview active buffers queue
 * @buf: buffer to add to the active buffers list
 */
static inline void viss_active_queue_add(struct viss_video *vid_prev,
					 struct viss_vid_buffer *buf)
{
	list_add_tail(&buf->list, &vid_prev->active_buf_q);
}

/**
 * viss_active_queue_pop - pop buffer from the preview active buffers queue
 *
 * The caller must assure the active_buf_q list is not empty.
 */
static inline struct viss_vid_buffer *viss_active_queue_pop(
				    struct viss_video *vid_prev)
{
	struct viss_vid_buffer *buf;

	buf = list_entry(vid_prev->active_buf_q.next,
			 struct viss_vid_buffer, list);
	list_del(&buf->list);
	return buf;
}

/**
 * viss_pending_queue_add - add buffer to the preview pending buffers queue
 * @buf: buffer to add to the pending buffers list
 */
static inline void viss_pending_queue_add(struct viss_video *vid_prev,
					  struct viss_vid_buffer *buf)
{
	list_add_tail(&buf->list, &vid_prev->pending_buf_q);
}

/**
 * viss_pending_queue_pop - pop buffer from the preview pending buffers queue
 *
 * The caller must assure the pending_buf_q list is not empty.
 */
static inline struct viss_vid_buffer *viss_pending_queue_pop(
				     struct viss_video *vid_prev)
{
	struct viss_vid_buffer *buf;

	buf = list_entry(vid_prev->pending_buf_q.next,
			struct viss_vid_buffer, list);
	list_del(&buf->list);
	return buf;
}

/**
 * viss_active_queue_pop - pop buffer from the preview active buffers queue
 *
 * The caller must assure the active_buf_q list is not empty.
 */
static inline struct viss_vid_buffer *viss_active_queue_pop_tail(
				    struct viss_video *vid_prev)
{
	struct viss_vid_buffer *buf;

	buf = list_entry(vid_prev->active_buf_q.prev,
			 struct viss_vid_buffer, list);
	list_del(&buf->list);
	return buf;
}

/**
 * viss_pending_queue_add - add buffer to the preview pending buffers queue
 * @buf: buffer to add to the pending buffers list
 */
static inline void viss_pending_queue_add_front(struct viss_video *vid_prev,
					  struct viss_vid_buffer *buf)
{
	list_add(&buf->list, &vid_prev->pending_buf_q);
}

#endif /* VISS_ISP_PREVIEW_H_ */
