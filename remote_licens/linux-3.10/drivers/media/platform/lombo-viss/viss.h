/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * VISS-VIC driver code for LomboTech
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

#ifndef ___VISS__H___
#define ___VISS__H___

#include <linux/sizes.h>
#include <linux/io.h>
#include <linux/irqreturn.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/videodev2.h>

#include <media/media-entity.h>
#include <media/videobuf2-core.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-mediabus.h>
#include <media/lombo_viss.h>

#include "csp.h"

enum viss_dev_flags {
	/* capture node */
	ST_PREV_SHUT,
	ST_PREV_BUSY,
	ST_PREV_LPM,
	ST_PREV_PENDING,
	ST_PREV_RUN,
	ST_PREV_STREAM,
	ST_PREV_SUSPENDED,
	ST_PREV_OFF,
	ST_PREV_IN_USE,
	ST_PREV_CONFIG,
	ST_SENSOR_STREAM,
	ST_LPM,
};

enum viss_datapath {
	VISS_IO_NONE,
	VISS_IO_CAMERA,
	VISS_IO_DMA,
	VISS_IO_LCDFIFO,
	VISS_IO_WRITEBACK,
	VISS_IO_ISP,
};

/*
 * The subdevices' group IDs.
 */
#define GRP_ID_SENSOR		(1 << 8)
#define GRP_ID_MCSI		(1 << 9)
#define GRP_ID_VIC		(1 << 10)
#define GRP_ID_VISS		(1 << 11)
#define GRP_ID_VISS_IS		(1 << 12)


#define VISS_MAX_PLANES				3
#define VISS_MAX_DMA_CH				4

#define VISS_SD_PAD_SINK			0
#define VISS_SD_PAD_SOURCE_DMA			1
#define VISS_SD_PAD_SOURCE_ISP			2
/*for VIC/MCSI */
#define VISS_SD_PADS_NUM			3
/* for ISP/ISP-LITE there are no source to isp */
#define VISS_REQ_BUFS_MIN			2

/*
 * v4l2_device notification id. This is only for internal use in the kernel.
 * Sensor subdevs should issue N7_VISS_TX_END_NOTIFY notification in single
 * frame capture mode when there is only one VSYNC pulse issued by the sensor
 * at begining of the frame transmission.
 */
#define N7_VISS_TX_END_NOTIFY _IO('e', 0)
#define VISS_MAX_PLANES	3
#define FMT_FLAGS_RAW_BAYER	(1 << 0)
#define FMT_FLAGS_YUV		(1 << 1)
#define FMT_FLAGS_CAM		(1 << 2)
#define FMT_FLAGS_M2M_IN	(1 << 3)
#define FMT_FLAGS_M2M_OUT	(1 << 4)
#define FMT_FLAGS_M2M		(1 << 3 | 1 << 4)
#define FMT_HAS_ALPHA		(1 << 5)
#define FMT_FLAGS_COMPRESSED	(1 << 6)
#define FMT_FLAGS_WRITEBACK	(1 << 7)

/**
 * struct viss_fmt - color format data structure
 * @mbus_code: media bus pixel code, -1 if not applicable
 * @name: format description
 * @fourcc: fourcc code for this format, 0 if not applicable
 * @color: the driver's private color format id
 * @memplanes: number of physically non-contiguous data planes
 * @colplanes: number of physically contiguous data planes
 * @depth: per plane driver's private 'number of bits per pixel'
 * @mdataplanes: bitmask indicating meta data plane(s), (1 << plane_no)
 * @flags: flags indicating which operation mode format applies to
 */
struct viss_fmt {
	enum v4l2_mbus_pixelcode mbus_code;
	char	*name;
	u32	fourcc;
	u32	color;
	u16	memplanes;
	u16	colplanes;
	u8	depth[VISS_MAX_PLANES];
	u16	mdataplanes;
	u16	flags;
};

enum viss_subdev_index {
	IDX_SENSOR,
	IDX_MCSI,
	IDX_VIC,
	IDX_IS_ISP,
	IDX_VISS,
	IDX_MAX,
};

struct media_pipeline;
struct v4l2_subdev;

struct lombo_media_pipeline {
	struct media_pipeline mp;
	const struct viss_pipeline_ops *ops;
};

struct viss_pipeline {
	struct lombo_media_pipeline ep;
	struct list_head list;
	struct v4l2_subdev *subdevs[IDX_MAX];
	struct media_pipeline *m_pipeline;
};

/*
 * Media pipeline operations to be called from within the viss
 * video node when it is the last entity of the pipeline. Implemented
 * by corresponding media device driver.
 */
struct viss_pipeline_ops {
	int (*prepare)(struct lombo_media_pipeline *p,
						struct media_entity *me);
	int (*unprepare)(struct lombo_media_pipeline *p);
	int (*open)(struct viss_pipeline *p, struct media_entity *me,
			  bool resume);
	int (*close)(struct viss_pipeline *p);
	int (*set_stream)(struct viss_pipeline *p, bool state);
	int (*restart)(struct viss_pipeline *p, int level);
};

#define viss_pipeline_call(f, op, p, args...)				\
	(!(f) ? -ENODEV : (((f)->pipeline_ops && (f)->pipeline_ops->op) ? \
			    (f)->pipeline_ops->op((p), ##args) : -ENOIOCTLCMD))

#if 0
struct lombo_media_pipeline_ops {
	int (*prepare)(struct lombo_media_pipeline *p,
						struct media_entity *me);
	int (*unprepare)(struct lombo_media_pipeline *p);
	int (*open)(struct lombo_media_pipeline *p, struct media_entity *me,
							bool resume);
	int (*close)(struct lombo_media_pipeline *p);
	int (*set_stream)(struct lombo_media_pipeline *p, bool state);
};
#endif

struct lombo_video_entity {
	struct video_device vdev;
	struct viss_pipeline_ops *pipe;
};

static inline struct lombo_video_entity *vdev_to_lombo_video_entity(
					struct video_device *vdev)
{
	return container_of(vdev, struct lombo_video_entity, vdev);
}

#define viss_isp_pipeline_call(ent, op, args...) \
	((!(ent) || !(ent)->pipe) ? -ENOENT : \
	(((ent)->pipe->ops && (ent)->pipe->ops->op) ? \
	(ent)->pipe->ops->op(((ent)->pipe), ##args) : -ENOIOCTLCMD))	  \

struct viss_events {
	unsigned int p0_overflow;
	unsigned int p1_overflow;
	unsigned int p2_overflow;
};

/**
 * struct viss_frame - source/target frame properties
 * @f_width: full pixel width
 * @f_height: full pixel height
 * @sub_width:width of single channel in multi-channel
 * @sub_height:height of single channel in multi-channel
 * @rect: crop/composition rectangle
 * @fmt: pointer to pixel format description data structure
 */

struct viss_frame {
	u16			f_width;
	u16			f_height;
	u32		sub_width;
	u32		sub_height;
	struct v4l2_rect	rect;
	const struct viss_fmt	*fmt;
};

/**
 * struct viss_addr - the VISS physical address set for DMA
 * @y:	 luminance plane physical address
 * @cb:	 Cb plane physical address
 * @cr:	 Cr plane physical address
 */
struct viss_addr {
	u32	y;
	u32	cb;
	u32	cr;
};

/**
 * struct viss_buffer - the driver's video buffer
 * @vb:    v4l videobuf buffer
 * @list:  linked list structure for buffer queue
 * @paddr: precalculated physical address set
 * @index: buffer index for the output DMA engine
 */
struct viss_buffer {
	struct vb2_buffer	vb;
	struct list_head	list;
	struct viss_addr	paddr;
	int			index;
};

struct viss_drvdata {
	u16		max_width;
	u16		max_height;
	u16		out_width_align;
	u16		win_hor_offs_align;
	u16		out_hor_offs_align;
};

/**
 * struct viss_dev - viss device structure
 * @pdev: pointer to VISS device platform device
 * @dd: SoC specific driver data structure
 * @v4l2_dev: pointer to top the level v4l2_device
 * @vfd: video device node
 * @fh: v4l2 file handle
 * @alloc_ctx: videobuf2 memory allocator context
 * @subdev: VISS subdev
 * @vd_pad: media (sink) pad for the capture video node
 * @subdev_pads: the subdev media pads
 * @sensor: sensor subdev attached to VISS directly or through MIPI-CSIS
 * @ctrl_handler: v4l2 control handler
 * @test_pattern: test pattern controls
 * @index: VISS platform device index
 * @pipeline: video capture pipeline data structure
 * @pipeline_ops: media pipeline ops for the video node driver
 * @nr: video id
 * @slock: spinlock protecting this data structure and the hw registers
 * @lock: mutex serializing video device and the subdev operations
 * @clock: VISS gate clock
 * @regs: memory mapped io registers
 * @irq_queue: interrupt handler waitqueue
 * @payload: image size in bytes (w x h x bpp)
 * @inp_frame: camera input frame structure
 * @out_frame: DMA output frame structure
 * @out_path: output data path (DMA or FIFO)
 * @source_subdev_grp_id: source subdev group id
 * @state: driver state flags
 * @pending_buf_q: pending buffers queue head
 * @active_buf_q: the queue head of buffers scheduled in hardware
 * @vb_queue: vb2 buffers queue
 * @active_buf_count: number of video buffers scheduled in hardware
 * @frame_count: the captured frames counter
 * @reqbufs_count: the number of buffers requested with REQBUFS ioctl
 * @ref_count: driver's private reference counter
 */
struct viss_dev {
	struct platform_device		*pdev;
	struct viss_drvdata		*dd;
	struct v4l2_device		*v4l2_dev;
	struct video_device		vfd;
	struct v4l2_fh			fh;
	struct vb2_alloc_ctx		*alloc_ctx;
	struct v4l2_subdev		subdev;
	struct media_pad		vd_pad;
	struct media_pad		subdev_pads[VISS_SD_PADS_NUM];
	struct v4l2_mbus_framefmt	ci_fmt;
	struct v4l2_subdev		*sensor;
	struct v4l2_ctrl_handler	ctrl_handler;
	struct v4l2_ctrl		*test_pattern;
	int				index;
	struct viss_pipeline		pipeline;
	const struct viss_pipeline_ops *pipeline_ops;
	int				nr;

	struct mutex			lock;
	spinlock_t			slock;

	struct clk			*clock;
	void __iomem			*regs;
	wait_queue_head_t		irq_queue;

	unsigned long			payload[VISS_MAX_PLANES];
	unsigned long			bytesperline[VISS_MAX_PLANES];
	unsigned long			bpl;
	struct viss_frame		inp_frame;
	struct viss_frame		out_frame;
	atomic_t			out_path;
	unsigned int			source_subdev_grp_id;

	unsigned long			state;
	struct list_head		pending_buf_q;
	struct list_head		active_buf_q;
	struct vb2_queue		vb_queue;
	unsigned int			frame_count;
	unsigned int			reqbufs_count;
	int				ref_count;

	void				*tmp_buf0_va;
	u32				tmp_buf0_pa;
	void				*tmp_buf1_va;
	u32				tmp_buf1_pa;

	/* Use 0 to ch_number DMA channels */
	u8				ch_number;
	struct viss_addr		ch_paddr[VISS_MAX_DMA_CH];
	u32				interlaced;
	u8				sort;

#if 1
	int				active_cnt;
	int				pending_cnt;
#endif

	struct viss_events		events[VISS_MAX_DMA_CH];
	bool				streaming;
	bool				user_subdev_api;

	struct timer_list		timer;
	u32				to_cnt;

	/* for frame rate account */
	u32				itv_frames;
	u32				frames;
	struct timeval			last_tv;

	u32				frames_lost_count;

	/* the FCI of each channel before the new buffer is not followed */
	u8				ch_pre_fci[VISS_MAX_DMA_CH];
};

#endif /* ___VISS__H___ */
