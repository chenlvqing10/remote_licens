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

#ifndef VISS_ISP_H_
#define VISS_ISP_H_

#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/videodev2.h>

#include <media/media-entity.h>
#include <media/videobuf2-core.h>
#include <media/v4l2-device.h>
#include <media/v4l2-mediabus.h>
#include <media/v4l2-event.h>
#include <linux/viss_isp.h>

#include "viss.h"
#include "viss-isp-preview.h"

#define VISS_IS_DRV_NAME		"viss-is"
#define VISS_IS_MAX_NUM 2

#define VISS_ISP_SUBPATH_WIDTH_MAX	1280
#define VISS_ISP_SUBPATH_HEIGHT_MAX	720

#define VISS_ISP_SINK_WIDTH_MIN		(16 + 8)
#define VISS_ISP_SINK_HEIGHT_MIN	(12 + 8)

#define VISS_ISP_SINK_WIDTH_MAX		(4000 - 16)
#define VISS_ISP_SINK_HEIGHT_MAX	(4000 + 12)

#define VISS_ISP_NUM_FORMATS		5

#define VISS_ISP_SD_PAD_SINK		0
#define VISS_ISP_SD_PAD_SRC_P0		1
#define VISS_ISP_SD_PAD_SRC_P1		2
#define VISS_ISP_SD_PADS_NUM		3

struct viss_isp_ctrls {
	struct v4l2_ctrl_handler handler;

	/* Auto white balance */
	struct v4l2_ctrl *auto_wb;
	/* Auto ISO control cluster */
	struct {
		struct v4l2_ctrl *auto_iso;
		struct v4l2_ctrl *iso;
	};
	/* Adjust - contrast */
	struct v4l2_ctrl *contrast;
	/* Adjust - saturation */
	struct v4l2_ctrl *saturation;
	/* Adjust - sharpness */
	struct v4l2_ctrl *sharpness;
	/* Adjust - brightness */
	struct v4l2_ctrl *brightness;
	/* Adjust - hue */
	struct v4l2_ctrl *hue;

	/* Auto/manual exposure */
	struct v4l2_ctrl *auto_exp;
	/* Manual exposure value */
	struct v4l2_ctrl *exposure;
	/* AE/AWB lock/unlock */
	struct v4l2_ctrl *aewb_lock;
	/* Exposure metering mode */
	struct v4l2_ctrl *exp_metering;
	/* Exposure bias */
	struct v4l2_ctrl *exp_bias;
	/* AFC */
	struct v4l2_ctrl *afc;
	/* WDR */
	struct v4l2_ctrl *wdr;
	/* ISP image effect */
	struct v4l2_ctrl *colorfx;
	struct v4l2_ctrl *win_pos_x;
	struct v4l2_ctrl *win_pos_y;
	struct v4l2_ctrl *win_widht;
	struct v4l2_ctrl *win_height;
};

/**
 * struct viss_isp - VISS-IS ISP data structure
 * @pdev: pointer to VISS-IS platform device
 * @alloc_ctx: videobuf2 memory allocator context
 * @subdev: ISP v4l2_subdev
 * @subdev_pads: the ISP subdev media pads
 * @ctrl_handler: v4l2 controls handler
 * @test_pattern: test pattern controls
 * @pipeline: video capture pipeline data structure
 * @video_lock: mutex serializing video device and the subdev operations
 * @fmt: pointer to color format description structure
 * @payload: image size in bytes (w x h x bpp)
 * @inp_frame: camera input frame structure
 * @out_frame: DMA output frame structure
 * @state: driver state flags
 * @video_capture: the ISP block video capture device
 */
struct viss_isp_memory {
	/* physical base address */
	dma_addr_t paddr;
	/* virtual base address */
	void *vaddr;
	/* total length */
	unsigned int size;
};

struct viss_isp {
	struct platform_device		*pdev;
	struct v4l2_subdev		subdev;
	struct media_pad		subdev_pads[VISS_ISP_SD_PADS_NUM];
	struct v4l2_mbus_framefmt	subdev_fmt;
	struct viss_isp_ctrls		ctrls;

	struct mutex			subdev_lock;

	struct v4l2_rect		inp_frame;
	struct v4l2_rect		out_frame[MAX_PATH];
	struct viss_isp_memory		tnr_mem_y;
	struct viss_isp_memory		tnr_mem_cb;
	struct viss_isp_memory		tnr_mem_cr;

	struct viss_isp_memory		stat[MAX_STAT];
	struct viss_isp_memory		stat_total;

	struct viss_isp_memory		save_yuv[3];
	struct timespec			ts;
	unsigned int			frame_time;
	unsigned int			output_format;
	unsigned int			vstart_cnt;

	int				id;
};
/* The driver's internal state flags */
enum is_state {
	IS_ST_IDLE,
	IS_ST_PWR_ON,
	IS_ST_STREAM,
	IS_ST_END,
};

enum is_srcif {
	IS_SOURCE_DVP,
	IS_SOURCE_MCSI,
};

enum output_format {
	PLANAR,
	SEMIPLANAR,
};

struct viss_command_ack {
	struct completion wait_complete;
};

/**
 * struct viss_is - viss-is data structure
 * @pdev: pointer to VISS-IS platform device
 * @lock: mutex serializing video device and the subdev operations
 * @slock: spinlock protecting this data structure and the hw registers
 * @regs: MCUCTL mmapped registers region
 * @irq_queue: interrupt handling waitqueue
 * @state: internal driver's state flags
 */
struct viss_is {
	struct platform_device		*pdev;
	struct viss_isp			isp;
	struct viss_is_dev		*viss;
	struct v4l2_ctrl_handler	ctrl_handler;

	struct delayed_work		watchdog;
	struct viss_command_ack		cmd_ack;
	struct work_struct		work;
	struct mutex			lock;
	spinlock_t			slock;

	void __iomem			*regs;
	int				irq;
	u8				isp_reparse;

	unsigned long			state;

	enum is_srcif			srcif;

	unsigned int			reg_data[0x3000/4];
	unsigned int			reg_add[0x3000/4];
	unsigned int			awb_stat[20];

	struct viss_isp_exif		exif;
	char				json[64];
};

static inline struct viss_is *viss_isp_to_is(struct viss_isp *isp)
{
	return container_of(isp, struct viss_is, isp);
}

#define ctrl_to_viss_isp(_ctrl) \
	container_of(ctrl->handler, struct viss_isp, ctrls.handler)

#endif /* VISS_ISP_H_ */
