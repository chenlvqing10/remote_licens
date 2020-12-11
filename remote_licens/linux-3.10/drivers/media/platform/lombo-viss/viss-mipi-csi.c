/*
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * VISS-MIPI-CSI driver code for LomboTech
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
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/list.h>
#include <linux/mfd/syscon.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf2-core.h>
#include <media/videobuf2-dma-contig.h>
#include <linux/delay.h>
#include <mach/common.h>

/* #define ENABLE_TRACE */
#include <mach/debug.h>

#include "viss.h"
#include "media-dev.h"
#include "viss-mipi-csi.h"
#include "csp.h"
#include "viss-m2m.h"

#ifdef CONFIG_ARCH_LOMBO_N7V1
static uint32_t mcsi_num;
#endif

static const struct viss_fmt mcsi_formats[] = {
	/* Luminance+Chrominance formats */
	{
		.name		= "YUV 4:2:2 packed, YCbYCr",
		.fourcc		= V4L2_PIX_FMT_YUYV,
		.depth		= { 16 },
		.color		= VISS_FMT_YCBYCR422,
		.memplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_YUYV8_2X8,
		.flags		= FMT_FLAGS_YUV | FMT_FLAGS_CAM,
	}, {
		.name		= "YUV 4:2:2 packed, CbYCrY",
		.fourcc		= V4L2_PIX_FMT_UYVY,
		.depth		= { 16 },
		.color		= VISS_FMT_CBYCRY422,
		.memplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_UYVY8_2X8,
		.flags		= FMT_FLAGS_YUV | FMT_FLAGS_CAM,
	}, {
		.name		= "YUV 4:2:2 packed, CrYCbY",
		.fourcc		= V4L2_PIX_FMT_VYUY,
		.depth		= { 16 },
		.color		= VISS_FMT_CRYCBY422,
		.memplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_VYUY8_2X8,
		.flags		= FMT_FLAGS_YUV | FMT_FLAGS_CAM,
	}, {
		.name		= "YUV 4:2:2 packed, YCrYCb",
		.fourcc		= V4L2_PIX_FMT_YVYU,
		.depth		= { 16 },
		.color		= VISS_FMT_YCRYCB422,
		.memplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_YVYU8_2X8,
		.flags		= FMT_FLAGS_YUV | FMT_FLAGS_CAM,
	}, {
		.name		= "YUV 4:2:2 planar, Y/Cb/Cr",
		.fourcc		= V4L2_PIX_FMT_YUV422P,
		.depth		= { 12 },
		.color		= VISS_FMT_YCBYCR422,
		.memplanes	= 1,
		.colplanes	= 3,
		.flags		= FMT_FLAGS_YUV,
	}, {
		.name		= "YUV 4:2:2 semi-planar, Y/CbCr",
		.fourcc		= V4L2_PIX_FMT_NV16,
		.depth		= { 16 },
		.color		= VISS_FMT_YCBYCR422,
		.memplanes	= 1,
		.colplanes	= 2,
		.flags		= FMT_FLAGS_YUV,
	}, {
		.name		= "YUV 4:2:2 semi-planar, Y/CrCb",
		.fourcc		= V4L2_PIX_FMT_NV61,
		.depth		= { 16 },
		.color		= VISS_FMT_YCRYCB422,
		.memplanes	= 1,
		.colplanes	= 2,
		.flags		= FMT_FLAGS_YUV,
	}, {
		.name		= "YUV 4:2:0 planar, YCbCr",
		.fourcc		= V4L2_PIX_FMT_YUV420,
		.depth		= { 12 },
		.color		= VISS_FMT_YCBCR420,
		.memplanes	= 1,
		.colplanes	= 3,
		.flags		= FMT_FLAGS_YUV,
	}, {
		.name		= "YUV 4:2:0 semi-planar, Y/CbCr",
		.fourcc		= V4L2_PIX_FMT_NV12,
		.depth		= { 12 },
		.color		= VISS_FMT_YCBCR420,
		.memplanes	= 1,
		.colplanes	= 2,
		.flags		= FMT_FLAGS_YUV,
	}, {
		.name		= "YUV 4:2:0 semi-planar, Y/CrCb",
		.fourcc		= V4L2_PIX_FMT_NV21,
		.depth		= { 12 },
		.color		= VISS_FMT_YCRCB420,
		.memplanes	= 1,
		.colplanes	= 2,
		.flags		= FMT_FLAGS_YUV,
	}, {
		.name		= "YUV 4:2:0 non-contig. 2p, Y/CbCr",
		.fourcc		= V4L2_PIX_FMT_NV12M,
		.color		= VISS_FMT_YCBCR420,
		.depth		= { 8, 4 },
		.memplanes	= 2,
		.colplanes	= 2,
		.flags		= FMT_FLAGS_YUV,
	}, {
		.name		= "YUV 4:2:0 non-contig. 2p, Y/CrCb",
		.fourcc		= V4L2_PIX_FMT_NV21M,
		.color		= VISS_FMT_YCRCB420,
		.depth		= { 8, 4 },
		.memplanes	= 2,
		.colplanes	= 2,
		.flags		= FMT_FLAGS_YUV,
	}, {
		.name		= "YUV 4:2:0 non-contig. 3p, Y/Cb/Cr",
		.fourcc		= V4L2_PIX_FMT_YUV420M,
		.color		= VISS_FMT_YCBCR420,
		.depth		= { 8, 2, 2 },
		.memplanes	= 3,
		.colplanes	= 3,
		.flags		= FMT_FLAGS_YUV,
	}, {
		.name		= "YUV 4:2:0 non-contig. 3p, Y/Cr/Cb",
		.fourcc		= V4L2_PIX_FMT_YVU420M,
		.color		= VISS_FMT_YCRCB420,
		.depth		= { 8, 2, 2 },
		.memplanes	= 3,
		.colplanes	= 3,
		.flags		= FMT_FLAGS_YUV,
	},
	/* Bayer formats */	/* export RAW8 as NV12 */
	 {
		.name		= "RAW8 (BGGR)",
		.fourcc		= V4L2_PIX_FMT_SBGGR8,
		.depth		= { 12 },
		.color		= VISS_FMT_RAW8,
		.memplanes	= 1,
		.colplanes	= 2,
		.mbus_code	= V4L2_MBUS_FMT_SBGGR8_1X8,
		.flags		= FMT_FLAGS_RAW_BAYER | FMT_FLAGS_CAM,
	}, {
		.name		= "RAW8 (GBRG)",
		.fourcc		= V4L2_PIX_FMT_SGBRG8,
		.depth		= { 12 },
		.color		= VISS_FMT_RAW8,
		.memplanes	= 1,
		.colplanes	= 2,
		.mbus_code	= V4L2_MBUS_FMT_SGBRG8_1X8,
		.flags		= FMT_FLAGS_RAW_BAYER | FMT_FLAGS_CAM,
	}, {
		.name		= "RAW8 (GRBG)",
		.fourcc		= V4L2_PIX_FMT_SGRBG8,
		.depth		= { 12 },
		.color		= VISS_FMT_RAW8,
		.memplanes	= 1,
		.colplanes	= 2,
		.mbus_code	= V4L2_MBUS_FMT_SGRBG8_1X8,
		.flags		= FMT_FLAGS_RAW_BAYER | FMT_FLAGS_CAM,
	}, {
		.name		= "RAW8 (RGGB)",
		.fourcc		= V4L2_PIX_FMT_SRGGB8,
		.depth		= { 12 },
		.color		= VISS_FMT_RAW8,
		.memplanes	= 1,
		.colplanes	= 2,
		.mbus_code	= V4L2_MBUS_FMT_SRGGB8_1X8,
		.flags		= FMT_FLAGS_RAW_BAYER | FMT_FLAGS_CAM,
	},
	/* default use parser long pkg, so RAW10 and RAW12 is expand to 16bit */
	{
		.name		= "RAW10 (BGGR)",
		.fourcc		= V4L2_PIX_FMT_SBGGR10,
		.depth		= { 16 },
		.color		= VISS_FMT_RAW10,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_SBGGR10_1X10,
		.flags		= FMT_FLAGS_RAW_BAYER | FMT_FLAGS_CAM,
	}, {
		.name		= "RAW10 (GBRG)",
		.fourcc		= V4L2_PIX_FMT_SGBRG10,
		.depth		= { 16 },
		.color		= VISS_FMT_RAW10,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_SGBRG10_1X10,
		.flags		= FMT_FLAGS_RAW_BAYER | FMT_FLAGS_CAM,
	}, {
		.name		= "RAW10 (GRBG)",
		.fourcc		= V4L2_PIX_FMT_SGRBG10,
		.depth		= { 16 },
		.color		= VISS_FMT_RAW10,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_SGRBG10_1X10,
		.flags		= FMT_FLAGS_RAW_BAYER | FMT_FLAGS_CAM,
	}, {
		.name		= "RAW10 (RGGB)",
		.fourcc		= V4L2_PIX_FMT_SRGGB10,
		.depth		= { 16 },
		.color		= VISS_FMT_RAW10,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_SRGGB10_1X10,
		.flags		= FMT_FLAGS_RAW_BAYER | FMT_FLAGS_CAM,
	}, {
		.name		= "RAW12 (BGGR)",
		.fourcc		= V4L2_PIX_FMT_SBGGR12,
		.depth		= { 16 },
		.color		= VISS_FMT_RAW12,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_SBGGR12_1X12,
		.flags		= FMT_FLAGS_RAW_BAYER | FMT_FLAGS_CAM,
	}, {
		.name		= "RAW12 (GBRG)",
		.fourcc		= V4L2_PIX_FMT_SGBRG12,
		.depth		= { 16 },
		.color		= VISS_FMT_RAW12,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_SGBRG12_1X12,
		.flags		= FMT_FLAGS_RAW_BAYER | FMT_FLAGS_CAM,
	}, {
		.name		= "RAW12 (GRBG)",
		.fourcc		= V4L2_PIX_FMT_SGRBG12,
		.depth		= { 16 },
		.color		= VISS_FMT_RAW12,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_SGRBG12_1X12,
		.flags		= FMT_FLAGS_RAW_BAYER | FMT_FLAGS_CAM,
	}, {
		.name		= "RAW12 (RGGB)",
		.fourcc		= V4L2_PIX_FMT_SRGGB12,
		.depth		= { 16 },
		.color		= VISS_FMT_RAW12,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_SRGGB12_1X12,
		.flags		= FMT_FLAGS_RAW_BAYER | FMT_FLAGS_CAM,
	},
};

/**
 * mcsi_find_format - lookup MIPI-CSI color format by fourcc or media bus code
 * @pixelformat: fourcc to match, ignored if null
 * @mbus_code: media bus code to match, ignored if null
 * @mask: the color format flags to match
 * @index: index to the mcsi_formats array, ignored if negative
 */
static const struct viss_fmt *mcsi_find_format(const u32 *pixelformat,
			const u32 *mbus_code, unsigned int mask, int index)
{
	const struct viss_fmt *fmt, *def_fmt = NULL;
	unsigned int i;
	int id = 0;

	if (index >= (int)ARRAY_SIZE(mcsi_formats))
		return NULL;

	PRT_TRACE_BEGIN("\n");

	if (pixelformat != NULL)
		PRT_DBG("*pixelformat=0x%x\n", *pixelformat);
	if (mbus_code != NULL)
		PRT_DBG("*mbus_code=0x%x\n", *mbus_code);

	for (i = 0; i < ARRAY_SIZE(mcsi_formats); ++i) {
		fmt = &mcsi_formats[i];
		if (mask && !(fmt->flags & mask))
			continue;

		if (pixelformat && (fmt->fourcc == *pixelformat)) {
			PRT_TRACE_END("\n");
			return fmt;
		}

		if (mbus_code && fmt->mbus_code == *mbus_code) {
			PRT_TRACE_END("\n");
			return fmt;
		}

		if (index == id)
			def_fmt = fmt;
		id++;
	}

	PRT_TRACE_END("id=%d,def_fmt->fourcc=0x%x\n", id, def_fmt->fourcc);
	return def_fmt;
}

static void mcsi_account_framerate(struct viss_dev *mcsi, struct timeval *tv)
{
	u32 timeused = 0;
	u32 frmr_int = 0;
	u32 frmr_dec = 0;

	if (0 == mcsi->itv_frames)
		return;

	if (mcsi->frames == 0) {
		mcsi->last_tv = *tv;
	} else if (mcsi->frames == mcsi->itv_frames) {

		timeused = (u64)(tv->tv_sec - mcsi->last_tv.tv_sec) *
				USEC_PER_SEC +
				tv->tv_usec - mcsi->last_tv.tv_usec;
		frmr_int = (mcsi->itv_frames * 1000 * 1000) /
			(timeused / 1000);
		frmr_dec = frmr_int % 1000;
		frmr_int = frmr_int / 1000;

		PRT_INFO("output %d frames timeused %u us\n",
			mcsi->itv_frames, timeused);
		PRT_INFO("frame rate %u.%03u\n", frmr_int, frmr_dec);

		mcsi->last_tv = *tv;
		mcsi->frames = 0;
	}
	mcsi->frames++;
}

static ssize_t viss_mcsi_reparse_sysfs_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct viss_dev *mcsi = platform_get_drvdata(pdev);
	int count = 0;

	if (isp_is_offline(mcsi->sensor)) {
		struct viss_buffer *buf;
		mcsi->reqbufs_count = RAW_BUF_NUM;
		mcsi->pending_cnt = 0;
		mcsi->active_cnt = 0;
		list_for_each_entry(buf, &mcsi->pending_buf_q, list)
			mcsi->pending_cnt++;
		list_for_each_entry(buf, &mcsi->active_buf_q, list)
			mcsi->active_cnt++;
	}

	count += sprintf(buf + count, "mcsi streaming %d\n",
			mcsi->streaming);
	count += sprintf(buf + count, "mcsi state %ld\n",
			mcsi->state);
	count += sprintf(buf + count, "frame count %d\n",
			mcsi->frame_count);
	count += sprintf(buf + count, "frame lost count %d\n",
			mcsi->frames_lost_count);
	count += sprintf(buf + count, "requset buf count %d\n",
			mcsi->reqbufs_count);
	count += sprintf(buf + count, "pending buf count %d\n",
			mcsi->pending_cnt);
	count += sprintf(buf + count, "active buf count %d\n",
			mcsi->active_cnt);
	count += sprintf(buf + count, "frame size input: %dx%d\n",
		mcsi->inp_frame.f_width, mcsi->inp_frame.f_height);
	count += sprintf(buf + count, "frame size output: %dx%d\n",
		mcsi->out_frame.f_width, mcsi->out_frame.f_height);

	return count;
}

static ssize_t viss_mcsi_reparse_sysfs_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct viss_dev *mcsi = platform_get_drvdata(pdev);
	unsigned int itv_frames = 0;
	unsigned long flags;
	int n = 0;

	n = sscanf(buf, "%d", &itv_frames);
	if (n != 1) {
		PRT_ERR("Invalid interval frames (%s)\n", buf);
		return count;
	}

	spin_lock_irqsave(&mcsi->slock, flags);
	mcsi->frames = 0;
	mcsi->itv_frames = itv_frames;
	spin_unlock_irqrestore(&mcsi->slock, flags);

	return count;
}

static DEVICE_ATTR(mcsi_reparse, S_IWUSR | S_IRUGO,
	viss_mcsi_reparse_sysfs_show, viss_mcsi_reparse_sysfs_store);

/* Called with the media graph mutex held or @me stream_count > 0. */
static struct v4l2_subdev *__find_remote_sensor(struct media_entity *me)
{
	struct media_pad *pad = &me->pads[0];
	struct v4l2_subdev *sd;

	PRT_TRACE_BEGIN("\n");
	while (pad->flags & MEDIA_PAD_FL_SINK) {
		/* source pad */
		pad = media_entity_remote_source(pad);
		if (pad == NULL ||
		    media_entity_type(pad->entity) != MEDIA_ENT_T_V4L2_SUBDEV)
			break;

		sd = media_entity_to_v4l2_subdev(pad->entity);

		if (sd->grp_id == GRP_ID_SENSOR) {
			PRT_TRACE_END("\n");
			return sd;
		}
		/* sink pad */
		pad = &sd->entity.pads[0];
	}
	PRT_TRACE_END("\n");
	return NULL;
}

static int __mcsi_if_config(struct viss_dev *mcsi,
				struct viss_source_info *si)
{
	struct mcsi_if_info *mcsi_if = NULL;
	u32 mcsi_dt = MCSI_YUV422;

	mcsi_if = (struct mcsi_if_info *)si->if_info;

	if (mcsi_if == NULL) {
		PRT_ERR("si->if_info is NULL!\n");
		return -ENODEV;
	}

	PRT_INFO("dlanes=%d,data_type=%d,mfreq=%d\n",
		mcsi_if->dlanes, mcsi_if->data_type, mcsi_if->mfreq);

	if (mcsi_if->data_type == VISS_MCSI_RAW8)
		mcsi_dt = MCSI_RAW8;
	else if (mcsi_if->data_type == VISS_MCSI_RAW10)
		mcsi_dt = MCSI_RAW10;
	else if (mcsi_if->data_type == VISS_MCSI_RAW12)
		mcsi_dt = MCSI_RAW12;

	/* parse long package */
	csp_mcsi_parse_long_pkg();
#ifdef CONFIG_ARCH_LOMBO_N7V1
	if (2 == mcsi_num) {
		csp_mipi_dphy_init_1(mcsi_if->dlanes, MCSI_ID0, mcsi_dt,
						MCSI_VC0, mcsi_if->mfreq);
		/* PRT_INFO("csp_mipi_dphy_init_1\n"); */
	} else {
		csp_mipi_dphy_init_0(mcsi_if->dlanes, MCSI_ID0, mcsi_dt,
						MCSI_VC0, mcsi_if->mfreq);
		/* PRT_INFO("csp_mipi_dphy_init_2\n"); */
	}
#else
	csp_mipi_dphy_init_0(mcsi_if->dlanes, MCSI_ID0, mcsi_dt,
						MCSI_VC0, mcsi_if->mfreq);
		/* PRT_INFO("csp_mipi_dphy_init_2\n"); */
#endif
#ifdef CONFIG_ARCH_LOMBO_N7V1
	/* set head filter configuration */
	csp_mcsi_pixel_channel_head_filter(mcsi_dt, MCSI_VC0);
#endif
	/* set head filter configuration */
	csp_mcsi_dma_channel_head_filter(mcsi_dt, MCSI_VC0);
	/* set head mapping configuration */
	csp_mcsi_dma_channel_head_mapping(mcsi_dt, MCSI_VC0);

	return 0;
}

static void __mcsi_yuv_seq_adjust(struct viss_dev *mcsi)
{
	u32 yuv_seq = 0;
	u32 uv_swap = 0;
	u32 out_fmt = MCSI_OUTFMT_YUV420SP;

	if (mcsi->out_frame.fmt->flags & FMT_FLAGS_CAM)
		out_fmt = MCSI_OUTFMT_PAST_THROUGH;
	else {
		switch (mcsi->out_frame.fmt->color) {
		case VISS_FMT_YCBYCR422:
		case VISS_FMT_YCRYCB422:
			if ((mcsi->out_frame.fmt->color) ==
				VISS_FMT_YCRYCB422)
				uv_swap = 1;
			if (mcsi->out_frame.fmt->colplanes == 2)
				out_fmt = MCSI_OUTFMT_YUV422SP;
			else
				out_fmt = MCSI_OUTFMT_YUV422P;
			break;
		case VISS_FMT_YCBCR420:
		case VISS_FMT_YCRCB420:
			if ((mcsi->out_frame.fmt->color) ==
				VISS_FMT_YCRCB420)
				uv_swap = 1;
			if (mcsi->out_frame.fmt->colplanes == 2)
				out_fmt = MCSI_OUTFMT_YUV420SP;
			else
				out_fmt = MCSI_OUTFMT_YUV420P;
			break;
		}

		/* set yuv seq */
		switch (mcsi->inp_frame.fmt->color) {
		case VISS_FMT_YCBYCR422:
			if (uv_swap == 0)
				yuv_seq = MCSI_C1C0C3C2;
			else
				yuv_seq = MCSI_C3C0C1C2;
			break;
		case VISS_FMT_YCRYCB422:
			if (uv_swap == 0)
				yuv_seq = MCSI_C3C0C1C2;
			else
				yuv_seq = MCSI_C1C0C3C2;
			break;
		case VISS_FMT_CBYCRY422:
			if (uv_swap == 0)
				yuv_seq = MCSI_C0C1C2C3;
			else
				yuv_seq = MCSI_C2C1C0C3;
			break;
		case VISS_FMT_CRYCBY422:
			if (uv_swap == 0)
				yuv_seq = MCSI_C2C1C0C3;
			else
				yuv_seq = MCSI_C0C1C2C3;
			break;
		}
	}

	/* set output format */
	csp_mcsi_component_sequence(yuv_seq);
	/* set output format */
	csp_mcsi_dma_output_format(out_fmt);
}

static int mcsi_hw_init(struct viss_dev *mcsi, bool isp_output)
{
	struct mcsi_if_info *mcsi_if = NULL;
	struct viss_source_info *si;
	u32 data_path = 0;
#ifdef CONFIG_ARCH_LOMBO_N7V1
	uint32_t p1 = 0, p2 = 0;
#endif
	u32 ret = 0;

	if (mcsi->sensor == NULL)
		return -ENXIO;

	if (mcsi->inp_frame.fmt == NULL || mcsi->out_frame.fmt == NULL)
		return -EINVAL;

	PRT_TRACE_BEGIN("\n");
#ifdef CONFIG_ARCH_LOMBO_N7V1
	mcsi_num = 0;
	if (lombo_func1())
		return -EINVAL;
	if (lombo_func2(&p1, &p2))
		return -EINVAL;
	mcsi_num = p1;
#endif
	/* PRT_INFO("mcsi_num=%d\n", mcsi_num); */
	/* Get sensor configuration data from the sensor subdev */
	si = v4l2_get_subdev_hostdata(mcsi->sensor);
	if (si == NULL) {
		PRT_ERR("can't get sensor configuration data!\n");
		return -EINVAL;
	}

	if (si->if_type != VISS_IF_TYPE_MCSI) {
		PRT_ERR("interface type is not VISS_IF_TYPE_MCSI!\n");
		return -EINVAL;
	}

	csp_viss_top_mcsi_reset();

	ret = __mcsi_if_config(mcsi, si);
	if (ret != 0)
		return ret;

	data_path = isp_output ? MCSI_OUTPUT_PIXEL_CH : MCSI_OUTPUT_DMA;
	csp_mcsi_data_path(data_path);

#if 0	/* It seems not friendly to some sensor */
	PRT_INFO("mcsi->inp_frame.f_width=%d\n",
		mcsi->inp_frame.f_width);
	csp_mcsi_video_width(mcsi->inp_frame.f_width);
#endif

	if (!isp_output) {
		mcsi_if = (struct mcsi_if_info *)si->if_info;

		/* RAW8 is export as NV12 temporary, deal with it first */
		if (mcsi_if->data_type == VISS_MCSI_RAW8)
			csp_mcsi_dma_output_format(MCSI_OUTFMT_PAST_THROUGH);
		else
			__mcsi_yuv_seq_adjust(mcsi);

		csp_mcsi_dma_burst_length(MCSI_DMA_BURST_LEN_8);

		mcsi_if = (struct mcsi_if_info *)si->if_info;

		/* set dma window */
		csp_mcsi_dma_offset(mcsi->inp_frame.rect.left,
				   mcsi->inp_frame.rect.top);

		if (mcsi_if->data_type == VISS_MCSI_RAW8) {
			csp_mcsi_dma_size(mcsi->inp_frame.rect.width,
					 mcsi->inp_frame.rect.height);
		} else {
			csp_mcsi_dma_size(mcsi->inp_frame.rect.width * 2,
					 mcsi->inp_frame.rect.height);
		}

		if (mcsi->out_frame.fmt->colplanes == 1) {
			csp_mcsi_dma_linestride(mcsi->out_frame.f_width * 2, 0);
		} else if (mcsi->out_frame.fmt->colplanes == 2) {
			if (mcsi_if->data_type == VISS_MCSI_RAW8)
				csp_mcsi_dma_linestride(
					mcsi->out_frame.f_width, 0);
			else
				csp_mcsi_dma_linestride(
					mcsi->out_frame.f_width,
					mcsi->out_frame.f_width);
		} else {
			csp_mcsi_dma_linestride(mcsi->out_frame.f_width,
						mcsi->out_frame.f_width / 2);
		}

		csp_mcsi_int_enable(MCSI_INT_DMA);
#ifdef CONFIG_ARCH_LOMBO_N7V1
		if (2 == mcsi_num)
			csp_mcsi_clear_set_ints(MCSI_DMA_INT_ALL_MASK,
				MCSI_DMA_INT_ALL_ERR | MCSI_DMA_INT_FCI |
						MCSI_DMA_INT_LB_OR_EN);
		else
			csp_mcsi_clear_set_ints(MCSI_DMA_INT_ALL_MASK,
				MCSI_DMA_INT_ALL_ERR | MCSI_DMA_INT_FCI);
#else
		csp_mcsi_clear_set_ints(MCSI_DMA_INT_ALL_MASK,
				MCSI_DMA_INT_ALL_ERR | MCSI_DMA_INT_FCI);
#endif

		/* FCI */
		csp_mcsi_dma_update_cmpt_address(MCSI_UD_TIMING_FCI);

		csp_mcsi_cfg_fifo();
	} else {
		if (isp_is_online(mcsi->sensor)) {
			csp_viss_top_isp_data_source(1);
		} else {
			struct offline_buf *buf =
				list_entry(mcsi->pending_buf_q.next,
					struct offline_buf, list);
			csp_mcsi_data_path(MCSI_OUTPUT_DMA);
			csp_mcsi_dma_output_format(MCSI_OUTFMT_PAST_THROUGH);
			csp_mcsi_dma_burst_length(MCSI_DMA_BURST_LEN_8);

			/* set dma window */
			csp_mcsi_dma_offset(0, 0);
			csp_mcsi_dma_size(buf->win.width * buf->raw_depth,
				buf->win.height);
			csp_mcsi_dma_linestride(buf->win.width * buf->raw_depth,
				0);
			if (buf->raw_depth == 1)
				csp_enable_mcsi_output_high_8bit_data();

			csp_mcsi_int_enable(MCSI_INT_DMA);
#ifdef CONFIG_ARCH_LOMBO_N7V1
			if (2 == mcsi_num)
				csp_mcsi_clear_set_ints(MCSI_DMA_INT_ALL_MASK,
						MCSI_DMA_INT_ALL_ERR |
						MCSI_DMA_INT_FCI |
						MCSI_DMA_INT_LB_OR_EN);
			else
				csp_mcsi_clear_set_ints(MCSI_DMA_INT_ALL_MASK,
						MCSI_DMA_INT_ALL_ERR |
						MCSI_DMA_INT_FCI);
#else
			csp_mcsi_clear_set_ints(MCSI_DMA_INT_ALL_MASK,
						MCSI_DMA_INT_ALL_ERR |
						MCSI_DMA_INT_FCI);
#endif
			/* FCI */
			csp_mcsi_dma_update_cmpt_address(MCSI_UD_TIMING_FCI);

			csp_mcsi_cfg_fifo();
		}
	}

	csp_mcsi_dump_regs(__func__);

	PRT_TRACE_END("\n");
	return 0;
}

/*
 * Reinitialize the driver so it is ready to start the streaming again.
 * Set mcsi->state to indicate stream off and the hardware shut down state.
 * If not suspending (@suspend is false), return any buffers to videobuf2.
 * Otherwise put any owned buffers onto the pending buffers queue, so they
 * can be re-spun when the device is being resumed. Also perform MIPI-CSI
 * software reset and disable streaming on the whole pipeline if required.
 */
static int mcsi_state_cleanup(struct viss_dev *mcsi, bool suspend)
{
	struct viss_buffer *buf;
	unsigned long flags;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");
	spin_lock_irqsave(&mcsi->slock, flags);

	mcsi->state &= ~((1 << ST_PREV_RUN) | (1 << ST_PREV_OFF) |
			 (1 << ST_PREV_STREAM));
	if (suspend)
		mcsi->state |= (1 << ST_PREV_SUSPENDED);
	else
		mcsi->state &= ~(1 << ST_PREV_SUSPENDED);

	/* Release unused buffers */
	while (!suspend && !list_empty(&mcsi->pending_buf_q)) {
		buf = mcsi_pending_queue_pop(mcsi);
		vb2_buffer_done(&buf->vb, VB2_BUF_STATE_ERROR);
	}
	/* If suspending put unused buffers onto pending queue */
	while (!list_empty(&mcsi->active_buf_q)) {
		buf = mcsi_active_queue_pop_tail(mcsi);
		if (suspend)
			mcsi_pending_queue_add_front(mcsi, buf);
		else
			vb2_buffer_done(&buf->vb, VB2_BUF_STATE_ERROR);
	}

	/* reset the mcsi */
	csp_mcsi_reset();
	csp_viss_top_mcsi_reset();

	spin_unlock_irqrestore(&mcsi->slock, flags);

	ret = viss_pipeline_call(mcsi, set_stream, &mcsi->pipeline, 0);
	if (ret != 0)
		PRT_ERR("pipeline stream off failed!\n");

	PRT_TRACE_END("\n");
	return ret;
}

static void mcsi_set_dma_addr(struct viss_addr *paddr)
{
	csp_mcsi_dma_cmpt0_addr(paddr->y);
	csp_mcsi_dma_cmpt1_addr(paddr->cb);
	csp_mcsi_dma_cmpt2_addr(paddr->cr);
}

static int mcsi_set_next_buf(struct viss_dev *mcsi)
{
	struct viss_buffer *vbuf;

	PRT_TRACE_BEGIN("\n");

	if (!list_empty(&mcsi->pending_buf_q)) {
		vbuf = mcsi_pending_queue_pop(mcsi);
		mcsi_set_dma_addr(&vbuf->paddr);
		mcsi_active_queue_add(mcsi, vbuf);
	}

	PRT_TRACE_END("\n");
	return 0;
}

static int mcsi_set_next_buf_offline(struct viss_dev *mcsi)
{
	PRT_TRACE_BEGIN("\n");

	if (!list_empty(&mcsi->pending_buf_q)) {
		struct offline_buf *buf = offline_buf_pop(&mcsi->pending_buf_q);
		csp_mcsi_dma_cmpt0_addr(buf->dma.paddr);
		offline_buf_add(&mcsi->active_buf_q, buf);
	}

	PRT_TRACE_END("\n");
	return 0;
}

static int mcsi_start_capture(struct viss_dev *mcsi)
{
	int ret = 0;

	PRT_TRACE_BEGIN("\n");

	if (atomic_read(&mcsi->out_path) == VISS_IO_ISP) {
		if (isp_is_online(mcsi->sensor)) {
			csp_mcsi_enable();
			csp_mcsi_int_clear(MCSI_INT_DMA);
		} else {
			csp_enable_mcsi_output_raw_msb_data();
			mcsi_set_next_buf_offline(mcsi);
			csp_mcsi_dma_capture_mode(MCSI_CAP_CONT);
			csp_mcsi_dma_start();
			csp_mcsi_enable();
			csp_mcsi_clr_pd(MCSI_DMA_INT_ALL_MASK);
			csp_mcsi_int_clear(MCSI_INT_DMA);
			mcsi_set_next_buf_offline(mcsi);
		}
		goto out;
	}

	if (!test_and_set_bit(ST_PREV_STREAM, &mcsi->state)) {
		if (list_empty(&mcsi->pending_buf_q)) {
			PRT_ERR("Pending buffer queue empty!\n");
			return -EINVAL;
		}

		mcsi_set_next_buf(mcsi);
		csp_mcsi_dma_capture_mode(MCSI_CAP_CONT);
		csp_mcsi_dma_start();

		csp_mcsi_enable();

		csp_mcsi_clr_pd(MCSI_DMA_INT_ALL_MASK);
		csp_mcsi_int_clear(MCSI_INT_DMA);

		mcsi_set_next_buf(mcsi);
	}
out:
	PRT_TRACE_END("\n");
	return ret;
}

static int mcsi_stop_capture(struct viss_dev *mcsi, bool suspend)
{
	unsigned long flags;

	if (!mcsi_active(mcsi))
		return 0;

	PRT_TRACE_BEGIN("\n");
	set_bit(ST_PREV_OFF, &mcsi->state);

	if (atomic_read(&mcsi->out_path) == VISS_IO_DMA) {
		del_timer(&mcsi->timer);
		mcsi->to_cnt = 0;
	}

	wait_event_timeout(mcsi->irq_queue,
			   !test_bit(ST_PREV_OFF, &mcsi->state),
			   (2 * HZ / 10)); /* 200 ms */

	spin_lock_irqsave(&mcsi->slock, flags);
	csp_mcsi_dma_stop();
	csp_mcsi_disable();
	spin_unlock_irqrestore(&mcsi->slock, flags);

	return mcsi_state_cleanup(mcsi, suspend);
}

/* Must be called with mcsi.slock spinlock held. */
static void mcsi_config_update(struct viss_dev *mcsi)
{
	PRT_TRACE_BEGIN("\n");

	/* set dma window */
	csp_mcsi_dma_offset(mcsi->inp_frame.rect.left,
			   mcsi->inp_frame.rect.top);
	csp_mcsi_dma_size(mcsi->inp_frame.rect.width * 2,
			 mcsi->inp_frame.rect.height);

	clear_bit(ST_PREV_CONFIG, &mcsi->state);
	PRT_TRACE_END("\n");
}

static int mcsi_restart(struct viss_dev *mcsi, bool isp_output)
{
	struct viss_buffer *vbuf = NULL;
	int ret;

	PRT_TRACE_BEGIN("\n");

	csp_mcsi_dma_stop();
	csp_mcsi_disable();
	csp_mcsi_reset();

	ret = mcsi_hw_init(mcsi, isp_output);
	if (ret) {
		/* ignore the error */
		PRT_WARN("\n");
	}

	if (isp_output) {
		csp_mcsi_enable();
		goto out;
	}

	/* requeue buffers from active queue to pending queue */
	while (!list_empty(&mcsi->active_buf_q)) {
		vbuf = mcsi_active_queue_pop_tail(mcsi);
		mcsi_pending_queue_add_front(mcsi, vbuf);
	}

	if (!test_bit(ST_PREV_STREAM, &mcsi->state))
		goto out;

	mcsi_set_next_buf(mcsi);
	csp_mcsi_dma_capture_mode(MCSI_CAP_CONT);
	csp_mcsi_dma_start();
	csp_mcsi_enable();

	csp_mcsi_clr_pd(MCSI_DMA_INT_ALL_MASK);
	csp_mcsi_int_clear(MCSI_INT_DMA);

	mcsi_set_next_buf(mcsi);

out:
	csp_mcsi_dump_regs(__func__);
	PRT_TRACE_END("\n");
	return 0;
}
static void mcsi_isp_offline(struct viss_dev *mcsi)
{
	u32 intsrc = 0;

	intsrc = csp_mcsi_get_int_pd();
	csp_mcsi_clr_pd(MCSI_DMA_INT_ALL_MASK);
	csp_mcsi_int_clear(MCSI_INT_DMA);

	if ((intsrc & MCSI_DMA_INT_ALL_ERR) != 0) {

#ifdef CONFIG_ARCH_LOMBO_N7V1
		if (2 == mcsi_num) {
			if (intsrc & MCSI_DMA_INT_LB_OR_EN)
				PRT_ERR("MCSI_DMA_INT_OVER_RUN\n");
		}
#endif
		if (intsrc & MCSI_DMA_INT_P0_OVF)
			PRT_ERR("MCSI_DMA_INT_P0_OVF\n");

		if (intsrc & MCSI_DMA_INT_P1_OVF)
			PRT_ERR("MCSI_DMA_INT_P1_OVF\n");

		if (intsrc & MCSI_DMA_INT_P2_OVF)
			PRT_ERR("MCSI_DMA_INT_P2_OVF\n");

		return;
	}

	if (intsrc & MCSI_DMA_INT_FCI) {
		if (!list_empty(&mcsi->active_buf_q) &&
		    !list_is_singular(&mcsi->active_buf_q)) {
			offline_fifo_add(&mcsi->active_buf_q);
			mcsi->frame_count++;
		} else {
			mcsi->frames_lost_count++;
		}
		mcsi_set_next_buf_offline(mcsi);
		offline_fifo_process();
	}
}
static irqreturn_t mcsi_irq_handler(int irq, void *priv)
{
	struct viss_dev *mcsi = priv;
	struct viss_buffer *vbuf;
	unsigned long flags;
	struct timeval *tv;
	struct timeval tv_v;
	struct timespec ts;
	u32 intsrc;

	PRT_TRACE_BEGIN("\n");

	spin_lock_irqsave(&mcsi->slock, flags);

	if (isp_is_offline(mcsi->sensor)) {
		mcsi_isp_offline(mcsi);
		spin_unlock_irqrestore(&mcsi->slock, flags);
		PRT_TRACE_END("\n");
		return IRQ_HANDLED;
	}

	if (atomic_read(&mcsi->out_path) != VISS_IO_DMA)
		goto done;

	mod_timer(&mcsi->timer, jiffies + HZ / 10);
	mcsi->to_cnt = 0;

	intsrc = csp_mcsi_get_int_pd();
	csp_mcsi_clr_pd(MCSI_DMA_INT_ALL_MASK);
	csp_mcsi_int_clear(MCSI_INT_DMA);

	if (test_and_clear_bit(ST_PREV_OFF, &mcsi->state)) {
		wake_up(&mcsi->irq_queue);
		goto done;
	}

	if ((intsrc & MCSI_DMA_INT_ALL_ERR) != 0) {
		mcsi_restart(mcsi, false);

		clear_bit(ST_PREV_RUN, &mcsi->state);
#ifdef CONFIG_ARCH_LOMBO_N7V1
		if (2 == mcsi_num) {
			if (intsrc & MCSI_DMA_INT_LB_OR_EN)
				PRT_ERR("MCSI_DMA_INT_OVER_RUN\n");
		}
#endif
		if (intsrc & MCSI_DMA_INT_P0_OVF) {
			PRT_ERR("MCSI_DMA_INT_P0_OVF\n");
			mcsi->events[0].p0_overflow++;
		}

		if (intsrc & MCSI_DMA_INT_P1_OVF) {
			PRT_ERR("MCSI_DMA_INT_P1_OVF\n");
			mcsi->events[0].p1_overflow++;
		}

		if (intsrc & MCSI_DMA_INT_P2_OVF) {
			PRT_ERR("MCSI_DMA_INT_P2_OVF\n");
			mcsi->events[0].p2_overflow++;
		}

		goto out;
	}

	PRT_DBG("acitve_cnt=%d,pending_cnt=%d\n",
		mcsi->active_cnt, mcsi->pending_cnt);

	if ((intsrc & MCSI_DMA_INT_FCI) &&
		test_bit(ST_PREV_RUN, &mcsi->state) &&
		!list_empty(&mcsi->active_buf_q) &&
		!list_is_singular(&mcsi->active_buf_q)) {
		ktime_get_real_ts(&ts);

		/*
		 * the vbuf that store the current frame is in the active queue
		 * head, pop and yield it to user
		 */
		vbuf = mcsi_active_queue_pop(mcsi);
		tv = &vbuf->vb.v4l2_buf.timestamp;
		tv->tv_sec = ts.tv_sec;
		tv->tv_usec = ts.tv_nsec / NSEC_PER_USEC;
		vbuf->vb.v4l2_buf.sequence = mcsi->frame_count++;
		vb2_buffer_done(&vbuf->vb, VB2_BUF_STATE_DONE);
		if (mcsi->itv_frames != 0)
			mcsi_account_framerate(mcsi, tv);

		mcsi_set_next_buf(mcsi);
	}

	if (test_bit(ST_PREV_CONFIG, &mcsi->state))
		mcsi_config_update(mcsi);

	if (list_is_singular(&mcsi->active_buf_q)) {
		WARN_ON(!list_empty(&mcsi->pending_buf_q));
		/* will set next buffer in buf_queue when usr QBUF */
		mcsi->frames_lost_count++;
		ktime_get_real_ts(&ts);
		tv_v.tv_sec = ts.tv_sec;
		tv_v.tv_usec = ts.tv_nsec / NSEC_PER_USEC;
		if (mcsi->itv_frames != 0)
			mcsi_account_framerate(mcsi, &tv_v);
		PRT_DBG("frame lost\n");
	}

	PRT_DBG("active_cnt=%d,pending_cnt=%d\n",
		mcsi->active_cnt, mcsi->pending_cnt);
done:
	set_bit(ST_PREV_RUN, &mcsi->state);
out:
	spin_unlock_irqrestore(&mcsi->slock, flags);
	PRT_TRACE_END("\n");
	return IRQ_HANDLED;
}

static int start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct viss_dev *mcsi = q->drv_priv;
	unsigned long flags;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");

	spin_lock_irqsave(&mcsi->slock, flags);
	mcsi->frame_count = 0;
	ret = mcsi_hw_init(mcsi, false);
	if (ret) {
		spin_unlock_irqrestore(&mcsi->slock, flags);
		mcsi_state_cleanup(mcsi, false);
		return ret;
	}

	if (list_is_singular(&mcsi->pending_buf_q)) {
		PRT_ERR("Only one buffer, request more!\n");
		return -EINVAL;
	}

	ret = mcsi_start_capture(mcsi);
	spin_unlock_irqrestore(&mcsi->slock, flags);
	if (ret != 0)
		goto out;

	ret = viss_pipeline_call(mcsi, set_stream, &mcsi->pipeline, 1);
	if (ret != 0)
		goto out;

	mod_timer(&mcsi->timer, jiffies + HZ / 10);
	mcsi->to_cnt = 0;

out:
	csp_mcsi_dump_regs(__func__);
	PRT_TRACE_END("\n");
	return ret;
}

static int stop_streaming(struct vb2_queue *q)
{
	struct viss_dev *mcsi = q->drv_priv;
#ifdef CONFIG_ARCH_LOMBO_N7V1
	mcsi_num = 0;
#endif
	if (!mcsi_active(mcsi))
		return -EINVAL;

	PRT_TRACE_BEGIN("\n");
	return mcsi_stop_capture(mcsi, false);
}

static int queue_setup(struct vb2_queue *vq, const struct v4l2_format *pfmt,
		       unsigned int *num_buffers, unsigned int *num_planes,
		       unsigned int sizes[], void *allocators[])
{
	const struct v4l2_pix_format_mplane *pixm = NULL;
	struct viss_dev *mcsi = vq->drv_priv;
	struct viss_frame *frame = &mcsi->out_frame;
	const struct viss_fmt *fmt = frame->fmt;
	unsigned long wh;
	int i;

	PRT_TRACE_BEGIN("\n");
	if (pfmt) {
		pixm = &pfmt->fmt.pix_mp;
		fmt = mcsi_find_format(&pixm->pixelformat, NULL, 0, -1);
		wh = pixm->width * pixm->height;
	} else {
		wh = frame->f_width * frame->f_height;
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
		allocators[i] = mcsi->alloc_ctx;
	}

	PRT_TRACE_END("\n");
	return 0;
}

static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_queue *vq = vb->vb2_queue;
	struct viss_dev *mcsi = vq->drv_priv;
	int i;

	if (mcsi->out_frame.fmt == NULL)
		return -EINVAL;

	PRT_TRACE_BEGIN("\n");
	for (i = 0; i < mcsi->out_frame.fmt->memplanes; i++) {
		unsigned long size = mcsi->payload[i];

		if (vb2_plane_size(vb, i) < size) {
			v4l2_err(&mcsi->vfd,
				 "User buffer too small (%ld < %ld)\n",
				 vb2_plane_size(vb, i), size);
			return -EINVAL;
		}
		vb2_set_plane_payload(vb, i, size);
	}

	PRT_TRACE_END("\n");
	return 0;
}

/* The color format (colplanes, memplanes) must be already configured. */
int mcsi_prepare_addr(struct viss_dev *mcsi, struct vb2_buffer *vb,
		      struct viss_frame *frame, struct viss_addr *paddr)
{
	int ret = 0;
	u32 pix_size;

	if (vb == NULL || frame == NULL)
		return -EINVAL;

	PRT_TRACE_BEGIN("\n");
	pix_size = frame->f_width * frame->f_height;

	PRT_DBG("memplanes=%d,colplanes=%d,pix_size=%d",
		frame->fmt->memplanes, frame->fmt->colplanes, pix_size);

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
			paddr->cb = (u32)(paddr->y + pix_size);
			paddr->cr = 0;
			break;
		case 3:
			paddr->cb = (u32)(paddr->y + pix_size);
			/* decompose Y into Y/Cb/Cr */
			if (VISS_FMT_YCBCR420 == frame->fmt->color)
				paddr->cr = (u32)(paddr->cb
						+ (pix_size >> 2));
			else /* 422 */
				paddr->cr = (u32)(paddr->cb
						+ (pix_size >> 1));
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

	PRT_DBG("PHYS_ADDR: y=0x%X,cb=0x%X,cr=0x%X,ret=%d",
		paddr->y, paddr->cb, paddr->cr, ret);

	PRT_TRACE_END("\n");
	return ret;
}

static void buffer_queue(struct vb2_buffer *vb)
{
	struct viss_buffer *vbuf
		= container_of(vb, struct viss_buffer, vb);
	struct viss_dev *mcsi = vb2_get_drv_priv(vb->vb2_queue);
	unsigned long flags;

	PRT_TRACE_BEGIN("\n");
	spin_lock_irqsave(&mcsi->slock, flags);
	mcsi_prepare_addr(mcsi, &vbuf->vb, &mcsi->out_frame, &vbuf->paddr);
	mcsi_pending_queue_add(mcsi, vbuf);

	if (vb2_is_streaming(&mcsi->vb_queue) &&
		list_is_singular(&mcsi->active_buf_q))
		mcsi_set_next_buf(mcsi);

	spin_unlock_irqrestore(&mcsi->slock, flags);
	PRT_TRACE_END("\n");
}

static const struct vb2_ops mcsi_qops = {
	.queue_setup	 = queue_setup,
	.buf_prepare	 = buffer_prepare,
	.buf_queue	 = buffer_queue,
	.wait_prepare	 = vb2_ops_wait_prepare,
	.wait_finish	 = vb2_ops_wait_finish,
	.start_streaming = start_streaming,
	.stop_streaming	 = stop_streaming,
};

static void mcsi_clear_event_counters(struct viss_dev *mcsi)
{
	unsigned long flags;

	PRT_TRACE_BEGIN("\n");
	spin_lock_irqsave(&mcsi->slock, flags);
	memset(mcsi->events, 0, sizeof(mcsi->events));
	spin_unlock_irqrestore(&mcsi->slock, flags);
	PRT_TRACE_END("\n");
}

static int mcsi_open(struct file *file)
{
	struct viss_dev *mcsi = video_drvdata(file);
	struct media_entity *me = &mcsi->vfd.entity;
	int ret;

	PRT_TRACE_BEGIN("\n");
	mutex_lock(&me->parent->graph_mutex);

	mutex_lock(&mcsi->lock);
	if (atomic_read(&mcsi->out_path) != VISS_IO_DMA) {
		ret = -EBUSY;
		goto unlock;
	}

	set_bit(ST_PREV_IN_USE, &mcsi->state);
	ret = pm_runtime_get_sync(&mcsi->pdev->dev);
	if (ret < 0)
		goto unlock;

	ret = v4l2_fh_open(file);
	if (ret != 0) {
		pm_runtime_put_sync(&mcsi->pdev->dev);
		goto unlock;
	}

	if (v4l2_fh_is_singular_file(file) &&
		(atomic_read(&mcsi->out_path) == VISS_IO_DMA)) {
		ret = viss_pipeline_call(mcsi, open, &mcsi->pipeline,
					 &mcsi->vfd.entity, true);
		if (ret < 0) {
			clear_bit(ST_PREV_IN_USE, &mcsi->state);
			pm_runtime_put_sync(&mcsi->pdev->dev);
			v4l2_fh_release(file);
		} else {
			mcsi_clear_event_counters(mcsi);
			mcsi->ref_count++;
		}
	}

unlock:
	mutex_unlock(&mcsi->lock);
	mutex_unlock(&me->parent->graph_mutex);
	PRT_TRACE_END("\n");
	return ret;
}

static int mcsi_release(struct file *file)
{
	struct viss_dev *mcsi = video_drvdata(file);

	PRT_TRACE_BEGIN("\n");
	csp_mcsi_dump_regs(__func__);
	mutex_lock(&mcsi->lock);

	if (v4l2_fh_is_singular_file(file) &&
	    atomic_read(&mcsi->out_path) == VISS_IO_DMA) {
		if (mcsi->streaming) {
			media_entity_pipeline_stop(&mcsi->vfd.entity);
			mcsi->streaming = false;
		}
		clear_bit(ST_PREV_IN_USE, &mcsi->state);
		mcsi_stop_capture(mcsi, false);
		viss_pipeline_call(mcsi, close, &mcsi->pipeline);
		mcsi->ref_count--;
	}

	vb2_fop_release(file);
	pm_runtime_put(&mcsi->pdev->dev);
	clear_bit(ST_PREV_SUSPENDED, &mcsi->state);

	mutex_unlock(&mcsi->lock);
	PRT_TRACE_END("\n");
	return 0;
}

static const struct v4l2_file_operations mcsi_fops = {
	.owner		= THIS_MODULE,
	.open		= mcsi_open,
	.release	= mcsi_release,
	.poll		= vb2_fop_poll,
	.unlocked_ioctl	= video_ioctl2,
	.mmap		= vb2_fop_mmap,
};

/*
 * Format and crop negotiation helpers
 */

static const struct viss_fmt *mcsi_try_format(struct viss_dev *mcsi,
					u32 *width, u32 *height,
					u32 *code, u32 *fourcc, int pad)
{
	struct viss_drvdata *dd = mcsi->dd;
	const struct viss_fmt *fmt;
	unsigned int flags = 0;

	PRT_TRACE_BEGIN("pad=%d\n", pad);
	if (width != NULL)
		PRT_DBG("*width=%d\n", *width);
	if (height != NULL)
		PRT_DBG("*height=%d\n", *height);
	if (code != NULL)
		PRT_DBG("*code=0x%x\n", *code);
	if (fourcc != NULL)
		PRT_DBG("*fourcc=0x%x\n", *fourcc);
	PRT_DBG("pad=%d\n", pad);

#if 0
	if (pad == VISS_SD_PAD_SINK) {
		v4l_bound_align_image(width, 8, dd->max_width,
				      ffs(dd->out_width_align) - 1,
				      height, 0, dd->max_height, 0, 0);
	} else {
		v4l_bound_align_image(width, 8, mcsi->inp_frame.rect.width,
				      ffs(dd->out_width_align) - 1,
				      height, 0, mcsi->inp_frame.rect.height,
				      0, 0);
		flags = mcsi->inp_frame.fmt->flags;
	}
#else
	v4l_bound_align_image(width, 8, dd->max_width,
			      ffs(dd->out_width_align) - 1,
			      height, 0, dd->max_height, 0, 0);
	if (pad != VISS_SD_PAD_SINK)
		flags = mcsi->inp_frame.fmt->flags;
#endif

	fmt = mcsi_find_format(fourcc, code, flags, 0);
	if (WARN_ON(!fmt))
		return NULL;

	if (code)
		*code = fmt->mbus_code;
	if (fourcc) {
		*fourcc = fmt->fourcc;
		/* treat RAW8 as NV12 temporary */
		if (fmt->fourcc == V4L2_PIX_FMT_SGRBG8)
			*fourcc = V4L2_PIX_FMT_NV12;
	}

	PRT_DBG("code: 0x%x, %dx%d\n", code ? *code : 0, *width, *height);

	PRT_TRACE_END("\n");
	return fmt;
}

static void mcsi_try_crop(struct viss_dev *mcsi, struct v4l2_rect *r)
{
	struct viss_frame *frame = &mcsi->inp_frame;

	PRT_TRACE_BEGIN("\n");
	v4l_bound_align_image(&r->width, 0, frame->f_width, 0,
			      &r->height, 0, frame->f_height, 0, 0);

	/* Adjust left/top if cropping rectangle got out of bounds */
	r->left = clamp_t(u32, r->left, 0, frame->f_width - r->width);
	r->left = round_down(r->left, mcsi->dd->win_hor_offs_align);
	r->top  = clamp_t(u32, r->top, 0, frame->f_height - r->height);

	PRT_DBG("(%d,%d)/%dx%d, sink fmt: %dx%d\n",
		 r->left, r->top, r->width, r->height,
		 frame->f_width, frame->f_height);
	PRT_TRACE_END("\n");
}

static void mcsi_try_compose(struct viss_dev *mcsi, struct v4l2_rect *r)
{
	struct viss_frame *frame = &mcsi->out_frame;
	struct v4l2_rect *crop_rect = &mcsi->inp_frame.rect;

	PRT_TRACE_BEGIN("\n");
	/* Scaling is not supported so we enforce compose rectangle size
	   same as size of the sink crop rectangle. */
	r->width = crop_rect->width;
	r->height = crop_rect->height;

	/* TODO */

	/* Adjust left/top if the composing rectangle got out of bounds */
	r->left = clamp_t(u32, r->left, 0, frame->f_width - r->width);
	r->left = round_down(r->left, mcsi->dd->out_hor_offs_align);
	r->top  = clamp_t(u32, r->top, 0, mcsi->out_frame.f_height - r->height);

	PRT_DBG("(%d,%d)/%dx%d, source fmt: %dx%d\n",
		r->left, r->top, r->width, r->height,
		frame->f_width, frame->f_height);
	PRT_TRACE_END("\n");
}

/*
 * Video node ioctl operations
 */
static int viss_vidioc_querycap_capture(struct file *file, void *priv,
					struct v4l2_capability *cap)
{
	struct viss_dev *mcsi = video_drvdata(file);
	struct device *dev = &mcsi->pdev->dev;
	unsigned int caps;

	PRT_TRACE_BEGIN("\n");
	strlcpy(cap->driver, dev->driver->name, sizeof(cap->driver));
	strlcpy(cap->card, dev->driver->name, sizeof(cap->card));
	snprintf(cap->bus_info, sizeof(cap->bus_info),
				"platform:%s", dev_name(dev));
	caps = V4L2_CAP_STREAMING | V4L2_CAP_VIDEO_CAPTURE_MPLANE;
	cap->device_caps = caps;
	cap->capabilities = caps | V4L2_CAP_DEVICE_CAPS;

	PRT_TRACE_END("\n");
	return 0;
}

static int mcsi_enum_fmt_mplane(struct file *file, void *priv,
				     struct v4l2_fmtdesc *f)
{
	const struct viss_fmt *fmt;

	if (f->index >= ARRAY_SIZE(mcsi_formats))
		return -EINVAL;

	PRT_TRACE_BEGIN("\n");
	fmt = &mcsi_formats[f->index];
	strlcpy(f->description, fmt->name, sizeof(f->description));
	f->pixelformat = fmt->fourcc;

	PRT_TRACE_END("\n");
	return 0;
}

static int mcsi_g_fmt_mplane(struct file *file, void *fh,
				  struct v4l2_format *f)
{
	struct viss_dev *mcsi = video_drvdata(file);
	struct v4l2_pix_format_mplane *pixm = &f->fmt.pix_mp;
	struct viss_frame *frame = &mcsi->out_frame;
	const struct viss_fmt *fmt = frame->fmt;
	int i;

	PRT_TRACE_BEGIN("\n");
	pixm->num_planes = fmt->memplanes;
	pixm->pixelformat = fmt->fourcc;
	pixm->width = frame->f_width;
	pixm->height = frame->f_height;
	pixm->field = V4L2_FIELD_NONE;
	pixm->colorspace = V4L2_COLORSPACE_JPEG;

	for (i = 0; i < pixm->num_planes; ++i) {
		pixm->plane_fmt[i].bytesperline = mcsi->bytesperline[i];
		pixm->plane_fmt[i].sizeimage = mcsi->payload[i];
	}
	PRT_TRACE_END("\n");
	return 0;
}

/**
 * viss_pipeline_try_format - negotiate and/or set formats at pipeline
 *                            elements
 * @viss: VISS device structure
 * @tfmt: media bus format to try/set on subdevs
 * @fmt_id: viss pixel format id corresponding to returned @tfmt (output)
 * @set: true to set format on subdevs, false to try only
 */
static int viss_pipeline_try_format(struct viss_dev *mcsi,
				    struct v4l2_mbus_framefmt *tfmt,
				    const struct viss_fmt **fmt_id,
				    bool set)
{
	struct v4l2_subdev *sd = mcsi->pipeline.subdevs[IDX_SENSOR];
	struct v4l2_subdev_format sfmt;
	struct v4l2_mbus_framefmt *mf = &sfmt.format;
	struct media_entity *me;
	const struct viss_fmt *vfmt;
	struct media_pad *pad;
	int ret, i = 1;
	u32 fcc;

	if (WARN_ON(!sd || !tfmt))
		return -EINVAL;

	PRT_TRACE_BEGIN("tfmt->code=0x%x,tfmt->width=%d,tfmt->height=%d\n",
			tfmt->code, tfmt->width, tfmt->height);
	memset(&sfmt, 0, sizeof(sfmt));
	sfmt.format = *tfmt;
	sfmt.which = set ? V4L2_SUBDEV_FORMAT_ACTIVE : V4L2_SUBDEV_FORMAT_TRY;

	me = viss_pipeline_get_head(&sd->entity);

	/*
	 * assume that each subdev prior the mcsi->subdev have only one source
	 */

	while (1) {
		/* formats that sensor may suport have flag FMT_FLAGS_CAM */
		vfmt = mcsi_find_format(NULL, mf->code != 0 ? &mf->code : NULL,
					FMT_FLAGS_CAM, i++);
		if (vfmt == NULL) {
			/*
			 * Notify user-space if common pixel code for
			 * host and sensor does not exist.
			 */
			return -EINVAL;
		}
		mf->code = tfmt->code = vfmt->mbus_code;

		/* set format on all pipeline subdevs */
		while (me != &mcsi->subdev.entity) {
			sd = media_entity_to_v4l2_subdev(me);

			PRT_DBG("sd->name=%s\n", sd->name);
			sfmt.pad = 0;
			ret = v4l2_subdev_call(sd, pad, set_fmt, NULL, &sfmt);
			if (ret)
				return ret;

			if (me->pads[0].flags & MEDIA_PAD_FL_SINK) {
				PRT_DBG("\n");
				sfmt.pad = me->num_pads - 1;
				mf->code = tfmt->code;
				ret = v4l2_subdev_call(sd, pad, set_fmt, NULL,
									&sfmt);
				if (ret)
					return ret;
			}

			pad = media_entity_remote_source(&me->pads[sfmt.pad]);
			if (!pad)
				return -EINVAL;
			me = pad->entity;
		}

		if (mf->code != tfmt->code) {
			PRT_DBG("mf->code=0x%x,tfmt->code=0x%x\n",
				mf->code, tfmt->code);
			continue;
		}

		fcc = vfmt->fourcc;
		tfmt->width  = mf->width;
		tfmt->height = mf->height;
		vfmt = mcsi_try_format(mcsi, &tfmt->width, &tfmt->height,
					NULL, &fcc, VISS_SD_PAD_SINK);
		vfmt = mcsi_try_format(mcsi, &tfmt->width, &tfmt->height,
					NULL, &fcc, VISS_SD_PAD_SOURCE_DMA);
		if (vfmt && vfmt->mbus_code)
			mf->code = vfmt->mbus_code;
		if (mf->width != tfmt->width || mf->height != tfmt->height) {
			PRT_DBG("mf->width=%d,mf->height=%d\n",
				mf->width, mf->height);
			PRT_DBG("tfmt->width=%d,tfmt->height=%d\n",
				tfmt->width, tfmt->height);
			continue;
		}
		tfmt->code = mf->code;
		break;
	}

	if (fmt_id && vfmt)
		*fmt_id = vfmt;
	*tfmt = *mf;

	PRT_TRACE_END("\n");
	return 0;
}

/**
 * viss_adjust_mplane_format - adjust bytesperline/sizeimage for each plane
 * @fmt: viss pixel format description (input)
 * @width: requested pixel width
 * @height: requested pixel height
 * @pix: multi-plane format to adjust
 */
static void viss_adjust_mplane_format(const struct viss_fmt *fmt, u32 width,
				u32 height, struct v4l2_pix_format_mplane *pix)
{
	u32 bytesperline = 0;
	int i;

	PRT_TRACE_BEGIN("\n");
	pix->colorspace	= V4L2_COLORSPACE_JPEG;
	pix->field = V4L2_FIELD_NONE;
	pix->num_planes = fmt->memplanes;
	pix->pixelformat = fmt->fourcc;
	pix->height = height;
	pix->width = width;

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
		 * V4L2_PIX_FMT_YUV420M and V4L2_PIX_FMT_YVU420M format.
		 * This calculation may need to e changed when other
		 * multi-planar formats are added to the mcsi_formats[]
		 * array.
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

static int mcsi_try_fmt_mplane(struct file *file, void *fh,
				   struct v4l2_format *f)
{
	struct v4l2_pix_format_mplane *pix = &f->fmt.pix_mp;
	struct viss_dev *mcsi = video_drvdata(file);
	const struct viss_fmt *inp_fmt = mcsi->inp_frame.fmt;
	struct v4l2_mbus_framefmt mf;
	const struct viss_fmt *vfmt = NULL;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");
	viss_md_graph_lock(mcsi);
	mutex_lock(&mcsi->lock);

	/*
	 * In case of the format that have flag FMT_FLAGS_CAM output format
	 * must match its camera interface input format.
	 */
	if (inp_fmt->flags & FMT_FLAGS_CAM)
		vfmt = inp_fmt;
	else {
		vfmt = mcsi_try_format(mcsi, &pix->width, &pix->height,
					NULL, &pix->pixelformat,
					VISS_SD_PAD_SOURCE_DMA);
	}

	if (!vfmt) {
		ret = -EINVAL;
		goto unlock;
	}

	if (!mcsi->user_subdev_api) {
		mf.width = pix->width;
		mf.height = pix->height;
		mf.code = vfmt->mbus_code;
		viss_pipeline_try_format(mcsi, &mf, &vfmt, false);
		pix->width = mf.width;
		pix->height = mf.height;
		if (vfmt)
			pix->pixelformat = vfmt->fourcc;
	}

	viss_adjust_mplane_format(vfmt, pix->width, pix->height, pix);

unlock:
	mutex_unlock(&mcsi->lock);
	viss_md_graph_unlock(mcsi);

	PRT_TRACE_END("\n");
	return ret;
}

static int __mcsi_set_format(struct viss_dev *mcsi,
				     struct v4l2_format *f)
{
	struct v4l2_pix_format_mplane *pixm = &f->fmt.pix_mp;
	struct v4l2_mbus_framefmt *mf = &mcsi->ci_fmt;
	struct viss_frame *ff = &mcsi->out_frame;
	struct viss_frame *inf = &mcsi->inp_frame;
	const struct viss_fmt *s_fmt = NULL;
	int ret, i;

	if (vb2_is_busy(&mcsi->vb_queue))
		return -EBUSY;

	PRT_TRACE_BEGIN("\n");
	/* Try the format at the DMA output */
	ff->fmt = mcsi_try_format(mcsi, &pixm->width, &pixm->height,
				 NULL, &pixm->pixelformat,
				 VISS_SD_PAD_SOURCE_DMA);
	if (!ff->fmt)
		return -EINVAL;

	/* Try to match format at the host and the sensor */
	if (!mcsi->user_subdev_api) {
		mf->code   = ff->fmt->mbus_code;
		mf->width  = pixm->width;
		mf->height = pixm->height;
		ret = viss_pipeline_try_format(mcsi, mf, &s_fmt, true);
		if (ret)
			return ret;

		pixm->width  = mf->width;
		pixm->height = mf->height;
	}

	viss_adjust_mplane_format(ff->fmt, pixm->width, pixm->height, pixm);

	mcsi->bpl = 0;
	for (i = 0; i < ff->fmt->memplanes; i++) {
		mcsi->bytesperline[i] = pixm->plane_fmt[i].bytesperline;
		mcsi->payload[i] = pixm->plane_fmt[i].sizeimage;
		mcsi->bpl += pixm->plane_fmt[i].bytesperline;
	}

	ff->f_width = pixm->width;
	ff->f_height = pixm->height;

	inf->rect.left = 0;
	inf->rect.top = 0;
	inf->rect.width = pixm->width;
	inf->rect.height = pixm->height;

	/* Reset cropping and set format at the camera interface input */
	if (!mcsi->user_subdev_api) {
		mcsi->inp_frame.fmt = s_fmt;
		mcsi->inp_frame.f_width = pixm->width;
		mcsi->inp_frame.f_height = pixm->height;
		mcsi->inp_frame.rect.left = 0;
		mcsi->inp_frame.rect.top = 0;
	}

	PRT_TRACE_END("\n");
	return ret;
}

static int mcsi_s_fmt_mplane(struct file *file, void *priv,
				  struct v4l2_format *f)
{
	struct viss_dev *mcsi = video_drvdata(file);
	int ret;

	PRT_TRACE_BEGIN("\n");
	viss_md_graph_lock(mcsi);
	mutex_lock(&mcsi->lock);
	/*
	 * The graph is walked within __mcsi_set_format() to set
	 * the format at subdevs thus the graph mutex needs to be held at
	 * this point and acquired before the video mutex, to avoid  AB-BA
	 * deadlock when viss_md_link_notify() is called by other thread.
	 * Ideally the graph walking and setting format at the whole pipeline
	 * should be removed from this driver and handled in userspace only.
	 */
	ret = __mcsi_set_format(mcsi, f);

	mutex_unlock(&mcsi->lock);
	viss_md_graph_unlock(mcsi);
	PRT_TRACE_END("\n");
	return ret;
}

static int mcsi_enum_input(struct file *file, void *priv,
			       struct v4l2_input *i)
{
	struct viss_dev *mcsi = video_drvdata(file);
	struct v4l2_subdev *sd = mcsi->pipeline.subdevs[IDX_SENSOR];

	/* TODO: support up to 4 inputs */

	if (i->index != 0)
		return -EINVAL;

	PRT_TRACE_BEGIN("\n");
	i->type = V4L2_INPUT_TYPE_CAMERA;
	if (sd)
		strlcpy(i->name, sd->name, sizeof(i->name));
	PRT_TRACE_END("\n");
	return 0;
}

static int mcsi_s_input(struct file *file, void *priv, unsigned int i)
{
	PRT_TRACE_BEGIN("\n");
	/* TODO: support up to 4 inputs */
	return i == 0 ? i : -EINVAL;
}

static int mcsi_g_input(struct file *file, void *priv, unsigned int *i)
{
	PRT_TRACE_BEGIN("\n");
	*i = 0;
	PRT_TRACE_END("\n");
	return 0;
}

static int mcsi_enum_framesizes(struct file *file, void *fh,
				struct v4l2_frmsizeenum *fsize)
{
	struct viss_dev *mcsi = video_drvdata(file);
	struct v4l2_subdev *sd = mcsi->pipeline.subdevs[IDX_SENSOR];

	PRT_DBG("sd->name=%s\n", sd->name);
	return v4l2_subdev_call(sd, video, enum_framesizes, fsize);
}

static int mcsi_enum_frameintervals(struct file *file, void *fh,
				struct v4l2_frmivalenum *fival)
{
	struct viss_dev *mcsi = video_drvdata(file);
	struct v4l2_subdev *sd = mcsi->pipeline.subdevs[IDX_SENSOR];

	PRT_DBG("sd->name=%s\n", sd->name);
	return v4l2_subdev_call(sd, video, enum_frameintervals, fival);
}

static int mcsi_g_parm(struct file *file, void *fh,
			struct v4l2_streamparm *a)
{
	struct viss_dev *mcsi = video_drvdata(file);
	struct v4l2_subdev *sd = mcsi->pipeline.subdevs[IDX_SENSOR];

	PRT_DBG("sd->name=%s\n", sd->name);
	return v4l2_subdev_call(sd, video, g_parm, a);
}

static int mcsi_s_parm(struct file *file, void *fh,
			struct v4l2_streamparm *a)
{
	struct viss_dev *mcsi = video_drvdata(file);
	struct v4l2_subdev *sd = mcsi->pipeline.subdevs[IDX_SENSOR];

	PRT_DBG("sd->name=%s\n", sd->name);
	return v4l2_subdev_call(sd, video, s_parm, a);
}

static int viss_pipeline_validate(struct viss_dev *mcsi)
{
	struct v4l2_subdev *sd = &mcsi->subdev;
	struct v4l2_subdev_format sink_fmt, src_fmt;
	struct media_pad *sink_pad, *src_pad;
	int i, ret;

	PRT_TRACE_BEGIN("\n");
	while (1) {
		/*
		 * Find current entity sink pad and any remote source pad linked
		 * to it. We stop if there is no sink pad in current entity or
		 * it is not linked to any other remote entity.
		 */
		src_pad = NULL;

		for (i = 0; i < sd->entity.num_pads; i++) {
			struct media_pad *p = &sd->entity.pads[i];

			if (p->flags & MEDIA_PAD_FL_SINK) {
				sink_pad = p;
				src_pad = media_entity_remote_source(sink_pad);
				if (src_pad)
					break;
			}
		}

		if (src_pad == NULL ||
		    (media_entity_type(src_pad->entity) !=
		     MEDIA_ENT_T_V4L2_SUBDEV))
			break;

		/*
		 * Don't call MIPI-CSI subdev operation to
		 * avoid nested locking
		 */
		if (sd == &mcsi->subdev) {
#if 0
			struct viss_frame *ff = &mcsi->out_frame;
			sink_fmt.format.width = ff->f_width;
			sink_fmt.format.height = ff->f_height;
			sink_fmt.format.code = mcsi->inp_frame.fmt->mbus_code;
#endif
			struct viss_frame *ff = &mcsi->inp_frame;
			sink_fmt.format.width = ff->f_width;
			sink_fmt.format.height = ff->f_height;
			sink_fmt.format.code = ff->fmt->mbus_code;

		} else {
			sink_fmt.pad = sink_pad->index;
			sink_fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
			ret = v4l2_subdev_call(sd, pad, get_fmt, NULL,
					       &sink_fmt);
			if (ret < 0 && ret != -ENOIOCTLCMD)
				return -EPIPE;
		}

		sd = media_entity_to_v4l2_subdev(src_pad->entity);
		src_fmt.pad = src_pad->index;
		src_fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
		ret = v4l2_subdev_call(sd, pad, get_fmt, NULL, &src_fmt);
		if (ret < 0 && ret != -ENOIOCTLCMD)
			return -EPIPE;

		if (src_fmt.format.width != sink_fmt.format.width ||
		    src_fmt.format.height != sink_fmt.format.height ||
		    src_fmt.format.code != sink_fmt.format.code)
			return -EPIPE;
	}
	PRT_TRACE_END("\n");
	return 0;
}

static int mcsi_streamon(struct file *file, void *priv,
			      enum v4l2_buf_type type)
{
	struct viss_dev *mcsi = video_drvdata(file);
	struct media_entity *entity = &mcsi->vfd.entity;
	struct viss_pipeline *p = &mcsi->pipeline;
	int ret;

	if (mcsi_active(mcsi))
		return -EBUSY;

	PRT_TRACE_BEGIN("\n");
	ret = media_entity_pipeline_start(entity, p->m_pipeline);
	if (ret < 0)
		return ret;

	if (mcsi->user_subdev_api) {
		ret = viss_pipeline_validate(mcsi);
		if (ret < 0)
			goto err_p_stop;
	}

	mcsi->sensor = __find_remote_sensor(&mcsi->subdev.entity);

	ret = vb2_ioctl_streamon(file, priv, type);
	if (ret == 0) {
		mcsi->streaming = true;
		PRT_TRACE_END("\n");
		return ret;
	}

err_p_stop:
	media_entity_pipeline_stop(entity);
	return ret;
}

static int mcsi_streamoff(struct file *file, void *priv,
			       enum v4l2_buf_type type)
{
	struct viss_dev *mcsi = video_drvdata(file);
	int ret;

	PRT_TRACE_BEGIN("\n");
	ret = vb2_ioctl_streamoff(file, priv, type);
	if (ret < 0)
		return ret;

	media_entity_pipeline_stop(&mcsi->vfd.entity);
	mcsi->streaming = false;
	PRT_TRACE_END("\n");
	return 0;
}

static int mcsi_reqbufs(struct file *file, void *priv,
			     struct v4l2_requestbuffers *reqbufs)
{
	struct viss_dev *mcsi = video_drvdata(file);
	int ret;

	PRT_TRACE_BEGIN("\n");
	reqbufs->count = max_t(u32, VISS_REQ_BUFS_MIN, reqbufs->count);
	ret = vb2_ioctl_reqbufs(file, priv, reqbufs);
	if (!ret)
		mcsi->reqbufs_count = reqbufs->count;

	PRT_TRACE_END("\n");
	return ret;
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

static int mcsi_g_selection(struct file *file, void *fh,
				 struct v4l2_selection *sel)
{
	struct viss_dev *mcsi = video_drvdata(file);
	struct viss_frame *f = &mcsi->out_frame;
	struct viss_frame *inf = &mcsi->inp_frame;

	if (sel->type != V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE)
		return -EINVAL;

	PRT_TRACE_BEGIN("\n");

	switch (sel->target) {
	case V4L2_SEL_TGT_COMPOSE_BOUNDS:
	case V4L2_SEL_TGT_COMPOSE_DEFAULT:
		sel->r.left = 0;
		sel->r.top = 0;
		sel->r.width = f->f_width;
		sel->r.height = f->f_height;
		return 0;
	case V4L2_SEL_TGT_CROP_DEFAULT:
		sel->r.left = 0;
		sel->r.top = 0;
		sel->r.width = inf->f_width;
		sel->r.height = inf->f_height;
		return 0;
	case V4L2_SEL_TGT_COMPOSE:
		sel->r = f->rect;
		return 0;
	case V4L2_SEL_TGT_CROP:
		sel->r = inf->rect;
		return 0;
	}

	return -EINVAL;
}

static int mcsi_s_selection(struct file *file, void *fh,
				 struct v4l2_selection *sel)
{
	struct viss_dev *mcsi = video_drvdata(file);
	struct v4l2_rect rect = sel->r;
	struct viss_frame *f;
	unsigned long flags;

	if (sel->type != V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE)
		return -EINVAL;

	if (sel->target == V4L2_SEL_TGT_COMPOSE) {
		mcsi_try_compose(mcsi, &rect);
		f = &mcsi->out_frame;
	} else if (sel->target == V4L2_SEL_TGT_CROP) {
		mcsi_try_crop(mcsi, &rect);
		f = &mcsi->inp_frame;
	} else
		return -EINVAL;

	PRT_TRACE_BEGIN("\n");

	if ((sel->flags & V4L2_SEL_FLAG_LE) &&
	    !enclosed_rectangle(&rect, &sel->r))
		return -ERANGE;

	if ((sel->flags & V4L2_SEL_FLAG_GE) &&
	    !enclosed_rectangle(&sel->r, &rect))
		return -ERANGE;

	sel->r = rect;
	spin_lock_irqsave(&mcsi->slock, flags);
	f->rect = rect;
	set_bit(ST_PREV_CONFIG, &mcsi->state);
	spin_unlock_irqrestore(&mcsi->slock, flags);

	PRT_TRACE_END("\n");
	return 0;
}

static int mcsi_get_sensor_dev(struct file *file, void *fh,
				 int *arg)
{
	struct viss_dev *mcsi = video_drvdata(file);
	struct v4l2_subdev *sd = mcsi->pipeline.subdevs[IDX_SENSOR];

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

static long mcsi_default(struct file *file, void *fh,
		bool valid_prio, unsigned int cmd, void *arg)
{
	int ret = 0;

	switch (cmd) {
	case VIDIOC_VISS_GET_SENSOR_DEV:
		ret = mcsi_get_sensor_dev(file, fh, arg);
		break;
	default:
		return -ENOIOCTLCMD;
	}

	return ret;
}

static const struct v4l2_ioctl_ops mcsi_ioctl_ops = {
	.vidioc_querycap		= viss_vidioc_querycap_capture,

	.vidioc_enum_fmt_vid_cap_mplane	= mcsi_enum_fmt_mplane,
	.vidioc_try_fmt_vid_cap_mplane	= mcsi_try_fmt_mplane,
	.vidioc_s_fmt_vid_cap_mplane	= mcsi_s_fmt_mplane,
	.vidioc_g_fmt_vid_cap_mplane	= mcsi_g_fmt_mplane,

	.vidioc_enum_framesizes		= mcsi_enum_framesizes,
	.vidioc_enum_frameintervals	= mcsi_enum_frameintervals,

	.vidioc_g_parm			= mcsi_g_parm,
	.vidioc_s_parm			= mcsi_s_parm,

	.vidioc_reqbufs			= mcsi_reqbufs,
	.vidioc_querybuf		= vb2_ioctl_querybuf,
	.vidioc_prepare_buf		= vb2_ioctl_prepare_buf,
	.vidioc_create_bufs		= vb2_ioctl_create_bufs,
	.vidioc_qbuf			= vb2_ioctl_qbuf,
	.vidioc_dqbuf			= vb2_ioctl_dqbuf,
	.vidioc_expbuf			= vb2_ioctl_expbuf,

	.vidioc_streamon		= mcsi_streamon,
	.vidioc_streamoff		= mcsi_streamoff,

	.vidioc_g_selection		= mcsi_g_selection,
	.vidioc_s_selection		= mcsi_s_selection,

	.vidioc_enum_input		= mcsi_enum_input,
	.vidioc_s_input			= mcsi_s_input,
	.vidioc_g_input			= mcsi_g_input,

	.vidioc_default			= mcsi_default,
};

/* Capture subdev media entity operations */
static int mcsi_link_setup(struct media_entity *entity,
				const struct media_pad *local,
				const struct media_pad *remote, u32 flags)
{
	struct v4l2_subdev *sd = media_entity_to_v4l2_subdev(entity);
	struct viss_dev *mcsi = v4l2_get_subdevdata(sd);
	unsigned int remote_ent_type = media_entity_type(remote->entity);
	int ret = 0;

	if (WARN_ON(mcsi == NULL))
		return 0;

	PRT_TRACE_BEGIN("\n");
	PRT_DBG("%s --> %s, flags: 0x%x. source_id: 0x%x, local->index: %u.\n",
		 remote->entity->name, local->entity->name,
		 flags, mcsi->source_subdev_grp_id, local->index);

	mutex_lock(&mcsi->lock);

	switch (local->index) {
	case VISS_SD_PAD_SINK:
		if (remote_ent_type != MEDIA_ENT_T_V4L2_SUBDEV) {
			ret = -EINVAL;
			break;
		}
		if (flags & MEDIA_LNK_FL_ENABLED) {
			if (mcsi->source_subdev_grp_id == 0)
				mcsi->source_subdev_grp_id = sd->grp_id;
			else
				ret = -EBUSY;
		} else {
			mcsi->source_subdev_grp_id = 0;
			mcsi->sensor = NULL;
		}
		break;

	case VISS_SD_PAD_SOURCE_DMA:
		if (!(flags & MEDIA_LNK_FL_ENABLED))
			atomic_set(&mcsi->out_path, VISS_IO_NONE);
		else if (remote_ent_type == MEDIA_ENT_T_DEVNODE)
			atomic_set(&mcsi->out_path, VISS_IO_DMA);
		else
			ret = -EINVAL;
		break;

	case VISS_SD_PAD_SOURCE_ISP:
		if (!(flags & MEDIA_LNK_FL_ENABLED))
			atomic_set(&mcsi->out_path, VISS_IO_NONE);
		else if (remote_ent_type == MEDIA_ENT_T_V4L2_SUBDEV)
			atomic_set(&mcsi->out_path, VISS_IO_ISP);
		else
			ret = -EINVAL;
		break;

	default:
		v4l2_err(sd, "Invalid pad index\n");
		ret = -EINVAL;
	}
	mb();

	mutex_unlock(&mcsi->lock);
	PRT_TRACE_END("\n");
	return ret;
}

static const struct media_entity_operations mcsi_subdev_media_ops = {
	.link_setup = mcsi_link_setup,
};

static int mcsi_subdev_enum_mbus_code(struct v4l2_subdev *sd,
				struct v4l2_subdev_fh *fh,
				struct v4l2_subdev_mbus_code_enum *code)
{
	const struct viss_fmt *fmt;

	PRT_TRACE_BEGIN("\n");
	fmt = mcsi_find_format(NULL, NULL, 0, code->index);
	if (!fmt)
		return -EINVAL;
	code->code = fmt->mbus_code;
	PRT_TRACE_END("\n");
	return 0;
}

static int mcsi_subdev_get_fmt(struct v4l2_subdev *sd,
				    struct v4l2_subdev_fh *fh,
				    struct v4l2_subdev_format *fmt)
{
	struct viss_dev *mcsi = v4l2_get_subdevdata(sd);
	struct v4l2_mbus_framefmt *mf = &fmt->format;
	struct viss_frame *f = &mcsi->inp_frame;

	PRT_TRACE_BEGIN("\n");
	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		mf = v4l2_subdev_get_try_format(fh, fmt->pad);
		fmt->format = *mf;
		PRT_TRACE_END("\n");
		return 0;
	}

	mutex_lock(&mcsi->lock);

	if (fmt->pad == VISS_SD_PAD_SINK) {
		*mf = mcsi->ci_fmt;
	} else {
		mf->code = f->fmt->mbus_code;
		/* crop size */
		mf->width = f->rect.width;
		mf->height = f->rect.height;
	}

	mf->colorspace = V4L2_COLORSPACE_JPEG;
	mutex_unlock(&mcsi->lock);
	PRT_TRACE_END("\n");
	return 0;
}

static int mcsi_subdev_set_fmt(struct v4l2_subdev *sd,
				    struct v4l2_subdev_fh *fh,
				    struct v4l2_subdev_format *fmt)
{
	struct viss_dev *mcsi = v4l2_get_subdevdata(sd);
	struct v4l2_mbus_framefmt *mf = &fmt->format;
	struct viss_frame *sink = &mcsi->inp_frame;
	struct viss_frame *source = &mcsi->out_frame;
	const struct viss_fmt *vfmt;

	PRT_DBG("pad%d: code: 0x%x, %dx%d\n",
		fmt->pad, mf->code, mf->width, mf->height);

	PRT_TRACE_BEGIN("\n");
	mf->colorspace = V4L2_COLORSPACE_JPEG;
	mutex_lock(&mcsi->lock);

	if ((atomic_read(&mcsi->out_path) == VISS_IO_ISP &&
	    sd->entity.stream_count > 0) ||
	    (atomic_read(&mcsi->out_path) == VISS_IO_DMA &&
	    vb2_is_busy(&mcsi->vb_queue))) {
		mutex_unlock(&mcsi->lock);
		return -EBUSY;
	}

	vfmt = mcsi_try_format(mcsi, &mf->width, &mf->height,
				   &mf->code, NULL, fmt->pad);

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		mf = v4l2_subdev_get_try_format(fh, fmt->pad);
		*mf = fmt->format;
		mutex_unlock(&mcsi->lock);
		PRT_TRACE_END("\n");
		return 0;
	}

	if (fmt->pad == VISS_SD_PAD_SINK) {
		sink->f_width = mf->width;
		sink->f_height = mf->height;
		sink->fmt = vfmt;
		/* Set sink crop rectangle */
		sink->rect.width = mf->width;
		sink->rect.height = mf->height;
		sink->rect.left = 0;
		sink->rect.top = 0;
		/* Reset source format and crop rectangle */
		source->rect = sink->rect;
		source->f_width = mf->width;
		source->f_height = mf->height;
	} else {
		/* Allow changing format only on sink pad */
		mf->code = sink->fmt->mbus_code;
		mf->width = sink->rect.width;
		mf->height = sink->rect.height;
	}

	mutex_unlock(&mcsi->lock);
	PRT_TRACE_END("\n");
	return 0;
}

static int mcsi_subdev_get_selection(struct v4l2_subdev *sd,
					  struct v4l2_subdev_fh *fh,
					  struct v4l2_subdev_selection *sel)
{
	struct viss_dev *mcsi = v4l2_get_subdevdata(sd);
	struct viss_frame *f = &mcsi->inp_frame;
	struct v4l2_rect *try_sel;

	if ((sel->target != V4L2_SEL_TGT_CROP &&
	     sel->target != V4L2_SEL_TGT_CROP_BOUNDS) ||
	     sel->pad != VISS_SD_PAD_SINK)
		return -EINVAL;

	PRT_TRACE_BEGIN("\n");
	if (sel->which == V4L2_SUBDEV_FORMAT_TRY) {
		try_sel = v4l2_subdev_get_try_crop(fh, sel->pad);
		if (IS_ERR_OR_NULL(try_sel))
			return -EINVAL;
		sel->r = *try_sel;
		PRT_TRACE_END("\n");
		return 0;
	}

	mutex_lock(&mcsi->lock);
	if (sel->target == V4L2_SEL_TGT_CROP) {
		sel->r = f->rect;
	} else {
		sel->r.left = 0;
		sel->r.top = 0;
		sel->r.width = f->f_width;
		sel->r.height = f->f_height;
	}

	PRT_DBG("(%d,%d) %dx%d, f_w: %d, f_h: %d\n",
		 f->rect.left, f->rect.top, f->rect.width,
		 f->rect.height, f->f_width, f->f_height);
	mutex_unlock(&mcsi->lock);

	PRT_TRACE_END("\n");
	return 0;
}

static int mcsi_subdev_set_selection(struct v4l2_subdev *sd,
					  struct v4l2_subdev_fh *fh,
					  struct v4l2_subdev_selection *sel)
{
	struct viss_dev *mcsi = v4l2_get_subdevdata(sd);
	struct viss_frame *f = &mcsi->inp_frame;
	struct v4l2_rect *try_sel;

	if (sel->target != V4L2_SEL_TGT_CROP || sel->pad != VISS_SD_PAD_SINK)
		return -EINVAL;

	PRT_TRACE_BEGIN("\n");
	mutex_lock(&mcsi->lock);
	mcsi_try_crop(mcsi, &sel->r);

	if (sel->which == V4L2_SUBDEV_FORMAT_TRY) {
		try_sel = v4l2_subdev_get_try_crop(fh, sel->pad);
		if (IS_ERR_OR_NULL(try_sel)) {
			mutex_unlock(&mcsi->lock);
			return -EINVAL;
		}
		*try_sel = sel->r;
	} else {
		unsigned long flags;
		spin_lock_irqsave(&mcsi->slock, flags);
		f->rect = sel->r;
		/* Same crop rectangle on the source pad */
		mcsi->out_frame.rect = sel->r;
		set_bit(ST_PREV_CONFIG, &mcsi->state);
		spin_unlock_irqrestore(&mcsi->slock, flags);
	}
	mutex_unlock(&mcsi->lock);

	PRT_DBG("(%d,%d) %dx%d, f_w: %d, f_h: %d\n",
		 f->rect.left, f->rect.top, f->rect.width,
		 f->rect.height, f->f_width, f->f_height);

	PRT_TRACE_END("\n");
	return 0;
}

static int mcsi_subdev_s_stream(struct v4l2_subdev *sd, int on)
{
	struct viss_dev *mcsi = v4l2_get_subdevdata(sd);
	unsigned long flags;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");
	/*
	 * Find sensor subdev linked to VISS-MIPI-CSI directly. This is
	 * required for configuration where VISS-MIPI-CSI is used as a subdev
	 * only and feeds data internally to VISS-ISP.
	 * The pipeline links are protected through entity.stream_count
	 * so there is no need to take the media graph mutex here.
	 */
	mcsi->sensor = __find_remote_sensor(&sd->entity);

	if (atomic_read(&mcsi->out_path) != VISS_IO_ISP) {
		PRT_TRACE_END("\n");
		return -ENOIOCTLCMD;
	}

	mutex_lock(&mcsi->lock);
	if (on) {
		if (isp_is_offline(mcsi->sensor)) {
			struct viss_source_info *si =
				v4l2_get_subdev_hostdata(mcsi->sensor);
			struct viss_isp_sensor_fmt_info sensor_info;
			int w, h, d;

			v4l2_subdev_call(mcsi->sensor, core, ioctl,
				VIDIOC_VISS_ISP_SENSOR_FMT_INFO, &sensor_info);
			w = sensor_info.width + 2 * sensor_info.hoffset;
			h = sensor_info.height + 2 * sensor_info.voffset;
			d = (sensor_info.buswidth <= ISP_INPUT_8BITS) ? 1 : 2;
			offline_buf_pending[si->isp_id] = &mcsi->pending_buf_q;
			offline_buf_init(&mcsi->pdev->dev, w, h, d, si->isp_id);
		}

		spin_lock_irqsave(&mcsi->slock, flags);

		ret = mcsi_hw_init(mcsi, true);
		if (ret == 0)
			ret = mcsi_start_capture(mcsi);
		else
			csp_viss_top_mcsi_reset();
		spin_unlock_irqrestore(&mcsi->slock, flags);
	} else {
		set_bit(ST_PREV_OFF, &mcsi->state);
		if (isp_is_offline(mcsi->sensor)) {
			struct viss_source_info *si =
				v4l2_get_subdev_hostdata(mcsi->sensor);
			int isp_id = si->isp_id;

			csp_mcsi_reset();
			while (!list_empty(&mcsi->pending_buf_q))
				offline_buf_pop(&mcsi->pending_buf_q);
			while (!list_empty(&mcsi->active_buf_q))
				offline_buf_pop(&mcsi->active_buf_q);

			offline_buf_exit(&mcsi->pdev->dev, isp_id);
		}

		spin_lock_irqsave(&mcsi->slock, flags);
		csp_mcsi_dma_stop();
		csp_mcsi_disable();

		spin_unlock_irqrestore(&mcsi->slock, flags);
		clear_bit(ST_PREV_RUN, &mcsi->state);
	}
	mutex_unlock(&mcsi->lock);
	PRT_TRACE_END("\n");
	return ret;
}

static int mcsi_log_status(struct v4l2_subdev *sd)
{
	csp_mcsi_dump_regs(__func__);
	return 0;
}

static int mcsi_reset(struct v4l2_subdev *sd, u32 val)
{
	struct viss_dev *mcsi = v4l2_get_subdevdata(sd);
	unsigned long flags;

	PRT_INFO("called!\n");
	spin_lock_irqsave(&mcsi->slock, flags);
	if (val == 0) {
		if (csp_mcsi_chk_lbor())
			PRT_WARN("Line Buffer Over Flow detected!\n");
		csp_mcsi_disable();
	} else {
		mcsi_restart(mcsi, true);
	}
	spin_unlock_irqrestore(&mcsi->slock, flags);

	return 0;
}

static const struct v4l2_subdev_pad_ops mcsi_subdev_pad_ops = {
	.enum_mbus_code		= mcsi_subdev_enum_mbus_code,
	.get_selection		= mcsi_subdev_get_selection,
	.set_selection		= mcsi_subdev_set_selection,
	.get_fmt		= mcsi_subdev_get_fmt,
	.set_fmt		= mcsi_subdev_set_fmt,
};

static const struct v4l2_subdev_video_ops mcsi_subdev_video_ops = {
	.s_stream		= mcsi_subdev_s_stream,
};

static const struct v4l2_subdev_core_ops mcsi_core_ops = {
	.log_status		= mcsi_log_status,
	.reset			= mcsi_reset,
};

static struct v4l2_subdev_ops mcsi_subdev_ops = {
	.core			= &mcsi_core_ops,
	.video			= &mcsi_subdev_video_ops,
	.pad			= &mcsi_subdev_pad_ops,
};

static int mcsi_subdev_registered(struct v4l2_subdev *sd)
{
	struct viss_dev *mcsi = v4l2_get_subdevdata(sd);
	struct vb2_queue *q = &mcsi->vb_queue;
	struct video_device *vfd = &mcsi->vfd;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");
	memset(vfd, 0, sizeof(*vfd));

	mcsi->inp_frame.fmt = &mcsi_formats[0];
	mcsi->out_frame.fmt = &mcsi_formats[0];
	atomic_set(&mcsi->out_path, VISS_IO_DMA);

	snprintf(vfd->name, sizeof(vfd->name), "viss-mcsi.capture");

	vfd->fops = &mcsi_fops;
	vfd->ioctl_ops = &mcsi_ioctl_ops;
	vfd->v4l2_dev = sd->v4l2_dev;
	vfd->minor = -1;
	vfd->release = video_device_release_empty;
	vfd->queue = q;
#if 0
	vfd->lock = &mcsi->lock;
#endif
	mcsi->reqbufs_count = 0;

	INIT_LIST_HEAD(&mcsi->pending_buf_q);
	INIT_LIST_HEAD(&mcsi->active_buf_q);

	memset(q, 0, sizeof(*q));
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF;
	q->ops = &mcsi_qops;
	q->mem_ops = &vb2_dma_contig_memops;
	q->buf_struct_size = sizeof(struct viss_buffer);
	q->drv_priv = mcsi;
	q->timestamp_type = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &mcsi->lock;

	ret = vb2_queue_init(q);
	if (ret < 0)
		return ret;

	mcsi->vd_pad.flags = MEDIA_PAD_FL_SINK;
	ret = media_entity_init(&vfd->entity, 1, &mcsi->vd_pad, 0);
	if (ret < 0)
		return ret;

	video_set_drvdata(vfd, mcsi);
	mcsi->pipeline_ops = v4l2_get_subdev_hostdata(sd);

#if 0
	/*
	 * For proper order of acquiring/releasing the video
	 * and the graph mutex.
	 */
	v4l2_disable_ioctl_locking(vfd, VIDIOC_TRY_FMT);
	v4l2_disable_ioctl_locking(vfd, VIDIOC_S_FMT);
#endif

	ret = video_register_device(vfd, VFL_TYPE_GRABBER, mcsi->nr);
	if (ret < 0) {
		media_entity_cleanup(&vfd->entity);
		mcsi->pipeline_ops = NULL;
		return ret;
	}

	PRT_INFO("Registered %s as /dev/%s\n",
		  vfd->name, video_device_node_name(vfd));
	PRT_TRACE_END("\n");
	return 0;
}

static void mcsi_subdev_unregistered(struct v4l2_subdev *sd)
{
	struct viss_dev *mcsi = v4l2_get_subdevdata(sd);

	if (mcsi == NULL)
		return;

	if (video_is_registered(&mcsi->vfd)) {
		video_unregister_device(&mcsi->vfd);
		media_entity_cleanup(&mcsi->vfd.entity);
		mcsi->pipeline_ops = NULL;
	}
}

static const struct v4l2_subdev_internal_ops mcsi_subdev_internal_ops = {
	.registered = mcsi_subdev_registered,
	.unregistered = mcsi_subdev_unregistered,
};

static int mcsi_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct viss_dev *mcsi = container_of(ctrl->handler, struct viss_dev,
					      ctrl_handler);
	PRT_TRACE_BEGIN("\n");
	set_bit(ST_PREV_CONFIG, &mcsi->state);
	PRT_TRACE_END("\n");
	return 0;
}

static const struct v4l2_ctrl_ops mcsi_ctrl_ops = {
	.s_ctrl	= mcsi_s_ctrl,
};

static const struct v4l2_ctrl_config mcsi_ctrl = {
	.ops	= &mcsi_ctrl_ops,
	.id	= V4L2_CTRL_CLASS_USER | 0x1001,
	.type	= V4L2_CTRL_TYPE_BOOLEAN,
	.name	= "Test Pattern 640x480",
	.step	= 1,
};

static int mcsi_create_capture_subdev(struct viss_dev *mcsi)
{
	struct v4l2_ctrl_handler *handler = &mcsi->ctrl_handler;
	struct v4l2_subdev *sd = &mcsi->subdev;
	int ret;

	PRT_TRACE_BEGIN("\n");

	v4l2_subdev_init(sd, &mcsi_subdev_ops);
	sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	snprintf(sd->name, sizeof(sd->name), "VISS-MIPI-CSI");

	mcsi->subdev_pads[VISS_SD_PAD_SINK].flags = MEDIA_PAD_FL_SINK;
	mcsi->subdev_pads[VISS_SD_PAD_SOURCE_DMA].flags = MEDIA_PAD_FL_SOURCE;
	mcsi->subdev_pads[VISS_SD_PAD_SOURCE_ISP].flags = MEDIA_PAD_FL_SOURCE;
	ret = media_entity_init(&sd->entity, VISS_SD_PADS_NUM,
				mcsi->subdev_pads, 0);
	if (ret)
		return ret;

	v4l2_ctrl_handler_init(handler, 1);
	mcsi->test_pattern = v4l2_ctrl_new_custom(handler, &mcsi_ctrl, NULL);
	if (handler->error) {
		media_entity_cleanup(&sd->entity);
		return handler->error;
	}

	sd->ctrl_handler = handler;
	sd->internal_ops = &mcsi_subdev_internal_ops;
	sd->entity.ops = &mcsi_subdev_media_ops;
	sd->owner = THIS_MODULE;
	v4l2_set_subdevdata(sd, mcsi);

	PRT_TRACE_END("\n");
	return 0;
}

static void mcsi_unregister_capture_subdev(struct viss_dev *mcsi)
{
	struct v4l2_subdev *sd = &mcsi->subdev;

	PRT_TRACE_BEGIN("\n");
	v4l2_device_unregister_subdev(sd);
	media_entity_cleanup(&sd->entity);
	v4l2_ctrl_handler_free(&mcsi->ctrl_handler);
	v4l2_set_subdevdata(sd, NULL);
	PRT_TRACE_END("\n");
}

static void mcsi_clk_put(struct viss_dev *mcsi)
{
	/* TODO */
}

static int mcsi_clk_get(struct viss_dev *mcsi)
{
	int ret = 0;

	/* TODO */

	return ret;
}

static const struct of_device_id mcsi_of_match[];

/* do things when waiting for hardware interrupts timeout. */
static void lombo_mcsi_to_timer(unsigned long data)
{
	struct viss_dev *mcsi;
	unsigned long flags;

	mcsi = (struct viss_dev *)data;

	spin_lock_irqsave(&mcsi->slock, flags);

	if (test_bit(ST_PREV_STREAM, &mcsi->state)) {
		if (csp_mcsi_chk_lbor()) {
			mcsi_restart(mcsi, false);
			PRT_WARN("Line Buffer Over Flow detected!\n");
			mcsi->to_cnt = 0;
		} else if (!list_empty(&mcsi->active_buf_q)) {
			mcsi->to_cnt++;

			if ((mcsi->to_cnt % 10) == 0)
				mcsi_restart(mcsi, false);

			if ((mcsi->to_cnt == 10) ||
				((mcsi->to_cnt % 100) == 0)) {
				spin_unlock_irqrestore(&mcsi->slock, flags);
				PRT_ERR("mcsi Timeout waiting interrupt.\n");
				csp_mcsi_dump_regs(__func__);
				spin_lock_irqsave(&mcsi->slock, flags);
			}
		}
	}
	mod_timer(&mcsi->timer, jiffies + HZ / 10);
	mmiowb();
	spin_unlock_irqrestore(&mcsi->slock, flags);
}

static int mcsi_probe(struct platform_device *pdev)
{
	struct viss_drvdata *drv_data = NULL;
	struct device *dev = &pdev->dev;
	const struct of_device_id *of_id;
	struct viss_dev *mcsi;
	struct resource *res;
	int ret;

	PRT_TRACE_BEGIN("\n");

	if (dev->of_node == NULL) {
		PRT_DBG("no device data specified\n");
		return -ENOENT;
	}

	mcsi = devm_kzalloc(dev, sizeof(*mcsi), GFP_KERNEL);
	if (!mcsi)
		return -ENOMEM;

	mcsi->nr = of_alias_get_id(dev->of_node, "video");

	of_id = of_match_node(mcsi_of_match, dev->of_node);
	if (of_id)
		drv_data = (struct viss_drvdata *)of_id->data;

	if (!drv_data)
		return -EINVAL;

	mcsi->dd = drv_data;
	mcsi->pdev = pdev;

	PRT_DBG("mcsi->dd->max_width=%d\n", mcsi->dd->max_width);
	PRT_DBG("mcsi->dd->max_height=%d\n", mcsi->dd->max_height);
	PRT_DBG("mcsi->dd->out_width_align=%d\n", mcsi->dd->out_width_align);
	PRT_DBG("mcsi->dd->win_hor_offs_align=%d\n",
		mcsi->dd->win_hor_offs_align);
	PRT_DBG("mcsi->dd->out_hor_offs_align=%d\n",
		mcsi->dd->out_hor_offs_align);

	init_waitqueue_head(&mcsi->irq_queue);
	spin_lock_init(&mcsi->slock);
	mutex_init(&mcsi->lock);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	mcsi->regs = devm_ioremap_resource(dev, res);
	if (IS_ERR(mcsi->regs))
		return PTR_ERR(mcsi->regs);

	csp_mcsi_set_register_base(mcsi->regs);

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (res == NULL) {
		dev_err(dev, "Failed to get IRQ resource\n");
		return -ENXIO;
	}

	ret = mcsi_clk_get(mcsi);
	if (ret)
		return ret;

	setup_timer(&mcsi->timer, lombo_mcsi_to_timer, (unsigned long)mcsi);

	ret = devm_request_irq(dev, res->start, mcsi_irq_handler,
			       0, dev_name(dev), mcsi);
	if (ret) {
		dev_err(dev, "Failed to install irq (%d)\n", ret);
		goto err_clk;
	}

	/* The video node will be created within the subdev's registered() op */
	ret = mcsi_create_capture_subdev(mcsi);
	if (ret)
		goto err_clk;

	ret = device_create_file(&pdev->dev, &dev_attr_mcsi_reparse);
	if (ret) {
		PRT_ERR("device_create_file failed (%d)\n", ret);
		goto err_sd;
	}

	platform_set_drvdata(pdev, mcsi);
	pm_runtime_enable(dev);
	ret = pm_runtime_get_sync(dev);
	if (ret < 0)
		goto err_sd;

	mcsi->alloc_ctx = vb2_dma_contig_init_ctx(dev);
	if (IS_ERR(mcsi->alloc_ctx)) {
		ret = PTR_ERR(mcsi->alloc_ctx);
		goto err_pm;
	}
	pm_runtime_put(dev);

	PRT_DBG("VISS-MIPI-CSI registered successfully\n");
	return 0;
err_pm:
	pm_runtime_put(dev);
err_sd:
	mcsi_unregister_capture_subdev(mcsi);
err_clk:
	mcsi_clk_put(mcsi);
	PRT_TRACE_END("\n");
	return ret;
}

#ifdef CONFIG_PM_RUNTIME
static int mcsi_runtime_resume(struct device *dev)
{
	/* struct viss_dev *mcsi = dev_get_drvdata(dev); */

	/* TODO */

	return 0;
}

static int mcsi_runtime_suspend(struct device *dev)
{
	/* struct viss_dev *mcsi = dev_get_drvdata(dev); */

	/* TODO */
	return 0;
}
#endif

#ifdef CONFIG_PM_SLEEP
static int mcsi_resume(struct device *dev)
{
	struct viss_dev *mcsi = dev_get_drvdata(dev);
	unsigned long flags;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");
	spin_lock_irqsave(&mcsi->slock, flags);
	if (!test_and_clear_bit(ST_PREV_LPM, &mcsi->state) ||
	    !test_bit(ST_PREV_IN_USE, &mcsi->state)) {
		spin_unlock_irqrestore(&mcsi->slock, flags);
		PRT_TRACE_END("\n");
		return 0;
	}

	if (atomic_read(&mcsi->out_path) == VISS_IO_DMA) {
		del_timer(&mcsi->timer);
		mcsi->to_cnt = 0;
	}

	csp_mcsi_reset();
	csp_viss_top_mcsi_reset();
	spin_unlock_irqrestore(&mcsi->slock, flags);

	if (!test_and_clear_bit(ST_PREV_SUSPENDED, &mcsi->state)) {
		PRT_TRACE_END("\n");
		return 0;
	}

	INIT_LIST_HEAD(&mcsi->active_buf_q);
	viss_pipeline_call(mcsi, open, &mcsi->pipeline,
			   &mcsi->vfd.entity, false);

	spin_lock_irqsave(&mcsi->slock, flags);
	mcsi_hw_init(mcsi, atomic_read(&mcsi->out_path) == VISS_IO_ISP);
	clear_bit(ST_PREV_SUSPENDED, &mcsi->state);

	if (vb2_is_streaming(&mcsi->vb_queue)) {
		ret = mcsi_start_capture(mcsi);
		spin_unlock_irqrestore(&mcsi->slock, flags);
		if (ret != 0)
			goto out;

		ret = viss_pipeline_call(mcsi, set_stream, &mcsi->pipeline, 1);
		if (ret != 0)
			goto out;

		mod_timer(&mcsi->timer, jiffies + HZ / 10);
		mcsi->to_cnt = 0;
	} else {
		spin_unlock_irqrestore(&mcsi->slock, flags);
	}

out:
	PRT_TRACE_END("\n");
	return ret;
}

static int mcsi_suspend(struct device *dev)
{
	struct viss_dev *mcsi = dev_get_drvdata(dev);
	bool suspend = test_bit(ST_PREV_IN_USE, &mcsi->state);
	int ret;

	PRT_TRACE_BEGIN("\n");
	if (test_and_set_bit(ST_PREV_LPM, &mcsi->state)) {
		PRT_TRACE_END("\n");
		return 0;
	}

	if (!suspend)
		return 0;

	ret = mcsi_stop_capture(mcsi, suspend);
	if (ret < 0 || !test_bit(ST_PREV_IN_USE, &mcsi->state))
		return ret;

	return viss_pipeline_call(mcsi, close, &mcsi->pipeline);
}
#endif /* CONFIG_PM_SLEEP */

static int mcsi_remove(struct platform_device *pdev)
{
	struct viss_dev *mcsi = platform_get_drvdata(pdev);
	struct device *dev = &pdev->dev;

	PRT_TRACE_BEGIN("\n");
	pm_runtime_disable(dev);
	pm_runtime_set_suspended(dev);
	mcsi_unregister_capture_subdev(mcsi);
	vb2_dma_contig_cleanup_ctx(mcsi->alloc_ctx);
	mcsi_clk_put(mcsi);

	del_timer_sync(&mcsi->timer);

	dev_info(dev, "Driver unloaded\n");
	PRT_TRACE_END("\n");
	return 0;
}

static const struct dev_pm_ops mcsi_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(mcsi_suspend, mcsi_resume)
	SET_RUNTIME_PM_OPS(mcsi_runtime_suspend, mcsi_runtime_resume,
			   NULL)
};

static struct viss_drvdata mcsi_drvdata_n7 = {
	.max_width		= 8192,
	.max_height		= 8192,
	.out_width_align	= 8,
	.win_hor_offs_align	= 2,
	.out_hor_offs_align	= 8,
};

static const struct of_device_id mcsi_of_match[] = {
	{
		.compatible = "lombo,n7-viss-mcsi",
		.data = &mcsi_drvdata_n7,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, mcsi_of_match);

static struct platform_driver mcsi_driver = {
	.probe		= mcsi_probe,
	.remove		= mcsi_remove,
	.driver = {
		.of_match_table = mcsi_of_match,
		.name		= MIPI_CSI_DRV_NAME,
		.owner		= THIS_MODULE,
		.pm		= &mcsi_pm_ops,
	}
};
module_platform_driver(mcsi_driver);
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" MIPI_CSI_DRV_NAME);
