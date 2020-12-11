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
#include "viss-vic.h"
#include "csp.h"
#include "viss-m2m.h"

#ifdef CONFIG_ARCH_LOMBO_N7V1
static uint32_t vic_num;
#endif

static const struct viss_fmt vic_formats[] = {
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
	/* Bayer formats */
	 {
		.name		= "RAW8 (GRBG)",
		.fourcc		= V4L2_PIX_FMT_SGRBG8,
		.depth		= { 8 },
		.color		= VISS_FMT_RAW8,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_SGRBG8_1X8,
		.flags		= FMT_FLAGS_RAW_BAYER | FMT_FLAGS_CAM,
	}, {
		.name		= "RAW8 (GBRG)",
		.fourcc		= V4L2_PIX_FMT_SGBRG8,
		.depth		= { 8 },
		.color		= VISS_FMT_RAW8,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_SGBRG8_1X8,
		.flags		= FMT_FLAGS_RAW_BAYER | FMT_FLAGS_CAM,
	}, {
		.name		= "RAW8 (GRBG)",
		.fourcc		= V4L2_PIX_FMT_SGRBG8,
		.depth		= { 8 },
		.color		= VISS_FMT_RAW8,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_SGRBG8_1X8,
		.flags		= FMT_FLAGS_RAW_BAYER | FMT_FLAGS_CAM,
	}, {
		.name		= "RAW8 (RGGB)",
		.fourcc		= V4L2_PIX_FMT_SRGGB8,
		.depth		= { 8 },
		.color		= VISS_FMT_RAW8,
		.memplanes	= 1,
		.colplanes	= 1,
		.mbus_code	= V4L2_MBUS_FMT_SRGGB8_1X8,
		.flags		= FMT_FLAGS_RAW_BAYER | FMT_FLAGS_CAM,
	}, {
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
 * vic_find_format - lookup VIC color format by fourcc or media bus code
 * @pixelformat: fourcc to match, ignored if null
 * @mbus_code: media bus code to match, ignored if null
 * @mask: the color format flags to match
 * @index: index to the vic_formats array, ignored if negative
 */
static const struct viss_fmt *vic_find_format(const u32 *pixelformat,
			const u32 *mbus_code, unsigned int mask, int index)
{
	const struct viss_fmt *fmt, *def_fmt = NULL;
	unsigned int i;
	int id = 0;

	if (index >= (int)ARRAY_SIZE(vic_formats))
		return NULL;

	PRT_TRACE_BEGIN("\n");

	if (pixelformat != NULL)
		PRT_DBG("*pixelformat=0x%x\n", *pixelformat);
	if (mbus_code != NULL)
		PRT_DBG("*mbus_code=0x%x\n", *mbus_code);

	for (i = 0; i < ARRAY_SIZE(vic_formats); ++i) {
		fmt = &vic_formats[i];
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

static void vic_account_framerate(struct viss_dev *vic, struct timeval *tv)
{
	u32 timeused = 0;
	u32 frmr_int = 0;
	u32 frmr_dec = 0;

	if (0 == vic->itv_frames)
		return;

	if (vic->frames == 0) {
		vic->last_tv = *tv;
	} else if (vic->frames == vic->itv_frames) {

		timeused = (u64)(tv->tv_sec - vic->last_tv.tv_sec) *
				USEC_PER_SEC +
				tv->tv_usec - vic->last_tv.tv_usec;
		frmr_int = (vic->itv_frames * 1000 * 1000) /
			(timeused / 1000);
		frmr_dec = frmr_int % 1000;
		frmr_int = frmr_int / 1000;

		PRT_INFO("output %d frames timeused %u us\n",
			vic->itv_frames, timeused);
		PRT_INFO("frame rate %u.%03u\n", frmr_int, frmr_dec);

		vic->last_tv = *tv;
		vic->frames = 0;
	}
	vic->frames++;
}

static ssize_t viss_vic_reparse_sysfs_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct viss_dev *vic = platform_get_drvdata(pdev);
	int count = 0;

	if (isp_is_offline(vic->sensor)) {
		struct viss_buffer *buf;
		vic->reqbufs_count = RAW_BUF_NUM;
		vic->pending_cnt = 0;
		vic->active_cnt = 0;
		list_for_each_entry(buf, &vic->pending_buf_q, list)
			vic->pending_cnt++;
		list_for_each_entry(buf, &vic->active_buf_q, list)
			vic->active_cnt++;
	}

	count += sprintf(buf + count, "vic streaming %d\n",
			vic->streaming);
	count += sprintf(buf + count, "vic state %ld\n",
			vic->state);
	count += sprintf(buf + count, "frame count %d\n",
			vic->frame_count);
	count += sprintf(buf + count, "frame lost count %d\n",
			vic->frames_lost_count);
	count += sprintf(buf + count, "requset buf count %d\n",
			vic->reqbufs_count);
	count += sprintf(buf + count, "pending buf count %d\n",
			vic->pending_cnt);
	count += sprintf(buf + count, "active buf count %d\n",
			vic->active_cnt);
	count += sprintf(buf + count, "frame size input: %dx%d\n",
		vic->inp_frame.f_width, vic->inp_frame.f_height);
	count += sprintf(buf + count, "frame size output: %dx%d\n",
		vic->out_frame.f_width, vic->out_frame.f_height);

	return count;
}

static ssize_t viss_vic_reparse_sysfs_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct viss_dev *vic = platform_get_drvdata(pdev);
	unsigned int itv_frames = 0;
	unsigned long flags;
	int n = 0;

	n = sscanf(buf, "%d", &itv_frames);
	if (n != 1) {
		PRT_ERR("Invalid interval frames (%s)\n", buf);
		return count;
	}

	spin_lock_irqsave(&vic->slock, flags);
	vic->frames = 0;
	vic->itv_frames = itv_frames;
	spin_unlock_irqrestore(&vic->slock, flags);

	return count;
}

static DEVICE_ATTR(vic_reparse, S_IWUSR | S_IRUGO,
	viss_vic_reparse_sysfs_show, viss_vic_reparse_sysfs_store);

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

static int __vic_if_config(struct viss_dev *vic, struct viss_source_info *si)
{
	u32 sync_proto = 0;
	u32 field_det = 0;
	u32 data_proto = 0;

	switch (si->if_type) {
	case VISS_IF_TYPE_DC:
		sync_proto = 0;
		if (vic->inp_frame.fmt->flags & FMT_FLAGS_RAW_BAYER) {
			if (si->if_mode == 0)
				data_proto = 0;
			else if (si->if_mode == 1)
				data_proto = 1;
			else if (si->if_mode == 2)
				data_proto = 2;
			else {
				PRT_ERR("Invalid mode %d, if_type %d!\n",
					si->if_mode, si->if_type);
				return -EINVAL;
			}
		} else {
			data_proto = 3;
		}
		break;
	case VISS_IF_TYPE_ITU_601:
		if (si->if_mode == 0) {
			sync_proto = 2;
			field_det = 3;
			data_proto = 3;
		} else if (si->if_mode == 1) {
			sync_proto = 5;
			field_det = 1;
			data_proto = 3;
		} else if (si->if_mode == 2) {
			sync_proto = 2;
			field_det = 1;
			data_proto = 3;
		} else if (si->if_mode == 3) {
			sync_proto = 2;
			field_det = 2;
			data_proto = 5;
		} else {
			PRT_ERR("Invalid mode %d, if_type %d!\n",
				si->if_mode, si->if_type);
			return -EINVAL;
		}
		break;
	case VISS_IF_TYPE_ITU_656:
		sync_proto = 3;
		field_det = 0;
		data_proto = 3;
		break;
	case VISS_IF_TYPE_ITU_1120:
		if (si->if_mode == 0) {
			/* mode0 */
			sync_proto = 3;
			data_proto = 5;
		} else if (si->if_mode == 1) {
			/* mode1 */
			sync_proto = 4;
			data_proto = 3;
		} else {
			PRT_ERR("Invalid mode %d, if_type %d!\n",
				si->if_mode, si->if_type);
			return -EINVAL;
		}
		field_det = 0;
		break;
	/* TODO: JPEG mode */
	default:
		PRT_ERR("Unsupport interface type!\n");
		return -EINVAL;
	}

	PRT_DBG("if_mode=%d,if_type=%d\n",
			si->if_mode, si->if_type);
	PRT_DBG("sync_proto=%d,field_det=%d,data_proto=%d\n",
			sync_proto, field_det, data_proto);
	/* PRT_ERR("vic->sensor->name %s!\n", vic->sensor->name); */
	if (0 == strcmp(vic->sensor->name, "tp3810_dvp"))
		csp_vic_set_pclk_phase(62);/* 0x3e */

	/*
	 * set camera interface type
	 */
	csp_vic_select_sync_protocol(sync_proto);
	csp_vic_select_field_detecton(field_det);
	csp_vic_select_video_mode(si->interlaced);

	/*
	 * data transfer and storage
	 */
	csp_vic_select_data_protocol(data_proto);
	if (vic->ch_number) {
		csp_vic_set_multi_channel_number(vic->ch_number);
		csp_vic_multi_channel_enable();
	}

#if 0	/* use default embeded SYNC code position for BT.1120 mode0 */
	if ((si->if_type == VISS_IF_TYPE_ITU_1120) && (si->if_mode == 0))
		csp_vic_select_sync_code_position(0);
#endif

	if (si->if_type == VISS_IF_TYPE_ITU_601) {
		struct bt601_timing *bt601_timing = NULL;

		bt601_timing = (struct bt601_timing *)si->if_info;
		csp_vic_hsync_fb_offset(bt601_timing->h_off);
		csp_vic_hsync_active_width(bt601_timing->active_w);
		csp_vic_f1_vsync_fb_offset(bt601_timing->t_off);
		csp_vic_f1_vsync_active_line(bt601_timing->t_active_l);
		csp_vic_f2_vsync_fb_offset(bt601_timing->b_off);
		csp_vic_f2_vsync_active_line(bt601_timing->b_active_l);
	}

	/*
	 * set camera polarity
	 */
#ifdef CONFIG_ARCH_LOMBO_N7V1
	if (2 == vic_num) {
		/* PRT_ERR("vic_num %d!\n", vic_num); */
		if (si->pclk_polarity)
			csp_vic_set_pclk_polarity(
			(si->flags & V4L2_MBUS_PCLK_SAMPLE_FALLING) ? 0 : 1);
		else
			csp_vic_set_pclk_polarity(
			(si->flags & V4L2_MBUS_PCLK_SAMPLE_FALLING) ? 1 : 0);
	} else
		csp_vic_set_pclk_polarity(
			(si->flags & V4L2_MBUS_PCLK_SAMPLE_FALLING) ? 1 : 0);
#else
	csp_vic_set_pclk_polarity(
		(si->flags & V4L2_MBUS_PCLK_SAMPLE_FALLING) ? 1 : 0);
#endif
	csp_vic_set_field_polarity(
		(si->flags & V4L2_MBUS_FIELD_EVEN_LOW) ? 1 : 0);
	csp_vic_set_hsync_polarity(
		(si->flags & V4L2_MBUS_HSYNC_ACTIVE_LOW) ? 1 : 0);

	if (si->vsync_polarity)
		csp_vic_set_vsync_polarity(
		(si->flags & V4L2_MBUS_VSYNC_ACTIVE_LOW) ? 0 : 1);
	else
		csp_vic_set_vsync_polarity(
		(si->flags & V4L2_MBUS_VSYNC_ACTIVE_LOW) ? 1 : 0);

#if 0
	csp_vic_set_pclk_phase(0);
#endif

	return 0;
}

static void __vic_yuv_seq_adjust(struct viss_dev *vic)
{
	u32 yuv_seq = 0;
	u32 uv_swap = 0;
	u32 out_fmt = VIC_OUTFMT_YUV420SP;
	u32 ch = 0;

	if (vic->out_frame.fmt->flags & FMT_FLAGS_CAM)
		out_fmt = VIC_OUTFMT_PAST_THROUGH;
	else {
		switch (vic->out_frame.fmt->color) {
		case VISS_FMT_YCBYCR422:
		case VISS_FMT_YCRYCB422:
			if ((vic->out_frame.fmt->color) ==
				VISS_FMT_YCRYCB422)
				uv_swap = 1;
			if (vic->out_frame.fmt->colplanes == 2)
				out_fmt = VIC_OUTFMT_YUV422SP;
			else
				out_fmt = VIC_OUTFMT_YUV422P;
			break;
		case VISS_FMT_YCBCR420:
		case VISS_FMT_YCRCB420:
			if ((vic->out_frame.fmt->color) ==
				VISS_FMT_YCRCB420)
				uv_swap = 1;
			if (vic->out_frame.fmt->colplanes == 2)
				out_fmt = VIC_OUTFMT_YUV420SP;
			else
				out_fmt = VIC_OUTFMT_YUV420P;
			break;
		}

		/* set yuv seq */
		switch (vic->inp_frame.fmt->color) {
		case VISS_FMT_YCBYCR422:
			if (uv_swap == 0)
				yuv_seq = VIC_C1C0C3C2;
			else
				yuv_seq = VIC_C3C0C1C2;
			break;
		case VISS_FMT_YCRYCB422:
			if (uv_swap == 0)
				yuv_seq = VIC_C3C0C1C2;
			else
				yuv_seq = VIC_C1C0C3C2;
			break;
		case VISS_FMT_CBYCRY422:
			if (uv_swap == 0)
				yuv_seq = VIC_C0C1C2C3;
			else
				yuv_seq = VIC_C2C1C0C3;
			break;
		case VISS_FMT_CRYCBY422:
			if (uv_swap == 0)
				yuv_seq = VIC_C2C1C0C3;
			else
				yuv_seq = VIC_C0C1C2C3;
			break;
		}
	}
	csp_vic_set_yuv_seq(yuv_seq);

	/* set output format */
	for (ch = 0; ch <= vic->ch_number; ch++)
		csp_vic_dma_output_format(VIC_DMA_CH0 + ch, out_fmt);
}

static int vic_hw_init(struct viss_dev *vic, bool isp_output)
{
	struct viss_source_info *si;
	u32 data_path = 0;
	int ret = 0;
	u32 ch = 0;
#ifdef CONFIG_ARCH_LOMBO_N7V1
	uint32_t p1 = 0, p2 = 0;
#endif
	if (vic->sensor == NULL)
		return -ENXIO;

	if (vic->inp_frame.fmt == NULL || vic->out_frame.fmt == NULL)
		return -EINVAL;

	PRT_TRACE_BEGIN("\n");
#ifdef CONFIG_ARCH_LOMBO_N7V1
	vic_num = 0;
	if (lombo_func1())
		return -EINVAL;
	if (lombo_func2(&p1, &p2))
		return -EINVAL;
	vic_num = p1;
#endif
	/* Get sensor configuration data from the sensor subdev */
	si = v4l2_get_subdev_hostdata(vic->sensor);
	if (si == NULL) {
		PRT_ERR("can't get sensor configuration data!\n");
		return -EINVAL;
	}

	/* isp_output only support VISS_IF_TYPE_DC */
	if (isp_output && (si->if_type != VISS_IF_TYPE_DC)) {
		PRT_ERR("ISP output not support ITU_* interface!\n");
		return -EINVAL;
	}

	csp_viss_top_vic_reset();

	ret = __vic_if_config(vic, si);
	if (ret != 0)
		return ret;

#if 0
	/* TODO: multi DMA chanel, only for BT.656 and BT.1120 */
	if () {
		csp_vic_multi_channel_enable();
		csp_vic_set_multi_channel_number();
		csp_vic_set_multi_channel_id();
	}
#endif

	data_path = isp_output ? VIC_OUTPUT_PIXEL_CH : VIC_OUTPUT_DMA;

	csp_vic_data_path(data_path);
	if (!isp_output) {
		__vic_yuv_seq_adjust(vic);

		if (vic->ch_number) {
			for (ch = 0; ch <= vic->ch_number; ch++) {
#if 1
				csp_vic_dma_burst_length(VIC_DMA_CH0 + ch,
						VIC_DMA_BURST_LEN_8);
#endif

				/* set dma window */
				csp_vic_dma_offset(VIC_DMA_CH0 + ch,
						vic->inp_frame.rect.left,
						vic->inp_frame.rect.top);

				csp_vic_dma_size(VIC_DMA_CH0 + ch,
						vic->inp_frame.sub_width * 2,
						vic->inp_frame.sub_height);

				csp_vic_clear_set_ints(VIC_DMA_CH0 + ch,
						VIC_DMA_INT_ALL_MASK,
						VIC_DMA_INT_ALL_ERR |
						VIC_DMA_INT_FCI);

				csp_vic_dma_fci_gen_mode(VIC_DMA_CH0 + ch,
						si->interlaced);
			}
		} else {
#if 1
			csp_vic_dma_burst_length(VIC_DMA_CH0,
					VIC_DMA_BURST_LEN_8);
#endif
			/* set dma window */
			csp_vic_dma_offset(VIC_DMA_CH0,
					vic->inp_frame.rect.left,
					vic->inp_frame.rect.top);

			csp_vic_dma_size(VIC_DMA_CH0,
					vic->inp_frame.rect.width * 2,
					vic->inp_frame.rect.height);

			csp_vic_clear_set_ints(VIC_DMA_CH0,
						VIC_DMA_INT_ALL_MASK,
						VIC_DMA_INT_ALL_ERR |
						VIC_DMA_INT_FCI);
		}


#if 0	/* TODO */
		/* field storage order */
		if () {
			csp_vic_dma_storage_mode();
			csp_vic_dma_linestride();
		}

		csp_vic_dma_fci_gen_mode();
		csp_vic_dma_vbi_gen_mode();
		csp_vic_dma_lcti_gen_mode();
#endif
		PRT_DBG("vic->out_frame.fmt->colplanes: %d\n",
			vic->out_frame.fmt->colplanes);
		if (vic->ch_number) {
			switch (vic->sort) {
			/* channel sorting:=
			 *                 =
			 */
			case 0:
				for (ch = 0; ch <= vic->ch_number; ch++) {
					if (vic->out_frame.fmt->colplanes == 1)
						csp_vic_dma_linestride(
						VIC_DMA_CH0 + ch,
						vic->inp_frame.sub_width * 4,
						0);
					else if (vic->out_frame.fmt->colplanes
							== 2)
						csp_vic_dma_linestride(
						VIC_DMA_CH0 + ch,
						vic->inp_frame.sub_width * 2,
						vic->inp_frame.sub_width * 2);
					else
						csp_vic_dma_linestride(
						VIC_DMA_CH0 + ch,
						vic->inp_frame.sub_width * 2,
						vic->inp_frame.sub_width / 2);
					/* FCI */
					csp_vic_dma_update_cmpt_address(
							VIC_DMA_CH0 + ch,
							VIC_UD_TIMING_FCI);
					}
				break;
			case 1:
				/* channel sorting:== */
				for (ch = 0; ch <= vic->ch_number; ch++) {
					if (vic->out_frame.fmt->colplanes == 1)
						csp_vic_dma_linestride(
						VIC_DMA_CH0 + ch,
						vic->inp_frame.sub_width * 2,
						0);
					else if (vic->out_frame.fmt->colplanes
							== 2)
						csp_vic_dma_linestride(
						VIC_DMA_CH0 + ch,
						vic->inp_frame.sub_width,
						vic->inp_frame.sub_width);
					else
						csp_vic_dma_linestride(
						VIC_DMA_CH0 + ch,
						vic->inp_frame.sub_width,
						vic->inp_frame.sub_width / 2);
					/* FCI */
					csp_vic_dma_update_cmpt_address(
							VIC_DMA_CH0 + ch,
							VIC_UD_TIMING_FCI);
				}
				break;
			}

		} else {
			if (vic->out_frame.fmt->colplanes == 1)
				csp_vic_dma_linestride(VIC_DMA_CH0,
						vic->out_frame.f_width * 2, 0);
			else if (vic->out_frame.fmt->colplanes == 2)
				csp_vic_dma_linestride(VIC_DMA_CH0,
						vic->out_frame.f_width,
						vic->out_frame.f_width);
			else
				csp_vic_dma_linestride(VIC_DMA_CH0,
						vic->out_frame.f_width,
						vic->out_frame.f_width / 2);
			/* FCI */
			csp_vic_dma_update_cmpt_address(VIC_DMA_CH0,
						VIC_UD_TIMING_FCI);
		}
		csp_vic_cfg_fifo();
	} else {
		if (isp_is_online(vic->sensor)) {
			csp_viss_top_isp_data_source(0);
		} else {
			struct offline_buf *buf =
				list_entry(vic->pending_buf_q.next,
					struct offline_buf, list);
			csp_vic_data_path(VIC_OUTPUT_DMA);
			__vic_yuv_seq_adjust(vic);

			csp_vic_dma_burst_length(VIC_DMA_CH0,
				VIC_DMA_BURST_LEN_8);
			/* set dma window */
			csp_vic_dma_offset(VIC_DMA_CH0, 0, 0);
			csp_vic_dma_size(VIC_DMA_CH0,
				buf->win.width * buf->raw_depth,
				buf->win.height);
			csp_vic_dma_linestride(VIC_DMA_CH0,
				buf->win.width * buf->raw_depth, 0);
			if (buf->raw_depth == 1)
				csp_enable_vic_output_high_8bit_data();
			csp_vic_clear_set_ints(VIC_DMA_CH0,
						VIC_DMA_INT_ALL_MASK,
						VIC_DMA_INT_ALL_ERR |
						VIC_DMA_INT_FCI);
			/* FCI */
			csp_vic_dma_update_cmpt_address(VIC_DMA_CH0,
					VIC_UD_TIMING_FCI);
			csp_vic_cfg_fifo();
		}
	}

	csp_vic_dump_regs(__func__);

	PRT_TRACE_END("\n");
	return 0;
}

/*
 * Reinitialize the driver so it is ready to start the streaming again.
 * Set vic->state to indicate stream off and the hardware shut down state.
 * If not suspending (@suspend is false), return any buffers to videobuf2.
 * Otherwise put any owned buffers onto the pending buffers queue, so they
 * can be re-spun when the device is being resumed. Also perform VIC
 * software reset and disable streaming on the whole pipeline if required.
 */
static int vic_state_cleanup(struct viss_dev *vic, bool suspend)
{
	struct viss_buffer *buf;
	unsigned long flags;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");
	spin_lock_irqsave(&vic->slock, flags);

	vic->state &= ~((1 << ST_PREV_RUN) | (1 << ST_PREV_OFF) |
			 (1 << ST_PREV_STREAM));
	if (suspend)
		vic->state |= (1 << ST_PREV_SUSPENDED);
	else
		vic->state &= ~(1 << ST_PREV_PENDING);

	/* Release unused buffers */
	while (!suspend && !list_empty(&vic->pending_buf_q)) {
		buf = vic_pending_queue_pop(vic);
		vb2_buffer_done(&buf->vb, VB2_BUF_STATE_ERROR);
	}
	/* If suspending put unused buffers onto pending queue */
	while (!list_empty(&vic->active_buf_q)) {
		buf = vic_active_queue_pop_tail(vic);
		if (suspend)
			vic_pending_queue_add_front(vic, buf);
		else
			vb2_buffer_done(&buf->vb, VB2_BUF_STATE_ERROR);
	}

	/* reset the vic */
	csp_vic_reset();
	csp_viss_top_vic_reset();

	spin_unlock_irqrestore(&vic->slock, flags);

	ret = viss_pipeline_call(vic, set_stream, &vic->pipeline, 0);
	if (ret != 0)
		PRT_ERR("pipeline stream off failed!\n");

	PRT_TRACE_END("\n");
	return ret;
}

static void vic_set_dma_addr(u32 chanel, struct viss_addr *paddr)
{
	csp_vic_dma_cmpt0_addr(chanel, paddr->y);
	csp_vic_dma_cmpt1_addr(chanel, paddr->cb);
	csp_vic_dma_cmpt2_addr(chanel, paddr->cr);
}

static int vic_set_next_buf(struct viss_dev *vic)
{
	struct viss_buffer *vbuf;
	u32 ch = 0;

	PRT_TRACE_BEGIN("\n");

	if (!list_empty(&vic->pending_buf_q)) {
		vbuf = vic_pending_queue_pop(vic);
		if (!vic->ch_number)
			vic_set_dma_addr(VIC_DMA_CH0 + ch, &vbuf->paddr);
		else
			for (ch = 0; ch <= vic->ch_number; ch++)
				vic_set_dma_addr(VIC_DMA_CH0 + ch,
					&vic->ch_paddr[ch]);
		vic_active_queue_add(vic, vbuf);
	}

	PRT_TRACE_END("\n");
	return 0;
}

static int vic_set_next_buf_offline(struct viss_dev *vic)
{
	PRT_TRACE_BEGIN("\n");

	if (!list_empty(&vic->pending_buf_q)) {
		struct offline_buf *buf = offline_buf_pop(&vic->pending_buf_q);
		csp_vic_dma_cmpt0_addr(VIC_DMA_CH0, buf->dma.paddr);
		offline_buf_add(&vic->active_buf_q, buf);
	}

	PRT_TRACE_END("\n");
	return 0;
}

static int vic_start_capture(struct viss_dev *vic)
{
	int ret = 0;
	u32 ch = 0;

	PRT_TRACE_BEGIN("\n");

	if (atomic_read(&vic->out_path) == VISS_IO_ISP) {
		if (isp_is_online(vic->sensor)) {
			csp_vic_enable();
		} else {
			csp_enable_vic_output_raw_msb_data();
			csp_vic_dma_capture_mode(VIC_DMA_CH0, VIC_CAP_CONT);
			vic_set_next_buf_offline(vic);
			csp_vic_enable();
			csp_vic_dma_start(VIC_DMA_CH0);
			csp_vic_clr_pd(VIC_DMA_CH0, VIC_DMA_INT_ALL_MASK);
			vic_set_next_buf_offline(vic);
		}
		goto out;
	}

	if (!test_and_set_bit(ST_PREV_STREAM, &vic->state)) {
		if (list_empty(&vic->pending_buf_q)) {
			PRT_ERR("Pending buffer queue empty!\n");
			return -EINVAL;
		}
		for (ch = 0; ch <= vic->ch_number; ch++)
			csp_vic_dma_capture_mode(VIC_DMA_CH0 + ch,
						VIC_CAP_CONT);

		vic_set_next_buf(vic);

		csp_vic_enable();
		for (ch = 0; ch <= vic->ch_number; ch++) {
			csp_vic_dma_start(VIC_DMA_CH0 + ch);
			csp_vic_clr_pd(VIC_DMA_CH0 + ch, VIC_DMA_INT_ALL_MASK);
		}
		vic_set_next_buf(vic);
	}
out:
	PRT_TRACE_END("\n");
	return ret;
}

static int vic_stop_capture(struct viss_dev *vic, bool suspend)
{
	unsigned long flags;
	u32 ch = 0;

	if (!vic_active(vic))
		return 0;

	PRT_TRACE_BEGIN("\n");
	set_bit(ST_PREV_OFF, &vic->state);

	if (atomic_read(&vic->out_path) == VISS_IO_DMA) {
		del_timer(&vic->timer);
		vic->to_cnt = 0;
	}

	wait_event_timeout(vic->irq_queue,
			   !test_bit(ST_PREV_OFF, &vic->state),
			   (2 * HZ / 10)); /* 200 ms */
	spin_lock_irqsave(&vic->slock, flags);
	for (ch = 0; ch <= vic->ch_number; ch++)
		csp_vic_dma_stop(VIC_DMA_CH0 + ch);
	csp_vic_disable();
	spin_unlock_irqrestore(&vic->slock, flags);
	return vic_state_cleanup(vic, suspend);
}

/* Must be called with vic.slock spinlock held. */
static void vic_config_update(struct viss_dev *vic)
{
	PRT_TRACE_BEGIN("\n");
	/* set dma window */
	if (vic->ch_number) {
		u32 ch = 0;
		for (ch = 0; ch <= vic->ch_number; ch++) {
			csp_vic_dma_offset(VIC_DMA_CH0 + ch,
					vic->inp_frame.rect.left,
					vic->inp_frame.rect.top);
			csp_vic_dma_size(VIC_DMA_CH0 + ch,
					vic->inp_frame.rect.width * 2,
					vic->inp_frame.rect.height / 2);
		}
	} else {
		csp_vic_dma_offset(VIC_DMA_CH0, vic->inp_frame.rect.left,
		   vic->inp_frame.rect.top);
		csp_vic_dma_size(VIC_DMA_CH0, vic->inp_frame.rect.width * 2,
				 vic->inp_frame.rect.height);
	}

	clear_bit(ST_PREV_CONFIG, &vic->state);
	PRT_TRACE_END("\n");
}

static void vic_isp_offline(struct viss_dev *vic)
{
	u32 intsrc = 0;

	intsrc = csp_vic_get_int_pd(VIC_DMA_CH0);
	csp_vic_clr_pd(VIC_DMA_CH0, VIC_DMA_INT_ALL_MASK);
	if ((intsrc & VIC_DMA_INT_ALL_ERR) != 0) {
		/* drop the current frame and reset vic on error */
		PRT_ERR("reset the vic\n");
		csp_vic_dma_stop(VIC_DMA_CH0);
		csp_vic_reset();
		csp_vic_dma_start(VIC_DMA_CH0);

		if (intsrc & VIC_DMA_INT_P0_OVF)
			PRT_ERR("VIC_DMA_INT_P0_OVF\n");

		if (intsrc & VIC_DMA_INT_P1_OVF)
			PRT_ERR("VIC_DMA_INT_P1_OVF\n");

		if (intsrc & VIC_DMA_INT_P2_OVF)
			PRT_ERR("VIC_DMA_INT_P2_OVF\n");

		return;
	}

	if (intsrc & VIC_DMA_INT_FCI) {
		if (!list_empty(&vic->active_buf_q) &&
		    !list_is_singular(&vic->active_buf_q)) {
			offline_fifo_add(&vic->active_buf_q);
			vic->frame_count++;
		} else {
			vic->frames_lost_count++;
		}
		vic_set_next_buf_offline(vic);
		offline_fifo_process();
	}
}
static irqreturn_t vic_irq_handler(int irq, void *priv)
{
	struct viss_dev *vic = priv;
	struct viss_buffer *vbuf;
	unsigned long flags;
	struct timeval *tv;
	struct timeval tv_v;
	struct timespec ts;
	u32 intsrc[vic->ch_number + 1];
	u32 ch = 0;
	u32 fci_pre_count = 0;
	u32 fci_count = 0;
	u32 fci = 0;

	PRT_TRACE_BEGIN("\n");
	spin_lock_irqsave(&vic->slock, flags);

	if (isp_is_offline(vic->sensor)) {
		vic_isp_offline(vic);
		spin_unlock_irqrestore(&vic->slock, flags);
		PRT_TRACE_END("\n");
		return IRQ_HANDLED;
	}

	if (atomic_read(&vic->out_path) != VISS_IO_DMA)
		goto done;

	mod_timer(&vic->timer, jiffies + HZ / 10);
	vic->to_cnt = 0;

	for (ch = 0; ch <= vic->ch_number; ch++) {
		intsrc[ch] = csp_vic_get_int_pd(VIC_DMA_CH0 + ch);
		csp_vic_clr_pd(VIC_DMA_CH0 + ch, VIC_DMA_INT_ALL_MASK);
	}

	if (test_and_clear_bit(ST_PREV_OFF, &vic->state)) {
		wake_up(&vic->irq_queue);
		goto done;
	}

	for (ch = 0; ch <= vic->ch_number; ch++) {
		if ((intsrc[ch] & VIC_DMA_INT_ALL_ERR) != 0) {
			/* drop the current frame and reset vic on error */
			PRT_ERR("reset the vic\n");
			csp_vic_dma_stop(VIC_DMA_CH0 + ch);
			csp_vic_reset();
			csp_vic_dma_start(VIC_DMA_CH0 + ch);

			clear_bit(ST_PREV_RUN, &vic->state);
			if (intsrc[ch] & VIC_DMA_INT_P0_OVF) {
				PRT_ERR("VIC_DMA_INT_P0_OVF\n");
				vic->events[VIC_DMA_CH0 + ch].p0_overflow++;
			}

			if (intsrc[ch] & VIC_DMA_INT_P1_OVF) {
				PRT_ERR("VIC_DMA_INT_P1_OVF\n");
				vic->events[VIC_DMA_CH0 + ch].p1_overflow++;
			}

			if (intsrc[ch] & VIC_DMA_INT_P2_OVF) {
				PRT_ERR("VIC_DMA_INT_P2_OVF\n");
				vic->events[VIC_DMA_CH0 + ch].p2_overflow++;
			}
#ifdef CONFIG_ARCH_LOMBO_N9V0
			if (intsrc[ch] & VIC_DMA_INT_LOSTH)
				PRT_ERR("VIC_DMA_INT_LOSTH\n");

			if (intsrc[ch] & VIC_DMA_INT_LOSTV)
				PRT_ERR("VIC_DMA_INT_LOSTV\n");
#endif

			goto done;
		}
	}
	PRT_DBG("active_cnt=%d,pending_cnt=%d\n",
		vic->active_cnt, vic->pending_cnt);
	if (!vic->ch_number) {
		fci = intsrc[VIC_DMA_CH0] & VIC_DMA_INT_FCI;
		goto refresh;
	}

	for (ch = 0; ch <= vic->ch_number; ch++) {
		if (vic->ch_pre_fci[ch] && (intsrc[ch] & VIC_DMA_INT_FCI)) {
			fci = 1;
			goto refresh;
		}
		/* after the last buf,
		 * save the FCI number of ch_number channels
		 */
		fci_pre_count += (vic->ch_pre_fci[ch] ||
				(intsrc[ch] & VIC_DMA_INT_FCI));
		/* save the FCI number of ch_number channels */
		fci_count += (intsrc[ch] & VIC_DMA_INT_FCI);
	}
	fci = (((vic->ch_number + 1) == fci_count) ||
		((vic->ch_number + 1) == fci_pre_count));
refresh:
	if ((fci) &&
		test_bit(ST_PREV_RUN, &vic->state) &&
		!list_empty(&vic->active_buf_q) &&
		!list_is_singular(&vic->active_buf_q)) {
		ktime_get_real_ts(&ts);

		/*
		 * the vbuf that store the current frame is in the active queue
		 * head, pop and yield it to user
		 */
		vbuf = vic_active_queue_pop(vic);
		tv = &vbuf->vb.v4l2_buf.timestamp;
		tv->tv_sec = ts.tv_sec;
		tv->tv_usec = ts.tv_nsec / NSEC_PER_USEC;
		vbuf->vb.v4l2_buf.sequence = vic->frame_count++;
		vb2_buffer_done(&vbuf->vb, VB2_BUF_STATE_DONE);
		if (vic->itv_frames != 0)
			vic_account_framerate(vic, tv);
		memset(vic->ch_pre_fci, 0, sizeof(vic->ch_pre_fci));
		vic_set_next_buf(vic);
	} else {
		for (ch = 0; ch <= vic->ch_number; ch++)
			/* the FCI of each channel before
			 *the new buffer is not followed
			 */
			vic->ch_pre_fci[ch] += (intsrc[ch] & VIC_DMA_INT_FCI);
	}

	if (test_bit(ST_PREV_CONFIG, &vic->state))
		vic_config_update(vic);

	if (list_is_singular(&vic->active_buf_q)) {
		WARN_ON(!list_empty(&vic->pending_buf_q));
		/* will set next buffer in buf_queue when usr QBUF */
		vic->frames_lost_count++;
		ktime_get_real_ts(&ts);
		tv_v.tv_sec = ts.tv_sec;
		tv_v.tv_usec = ts.tv_nsec / NSEC_PER_USEC;
		if (vic->itv_frames != 0)
			vic_account_framerate(vic, &tv_v);
		PRT_DBG("frame lost\n");
	}
	PRT_DBG("active_cnt=%d,pending_cnt=%d\n",
		vic->active_cnt, vic->pending_cnt);
done:
	set_bit(ST_PREV_RUN, &vic->state);
	spin_unlock_irqrestore(&vic->slock, flags);
	PRT_TRACE_END("\n");
	return IRQ_HANDLED;
}

static int start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct viss_dev *vic = q->drv_priv;
	unsigned long flags;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");

	spin_lock_irqsave(&vic->slock, flags);
	vic->frame_count = 0;
	ret = vic_hw_init(vic, false);
	if (ret) {
		spin_unlock_irqrestore(&vic->slock, flags);
		vic_state_cleanup(vic, false);
		return ret;
	}

	if (list_is_singular(&vic->pending_buf_q)) {
		PRT_ERR("Only one buffer, request more!\n");
		return -EINVAL;
	}

	ret = vic_start_capture(vic);
	spin_unlock_irqrestore(&vic->slock, flags);
	if (ret != 0)
		goto out;

	ret = viss_pipeline_call(vic, set_stream, &vic->pipeline, 1);
	if (ret != 0)
		goto out;

	mod_timer(&vic->timer, jiffies + HZ / 10);
	vic->to_cnt = 0;

out:
	csp_vic_dump_regs(__func__);
	PRT_TRACE_END("\n");
	return ret;
}

static int stop_streaming(struct vb2_queue *q)
{
	struct viss_dev *vic = q->drv_priv;
#ifdef CONFIG_ARCH_LOMBO_N7V1
	vic_num = 0;
#endif
	if (!vic_active(vic))
		return -EINVAL;

	PRT_TRACE_BEGIN("\n");
	return vic_stop_capture(vic, false);
}

static int queue_setup(struct vb2_queue *vq, const struct v4l2_format *pfmt,
		       unsigned int *num_buffers, unsigned int *num_planes,
		       unsigned int sizes[], void *allocators[])
{
	const struct v4l2_pix_format_mplane *pixm = NULL;
	struct viss_dev *vic = vq->drv_priv;
	struct viss_frame *frame = &vic->out_frame;
	const struct viss_fmt *fmt = frame->fmt;
	unsigned long wh;
	int i;

	PRT_TRACE_BEGIN("\n");
	if (pfmt) {
		pixm = &pfmt->fmt.pix_mp;
		fmt = vic_find_format(&pixm->pixelformat, NULL, 0, -1);
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
		allocators[i] = vic->alloc_ctx;
	}

	PRT_TRACE_END("\n");
	return 0;
}

static int buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_queue *vq = vb->vb2_queue;
	struct viss_dev *vic = vq->drv_priv;
	int i;

	if (vic->out_frame.fmt == NULL)
		return -EINVAL;

	PRT_TRACE_BEGIN("\n");
	for (i = 0; i < vic->out_frame.fmt->memplanes; i++) {
		unsigned long size = vic->payload[i];

		if (vb2_plane_size(vb, i) < size) {
			v4l2_err(&vic->vfd,
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
int vic_prepare_addr(struct viss_dev *vic, struct vb2_buffer *vb,
		      struct viss_frame *frame, struct viss_addr *paddr, u8 ch)
{
	int ret = 0;
	u32 pix_size;
	u32 sub_size = 0;
	u32 y_ch_offset = 0;
	u32 cb_ch_offset = 0;

	if (vb == NULL || frame == NULL)
		return -EINVAL;

	PRT_TRACE_BEGIN("\n");
	pix_size = frame->f_width * frame->f_height;
	sub_size =  vic->inp_frame.sub_width *  vic->inp_frame.sub_height;

	PRT_DBG("memplanes=%d,colplanes=%d,pix_size=%d,sub_size=%d\n",
		frame->fmt->memplanes,
		frame->fmt->colplanes,
		pix_size, sub_size);
	/* channel sorting:== */
	if (!vic->sort) {
		switch (ch) {
		case 0:
			y_ch_offset = 0;
			cb_ch_offset = pix_size;
			break;
		case 1:
			y_ch_offset = vic->inp_frame.sub_width;
			cb_ch_offset = pix_size +  vic->inp_frame.sub_width;
			break;
		case 2:
			y_ch_offset = sub_size * 2;
			cb_ch_offset = pix_size + sub_size;
			break;
		case 3:
			y_ch_offset =  sub_size * 2 +  vic->inp_frame.sub_width;
			cb_ch_offset = pix_size + sub_size +
				vic->inp_frame.sub_width;
			break;
		}
	} else {
		/* channel sorting:=
		 *                 =
		 */
		y_ch_offset = sub_size * ch;
		cb_ch_offset = pix_size + (sub_size / 2) * ch;
	}

	if (vb->v4l2_buf.memory == V4L2_MEMORY_DMABUF)
		paddr->y = vb2_dma_contig_plane_dma_addr(vb, 0) +
					vb->v4l2_planes[0].data_offset +
					y_ch_offset;
	else
		paddr->y = vb2_dma_contig_plane_dma_addr(vb, 0) + y_ch_offset;

	if (frame->fmt->memplanes == 1) {
		switch (frame->fmt->colplanes) {
		case 1:
			paddr->cb = 0;
			paddr->cr = 0;
			break;
		case 2:
			/* decompose Y into Y/Cb */
			paddr->cb = paddr->y - y_ch_offset + cb_ch_offset;
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

	PRT_DBG("%d PHYS_ADDR: y=0x%X,cb=0x%X,cr=0x%X,ret=%d",
		ch, paddr->y, paddr->cb, paddr->cr, ret);

	PRT_TRACE_END("\n");
	return ret;
}

static void buffer_queue(struct vb2_buffer *vb)
{
	struct viss_buffer *vbuf
		= container_of(vb, struct viss_buffer, vb);
	struct viss_dev *vic = vb2_get_drv_priv(vb->vb2_queue);
	unsigned long flags;
	u32 ch = 0;

	PRT_TRACE_BEGIN("\n");
	spin_lock_irqsave(&vic->slock, flags);

	if (!vic->ch_number)
		vic_prepare_addr(vic, &vbuf->vb,
				&vic->out_frame, &vbuf->paddr, 0);
	else
		for (ch = 0; ch <= vic->ch_number; ch++)
			vic_prepare_addr(vic, &vbuf->vb,
				&vic->out_frame, &vic->ch_paddr[ch], ch);
	vic_pending_queue_add(vic, vbuf);

	if (vb2_is_streaming(&vic->vb_queue) &&
		list_is_singular(&vic->active_buf_q))
		vic_set_next_buf(vic);

	spin_unlock_irqrestore(&vic->slock, flags);
	PRT_TRACE_END("\n");
}

static const struct vb2_ops vic_qops = {
	.queue_setup	 = queue_setup,
	.buf_prepare	 = buffer_prepare,
	.buf_queue	 = buffer_queue,
	.wait_prepare	 = vb2_ops_wait_prepare,
	.wait_finish	 = vb2_ops_wait_finish,
	.start_streaming = start_streaming,
	.stop_streaming	 = stop_streaming,
};

static void vic_clear_event_counters(struct viss_dev *vic)
{
	unsigned long flags;

	PRT_TRACE_BEGIN("\n");
	spin_lock_irqsave(&vic->slock, flags);
	memset(vic->events, 0, sizeof(vic->events));
	spin_unlock_irqrestore(&vic->slock, flags);
	PRT_TRACE_END("\n");
}

static int vic_open(struct file *file)
{
	struct viss_dev *vic = video_drvdata(file);
	struct media_entity *me = &vic->vfd.entity;
	int ret;

	PRT_TRACE_BEGIN("\n");
	mutex_lock(&me->parent->graph_mutex);

	mutex_lock(&vic->lock);
	if (atomic_read(&vic->out_path) != VISS_IO_DMA) {
		ret = -EBUSY;
		goto unlock;
	}

	set_bit(ST_PREV_IN_USE, &vic->state);
	ret = pm_runtime_get_sync(&vic->pdev->dev);
	if (ret < 0)
		goto unlock;

	ret = v4l2_fh_open(file);
	if (ret != 0) {
		pm_runtime_put_sync(&vic->pdev->dev);
		goto unlock;
	}

	if (v4l2_fh_is_singular_file(file) &&
		(atomic_read(&vic->out_path) == VISS_IO_DMA)) {
		ret = viss_pipeline_call(vic, open, &vic->pipeline,
					 &vic->vfd.entity, true);
		if (ret < 0) {
			clear_bit(ST_PREV_IN_USE, &vic->state);
			pm_runtime_put_sync(&vic->pdev->dev);
			v4l2_fh_release(file);
		} else {
			vic_clear_event_counters(vic);
			vic->ref_count++;
		}
	}

unlock:
	mutex_unlock(&vic->lock);
	mutex_unlock(&me->parent->graph_mutex);
	PRT_TRACE_END("\n");
	return ret;
}

static int vic_release(struct file *file)
{
	struct viss_dev *vic = video_drvdata(file);

	PRT_TRACE_BEGIN("\n");
	csp_vic_dump_regs(__func__);
	mutex_lock(&vic->lock);

	if (v4l2_fh_is_singular_file(file) &&
	    atomic_read(&vic->out_path) == VISS_IO_DMA) {
		if (vic->streaming) {
			media_entity_pipeline_stop(&vic->vfd.entity);
			vic->streaming = false;
		}
		clear_bit(ST_PREV_IN_USE, &vic->state);
		vic_stop_capture(vic, false);
		viss_pipeline_call(vic, close, &vic->pipeline);
		vic->ref_count--;
	}

	vb2_fop_release(file);
	pm_runtime_put(&vic->pdev->dev);
	clear_bit(ST_PREV_SUSPENDED, &vic->state);

	mutex_unlock(&vic->lock);
	PRT_TRACE_END("\n");
	return 0;
}

static const struct v4l2_file_operations vic_fops = {
	.owner		= THIS_MODULE,
	.open		= vic_open,
	.release	= vic_release,
	.poll		= vb2_fop_poll,
	.unlocked_ioctl	= video_ioctl2,
	.mmap		= vb2_fop_mmap,
};

/*
 * Format and crop negotiation helpers
 */

static const struct viss_fmt *vic_try_format(struct viss_dev *vic,
					u32 *width, u32 *height,
					u32 *code, u32 *fourcc, int pad)
{
	struct viss_drvdata *dd = vic->dd;
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
		v4l_bound_align_image(width, 8, vic->inp_frame.rect.width,
				      ffs(dd->out_width_align) - 1,
				      height, 0, vic->inp_frame.rect.height,
				      0, 0);
		flags = vic->inp_frame.fmt->flags;
	}
#else
	v4l_bound_align_image(width, 8, dd->max_width,
			      ffs(dd->out_width_align) - 1,
			      height, 0, dd->max_height, 0, 0);
	if (pad != VISS_SD_PAD_SINK)
		flags = vic->inp_frame.fmt->flags;
#endif

	fmt = vic_find_format(fourcc, code, flags, 0);
	if (WARN_ON(!fmt))
		return NULL;

	if (code)
		*code = fmt->mbus_code;
	if (fourcc)
		*fourcc = fmt->fourcc;

	PRT_DBG("code: 0x%x, %dx%d\n", code ? *code : 0, *width, *height);

	PRT_TRACE_END("\n");
	return fmt;
}

static void vic_try_crop(struct viss_dev *vic, struct v4l2_rect *r)
{
	struct viss_frame *frame = &vic->inp_frame;

	PRT_TRACE_BEGIN("\n");
	v4l_bound_align_image(&r->width, 0, frame->f_width, 0,
			      &r->height, 0, frame->f_height, 0, 0);

	/* Adjust left/top if cropping rectangle got out of bounds */
	r->left = clamp_t(u32, r->left, 0, frame->f_width - r->width);
	r->left = round_down(r->left, vic->dd->win_hor_offs_align);
	r->top  = clamp_t(u32, r->top, 0, frame->f_height - r->height);

	PRT_DBG("(%d,%d)/%dx%d, sink fmt: %dx%d\n",
		 r->left, r->top, r->width, r->height,
		 frame->f_width, frame->f_height);
	PRT_TRACE_END("\n");
}

static void vic_try_compose(struct viss_dev *vic, struct v4l2_rect *r)
{
	struct viss_frame *frame = &vic->out_frame;
	struct v4l2_rect *crop_rect = &vic->inp_frame.rect;

	PRT_TRACE_BEGIN("\n");
	/* Scaling is not supported so we enforce compose rectangle size
	   same as size of the sink crop rectangle. */
	r->width = crop_rect->width;
	r->height = crop_rect->height;

	/* TODO */

	/* Adjust left/top if the composing rectangle got out of bounds */
	r->left = clamp_t(u32, r->left, 0, frame->f_width - r->width);
	r->left = round_down(r->left, vic->dd->out_hor_offs_align);
	r->top  = clamp_t(u32, r->top, 0, vic->out_frame.f_height - r->height);

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
	struct viss_dev *vic = video_drvdata(file);
	struct device *dev = &vic->pdev->dev;
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

static int vic_enum_fmt_mplane(struct file *file, void *priv,
				     struct v4l2_fmtdesc *f)
{
	const struct viss_fmt *fmt;

	if (f->index >= ARRAY_SIZE(vic_formats))
		return -EINVAL;

	PRT_TRACE_BEGIN("\n");
	fmt = &vic_formats[f->index];
	strlcpy(f->description, fmt->name, sizeof(f->description));
	f->pixelformat = fmt->fourcc;

	PRT_TRACE_END("\n");
	return 0;
}

static int vic_g_fmt_mplane(struct file *file, void *fh,
				  struct v4l2_format *f)
{
	struct viss_dev *vic = video_drvdata(file);
	struct v4l2_pix_format_mplane *pixm = &f->fmt.pix_mp;
	struct viss_frame *frame = &vic->out_frame;
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
		pixm->plane_fmt[i].bytesperline = vic->bytesperline[i];
		pixm->plane_fmt[i].sizeimage = vic->payload[i];
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
static int viss_pipeline_try_format(struct viss_dev *vic,
				    struct v4l2_mbus_framefmt *tfmt,
				    const struct viss_fmt **fmt_id,
				    bool set)
{
	struct v4l2_subdev *sd = vic->pipeline.subdevs[IDX_SENSOR];
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
	 * assume that each subdev prior the vic->subdev have only one source
	 */

	while (1) {
		/* formats that sensor may support have flag FMT_FLAGS_CAM */
		vfmt = vic_find_format(NULL, mf->code != 0 ? &mf->code : NULL,
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
		while (me != &vic->subdev.entity) {
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
		vfmt = vic_try_format(vic, &tfmt->width, &tfmt->height,
					NULL, &fcc, VISS_SD_PAD_SINK);
		vfmt = vic_try_format(vic, &tfmt->width, &tfmt->height,
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
		 * multi-planar formats are added to the vic_formats[]
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

static int vic_try_fmt_mplane(struct file *file, void *fh,
				   struct v4l2_format *f)
{
	struct v4l2_pix_format_mplane *pix = &f->fmt.pix_mp;
	struct viss_dev *vic = video_drvdata(file);
	const struct viss_fmt *inp_fmt = vic->inp_frame.fmt;
	struct v4l2_mbus_framefmt mf;
	const struct viss_fmt *vfmt = NULL;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");
	viss_md_graph_lock(vic);
	mutex_lock(&vic->lock);

	/*
	 * In case of the format that have flag FMT_FLAGS_CAM output format
	 * must match its camera interface input format.
	 */
	if (inp_fmt->flags & FMT_FLAGS_CAM)
		vfmt = inp_fmt;
	else {
		vfmt = vic_try_format(vic, &pix->width, &pix->height,
					NULL, &pix->pixelformat,
					VISS_SD_PAD_SOURCE_DMA);
	}

	if (!vfmt) {
		ret = -EINVAL;
		goto unlock;
	}

	if (!vic->user_subdev_api) {
		mf.width = pix->width;
		mf.height = pix->height;
		mf.code = vfmt->mbus_code;
		viss_pipeline_try_format(vic, &mf, &vfmt, false);
		pix->width = mf.width;
		pix->height = mf.height;
		if (vfmt)
			pix->pixelformat = vfmt->fourcc;
	}

	viss_adjust_mplane_format(vfmt, pix->width, pix->height, pix);

unlock:
	mutex_unlock(&vic->lock);
	viss_md_graph_unlock(vic);

	PRT_TRACE_END("\n");
	return ret;
}

static int __vic_set_format(struct viss_dev *vic,
				     struct v4l2_format *f)
{
	struct v4l2_pix_format_mplane *pixm = &f->fmt.pix_mp;
	struct v4l2_mbus_framefmt *mf = &vic->ci_fmt;
	struct viss_frame *ff = &vic->out_frame;
	struct viss_frame *inf = &vic->inp_frame;
	const struct viss_fmt *s_fmt = NULL;
	int ret, i;

	if (vb2_is_busy(&vic->vb_queue))
		return -EBUSY;

	PRT_TRACE_BEGIN("\n");
	/* Try the format at the DMA output */
	ff->fmt = vic_try_format(vic, &pixm->width, &pixm->height,
				 NULL, &pixm->pixelformat,
				 VISS_SD_PAD_SOURCE_DMA);
	if (!ff->fmt)
		return -EINVAL;

	/* Try to match format at the host and the sensor */
	if (!vic->user_subdev_api) {
		mf->code   = ff->fmt->mbus_code;
		mf->width  = pixm->width;
		mf->height = pixm->height;
		ret = viss_pipeline_try_format(vic, mf, &s_fmt, true);
		if (ret)
			return ret;

		pixm->width  = mf->width;
		pixm->height = mf->height;
	}

	viss_adjust_mplane_format(ff->fmt, pixm->width, pixm->height, pixm);

	vic->bpl = 0;
	for (i = 0; i < ff->fmt->memplanes; i++) {
		vic->bytesperline[i] = pixm->plane_fmt[i].bytesperline;
		vic->payload[i] = pixm->plane_fmt[i].sizeimage;
		vic->bpl += pixm->plane_fmt[i].bytesperline;
	}

	ff->f_width = pixm->width;
	ff->f_height = pixm->height;

	inf->rect.left = 0;
	inf->rect.top = 0;
	inf->rect.width = pixm->width;
	inf->rect.height = pixm->height;

	/* Reset cropping and set format at the camera interface input */
	if (!vic->user_subdev_api) {
		vic->inp_frame.fmt = s_fmt;
		vic->inp_frame.f_width = pixm->width;
		vic->inp_frame.f_height = pixm->height;
		vic->inp_frame.rect.left = 0;
		vic->inp_frame.rect.top = 0;
	}

	PRT_TRACE_END("\n");
	return ret;
}

static int vic_s_fmt_mplane(struct file *file, void *priv,
				  struct v4l2_format *f)
{
	struct viss_dev *vic = video_drvdata(file);
	int ret;

	PRT_TRACE_BEGIN("\n");
	viss_md_graph_lock(vic);
	mutex_lock(&vic->lock);
	/*
	 * The graph is walked within __vic_set_format() to set
	 * the format at subdevs thus the graph mutex needs to be held at
	 * this point and acquired before the video mutex, to avoid  AB-BA
	 * deadlock when viss_md_link_notify() is called by other thread.
	 * Ideally the graph walking and setting format at the whole pipeline
	 * should be removed from this driver and handled in userspace only.
	 */
	ret = __vic_set_format(vic, f);

	mutex_unlock(&vic->lock);
	viss_md_graph_unlock(vic);
	PRT_TRACE_END("\n");
	return ret;
}

static int vic_enum_input(struct file *file, void *priv,
			       struct v4l2_input *i)
{
	struct viss_dev *vic = video_drvdata(file);
	struct v4l2_subdev *sd = vic->pipeline.subdevs[IDX_SENSOR];

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

static int vic_s_input(struct file *file, void *priv, unsigned int i)
{
	PRT_TRACE_BEGIN("\n");
	/* TODO: support up to 4 inputs */
	return i == 0 ? i : -EINVAL;
}

static int vic_g_input(struct file *file, void *priv, unsigned int *i)
{
	PRT_TRACE_BEGIN("\n");
	*i = 0;
	PRT_TRACE_END("\n");
	return 0;
}

static int vic_enum_framesizes(struct file *file, void *fh,
				struct v4l2_frmsizeenum *fsize)
{
	struct viss_dev *vic = video_drvdata(file);
	struct v4l2_subdev *sd = vic->pipeline.subdevs[IDX_SENSOR];

	PRT_DBG("sd->name=%s\n", sd->name);
	return v4l2_subdev_call(sd, video, enum_framesizes, fsize);
}

static int vic_enum_frameintervals(struct file *file, void *fh,
				struct v4l2_frmivalenum *fival)
{
	struct viss_dev *vic = video_drvdata(file);
	struct v4l2_subdev *sd = vic->pipeline.subdevs[IDX_SENSOR];

	PRT_DBG("sd->name=%s\n", sd->name);
	return v4l2_subdev_call(sd, video, enum_frameintervals, fival);
}

static int vic_g_parm(struct file *file, void *fh,
			struct v4l2_streamparm *a)
{
	struct viss_dev *vic = video_drvdata(file);
	struct v4l2_subdev *sd = vic->pipeline.subdevs[IDX_SENSOR];

	PRT_DBG("sd->name=%s\n", sd->name);
	return v4l2_subdev_call(sd, video, g_parm, a);
}

static int vic_s_parm(struct file *file, void *fh,
			struct v4l2_streamparm *a)
{
	struct viss_dev *vic = video_drvdata(file);
	struct v4l2_subdev *sd = vic->pipeline.subdevs[IDX_SENSOR];

	PRT_DBG("sd->name=%s\n", sd->name);
	return v4l2_subdev_call(sd, video, s_parm, a);
}

static int viss_pipeline_validate(struct viss_dev *vic)
{
	struct v4l2_subdev *sd = &vic->subdev;
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

		/* Don't call VIC subdev operation to avoid nested locking */
		if (sd == &vic->subdev) {
#if 0
			struct viss_frame *ff = &vic->out_frame;
			sink_fmt.format.width = ff->f_width;
			sink_fmt.format.height = ff->f_height;
			sink_fmt.format.code = vic->inp_frame.fmt->mbus_code;
#endif
			struct viss_frame *ff = &vic->inp_frame;
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

static int vic_streamon(struct file *file, void *priv,
			      enum v4l2_buf_type type)
{
	struct viss_source_info *si;

	struct viss_dev *vic = video_drvdata(file);
	struct media_entity *entity = &vic->vfd.entity;
	struct viss_pipeline *p = &vic->pipeline;
	int ret;

	if (vic_active(vic))
		return -EBUSY;

	PRT_TRACE_BEGIN("\n");
	ret = media_entity_pipeline_start(entity, p->m_pipeline);
	if (ret < 0)
		return ret;

	if (vic->user_subdev_api) {
		ret = viss_pipeline_validate(vic);
		if (ret < 0)
			goto err_p_stop;
	}

	vic->sensor = __find_remote_sensor(&vic->subdev.entity);
	si = v4l2_get_subdev_hostdata(vic->sensor);
	vic->inp_frame.sub_height = si->sub_height;
	vic->inp_frame.sub_width = si->sub_width;
	vic->ch_number = si->ch_number;
	vic->interlaced = si->interlaced;
	vic->sort = si->sort;
	/* PRT_ERR("vic_streamon\n"); */
	ret = vb2_ioctl_streamon(file, priv, type);
	if (ret == 0) {
		vic->streaming = true;
		PRT_TRACE_END("\n");
		return ret;
	}

err_p_stop:
	media_entity_pipeline_stop(entity);
	return ret;
}

static int vic_streamoff(struct file *file, void *priv,
			       enum v4l2_buf_type type)
{
	struct viss_dev *vic = video_drvdata(file);
	int ret;

	PRT_TRACE_BEGIN("\n");
	ret = vb2_ioctl_streamoff(file, priv, type);
	if (ret < 0)
		return ret;

	media_entity_pipeline_stop(&vic->vfd.entity);
	vic->streaming = false;
	PRT_TRACE_END("\n");
	return 0;
}

static int vic_reqbufs(struct file *file, void *priv,
			     struct v4l2_requestbuffers *reqbufs)
{
	struct viss_dev *vic = video_drvdata(file);
	int ret;

	PRT_TRACE_BEGIN("\n");
	reqbufs->count = max_t(u32, VISS_REQ_BUFS_MIN, reqbufs->count);
	ret = vb2_ioctl_reqbufs(file, priv, reqbufs);
	if (!ret)
		vic->reqbufs_count = reqbufs->count;

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

static int vic_g_selection(struct file *file, void *fh,
				 struct v4l2_selection *sel)
{
	struct viss_dev *vic = video_drvdata(file);
	struct viss_frame *f = &vic->out_frame;
	struct viss_frame *ff = &vic->inp_frame;

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
		sel->r.width = ff->f_width;
		sel->r.height = ff->f_height;
		return 0;

	case V4L2_SEL_TGT_COMPOSE:
		sel->r = f->rect;
		return 0;
	case V4L2_SEL_TGT_CROP:
		sel->r = ff->rect;
		return 0;
	}

	return -EINVAL;
}

static int vic_s_selection(struct file *file, void *fh,
				 struct v4l2_selection *sel)
{
	struct viss_dev *vic = video_drvdata(file);
	struct v4l2_rect rect = sel->r;
	struct viss_frame *f;
	unsigned long flags;

	if (sel->type != V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE)
		return -EINVAL;

	if (sel->target == V4L2_SEL_TGT_COMPOSE) {
		vic_try_compose(vic, &rect);
		f = &vic->out_frame;
	} else if (sel->target == V4L2_SEL_TGT_CROP) {
		vic_try_crop(vic, &rect);
		f = &vic->inp_frame;
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
	spin_lock_irqsave(&vic->slock, flags);
	f->rect = rect;
	set_bit(ST_PREV_CONFIG, &vic->state);
	spin_unlock_irqrestore(&vic->slock, flags);

	PRT_TRACE_END("\n");
	return 0;
}

static int vic_get_sensor_dev(struct file *file, void *fh,
				 int *arg)
{
	struct viss_dev *vic = video_drvdata(file);
	struct v4l2_subdev *sd = vic->pipeline.subdevs[IDX_SENSOR];

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

static long vic_default(struct file *file, void *fh,
		bool valid_prio, unsigned int cmd, void *arg)
{
	int ret = 0;

	switch (cmd) {
	case VIDIOC_VISS_GET_SENSOR_DEV:
		ret = vic_get_sensor_dev(file, fh, arg);
		break;
	default:
		return -ENOIOCTLCMD;
	}

	return ret;
}

static const struct v4l2_ioctl_ops vic_ioctl_ops = {
	.vidioc_querycap		= viss_vidioc_querycap_capture,

	.vidioc_enum_fmt_vid_cap_mplane	= vic_enum_fmt_mplane,
	.vidioc_try_fmt_vid_cap_mplane	= vic_try_fmt_mplane,
	.vidioc_s_fmt_vid_cap_mplane	= vic_s_fmt_mplane,
	.vidioc_g_fmt_vid_cap_mplane	= vic_g_fmt_mplane,

	.vidioc_enum_framesizes		= vic_enum_framesizes,
	.vidioc_enum_frameintervals	= vic_enum_frameintervals,

	.vidioc_g_parm			= vic_g_parm,
	.vidioc_s_parm			= vic_s_parm,

	.vidioc_reqbufs			= vic_reqbufs,
	.vidioc_querybuf		= vb2_ioctl_querybuf,
	.vidioc_prepare_buf		= vb2_ioctl_prepare_buf,
	.vidioc_create_bufs		= vb2_ioctl_create_bufs,
	.vidioc_qbuf			= vb2_ioctl_qbuf,
	.vidioc_dqbuf			= vb2_ioctl_dqbuf,
	.vidioc_expbuf			= vb2_ioctl_expbuf,

	.vidioc_streamon		= vic_streamon,
	.vidioc_streamoff		= vic_streamoff,

	.vidioc_g_selection		= vic_g_selection,
	.vidioc_s_selection		= vic_s_selection,

	.vidioc_enum_input		= vic_enum_input,
	.vidioc_s_input			= vic_s_input,
	.vidioc_g_input			= vic_g_input,

	.vidioc_default			= vic_default,
};

/* Capture subdev media entity operations */
static int vic_link_setup(struct media_entity *entity,
				const struct media_pad *local,
				const struct media_pad *remote, u32 flags)
{
	struct v4l2_subdev *sd = media_entity_to_v4l2_subdev(entity);
	struct viss_dev *vic = v4l2_get_subdevdata(sd);
	unsigned int remote_ent_type = media_entity_type(remote->entity);
	int ret = 0;

	if (WARN_ON(vic == NULL))
		return 0;

	PRT_TRACE_BEGIN("\n");
	PRT_DBG("%s --> %s, flags: 0x%x. source_id: 0x%x, local->index: %u.\n",
		 remote->entity->name, local->entity->name,
		 flags, vic->source_subdev_grp_id, local->index);

	mutex_lock(&vic->lock);

	switch (local->index) {
	case VISS_SD_PAD_SINK:
		if (remote_ent_type != MEDIA_ENT_T_V4L2_SUBDEV) {
			ret = -EINVAL;
			break;
		}
		if (flags & MEDIA_LNK_FL_ENABLED) {
			if (vic->source_subdev_grp_id == 0)
				vic->source_subdev_grp_id = sd->grp_id;
			else
				ret = -EBUSY;
		} else {
			vic->source_subdev_grp_id = 0;
			vic->sensor = NULL;
		}
		break;

	case VISS_SD_PAD_SOURCE_DMA:
		if (!(flags & MEDIA_LNK_FL_ENABLED))
			atomic_set(&vic->out_path, VISS_IO_NONE);
		else if (remote_ent_type == MEDIA_ENT_T_DEVNODE)
			atomic_set(&vic->out_path, VISS_IO_DMA);
		else
			ret = -EINVAL;
		break;

	case VISS_SD_PAD_SOURCE_ISP:
		if (!(flags & MEDIA_LNK_FL_ENABLED))
			atomic_set(&vic->out_path, VISS_IO_NONE);
		else if (remote_ent_type == MEDIA_ENT_T_V4L2_SUBDEV)
			atomic_set(&vic->out_path, VISS_IO_ISP);
		else
			ret = -EINVAL;
		break;

	default:
		v4l2_err(sd, "Invalid pad index\n");
		ret = -EINVAL;
	}
	mb();

	mutex_unlock(&vic->lock);
	PRT_TRACE_END("\n");
	return ret;
}

static const struct media_entity_operations vic_subdev_media_ops = {
	.link_setup = vic_link_setup,
};

static int vic_subdev_enum_mbus_code(struct v4l2_subdev *sd,
				struct v4l2_subdev_fh *fh,
				struct v4l2_subdev_mbus_code_enum *code)
{
	const struct viss_fmt *fmt;

	PRT_TRACE_BEGIN("\n");
	fmt = vic_find_format(NULL, NULL, 0, code->index);
	if (!fmt)
		return -EINVAL;
	code->code = fmt->mbus_code;
	PRT_TRACE_END("\n");
	return 0;
}

static int vic_subdev_get_fmt(struct v4l2_subdev *sd,
				    struct v4l2_subdev_fh *fh,
				    struct v4l2_subdev_format *fmt)
{
	struct viss_dev *vic = v4l2_get_subdevdata(sd);
	struct v4l2_mbus_framefmt *mf = &fmt->format;
	struct viss_frame *f = &vic->inp_frame;

	PRT_TRACE_BEGIN("\n");
	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		mf = v4l2_subdev_get_try_format(fh, fmt->pad);
		fmt->format = *mf;
		PRT_TRACE_END("\n");
		return 0;
	}

	mutex_lock(&vic->lock);

	if (fmt->pad == VISS_SD_PAD_SINK) {
		*mf = vic->ci_fmt;
	} else {
		mf->code = f->fmt->mbus_code;
		/* crop size */
		mf->width = f->rect.width;
		mf->height = f->rect.height;
	}

	mf->colorspace = V4L2_COLORSPACE_JPEG;
	mutex_unlock(&vic->lock);
	PRT_TRACE_END("\n");
	return 0;
}

static int vic_subdev_set_fmt(struct v4l2_subdev *sd,
				    struct v4l2_subdev_fh *fh,
				    struct v4l2_subdev_format *fmt)
{
	struct viss_dev *vic = v4l2_get_subdevdata(sd);
	struct v4l2_mbus_framefmt *mf = &fmt->format;
	struct viss_frame *sink = &vic->inp_frame;
	struct viss_frame *source = &vic->out_frame;
	const struct viss_fmt *vfmt;

	PRT_DBG("pad%d: code: 0x%x, %dx%d\n",
		fmt->pad, mf->code, mf->width, mf->height);

	PRT_TRACE_BEGIN("\n");
	mf->colorspace = V4L2_COLORSPACE_JPEG;
	mutex_lock(&vic->lock);

	if ((atomic_read(&vic->out_path) == VISS_IO_ISP &&
	    sd->entity.stream_count > 0) ||
	    (atomic_read(&vic->out_path) == VISS_IO_DMA &&
	    vb2_is_busy(&vic->vb_queue))) {
		mutex_unlock(&vic->lock);
		return -EBUSY;
	}

	vfmt = vic_try_format(vic, &mf->width, &mf->height,
				   &mf->code, NULL, fmt->pad);

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		mf = v4l2_subdev_get_try_format(fh, fmt->pad);
		*mf = fmt->format;
		mutex_unlock(&vic->lock);
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

	mutex_unlock(&vic->lock);
	PRT_TRACE_END("\n");
	return 0;
}

static int vic_subdev_get_selection(struct v4l2_subdev *sd,
					  struct v4l2_subdev_fh *fh,
					  struct v4l2_subdev_selection *sel)
{
	struct viss_dev *vic = v4l2_get_subdevdata(sd);
	struct viss_frame *f = &vic->inp_frame;
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

	mutex_lock(&vic->lock);
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
	mutex_unlock(&vic->lock);

	PRT_TRACE_END("\n");
	return 0;
}

static int vic_subdev_set_selection(struct v4l2_subdev *sd,
					  struct v4l2_subdev_fh *fh,
					  struct v4l2_subdev_selection *sel)
{
	struct viss_dev *vic = v4l2_get_subdevdata(sd);
	struct viss_frame *f = &vic->inp_frame;
	struct v4l2_rect *try_sel;

	if (sel->target != V4L2_SEL_TGT_CROP || sel->pad != VISS_SD_PAD_SINK)
		return -EINVAL;

	PRT_TRACE_BEGIN("\n");
	mutex_lock(&vic->lock);
	vic_try_crop(vic, &sel->r);

	if (sel->which == V4L2_SUBDEV_FORMAT_TRY) {
		try_sel = v4l2_subdev_get_try_crop(fh, sel->pad);
		if (IS_ERR_OR_NULL(try_sel)) {
			mutex_unlock(&vic->lock);
			return -EINVAL;
		}
		*try_sel = sel->r;
	} else {
		unsigned long flags;

		spin_lock_irqsave(&vic->slock, flags);
		f->rect = sel->r;
		/* Same crop rectangle on the source pad */
		vic->out_frame.rect = sel->r;
		set_bit(ST_PREV_CONFIG, &vic->state);
		spin_unlock_irqrestore(&vic->slock, flags);
	}
	mutex_unlock(&vic->lock);

	PRT_DBG("(%d,%d) %dx%d, f_w: %d, f_h: %d\n",
		 f->rect.left, f->rect.top, f->rect.width,
		 f->rect.height, f->f_width, f->f_height);

	PRT_TRACE_END("\n");
	return 0;
}

static int vic_subdev_s_stream(struct v4l2_subdev *sd, int on)
{
	struct viss_dev *vic = v4l2_get_subdevdata(sd);
	unsigned long flags;
	int ret = 0;
	u32 ch = 0;

	PRT_TRACE_BEGIN("\n");
	/*
	 * Find sensor subdev linked to VISS-VIC directly. This is
	 * required for configuration where VISS-VIC is used as a subdev
	 * only and feeds data internally to VISS-ISP.
	 * The pipeline links are protected through entity.stream_count
	 * so there is no need to take the media graph mutex here.
	 */
	vic->sensor = __find_remote_sensor(&sd->entity);

	if (atomic_read(&vic->out_path) != VISS_IO_ISP) {
		PRT_TRACE_END("\n");
		return -ENOIOCTLCMD;
	}

	mutex_lock(&vic->lock);
	if (on) {
		if (isp_is_offline(vic->sensor)) {
			struct viss_source_info *si =
				v4l2_get_subdev_hostdata(vic->sensor);
			struct viss_isp_sensor_fmt_info sensor_info;
			int w, h, d;

			v4l2_subdev_call(vic->sensor, core, ioctl,
				VIDIOC_VISS_ISP_SENSOR_FMT_INFO, &sensor_info);
			w = sensor_info.width + 2 * sensor_info.hoffset;
			h = sensor_info.height + 2 * sensor_info.voffset;
			d = (sensor_info.buswidth <= ISP_INPUT_8BITS) ? 1 : 2;
			offline_buf_pending[si->isp_id] = &vic->pending_buf_q;
			offline_buf_init(&vic->pdev->dev, w, h, d, si->isp_id);
		}

		spin_lock_irqsave(&vic->slock, flags);

		ret = vic_hw_init(vic, true);
		if (ret == 0)
			ret = vic_start_capture(vic);
		else
			csp_viss_top_vic_reset();
		spin_unlock_irqrestore(&vic->slock, flags);
	} else {
		set_bit(ST_PREV_OFF, &vic->state);
		if (isp_is_offline(vic->sensor)) {
			struct viss_source_info *si =
				v4l2_get_subdev_hostdata(vic->sensor);
			int isp_id = si->isp_id;

			csp_vic_reset();
			while (!list_empty(&vic->pending_buf_q))
				offline_buf_pop(&vic->pending_buf_q);

			while (!list_empty(&vic->active_buf_q))
				offline_buf_pop(&vic->active_buf_q);

			offline_buf_exit(&vic->pdev->dev, isp_id);
		}

		spin_lock_irqsave(&vic->slock, flags);
		for (ch = 0; ch <= vic->ch_number; ch++)
			csp_vic_dma_stop(VIC_DMA_CH0 + ch);
		csp_vic_disable();

		spin_unlock_irqrestore(&vic->slock, flags);

		clear_bit(ST_PREV_RUN, &vic->state);
	}
	mutex_unlock(&vic->lock);
	PRT_TRACE_END("\n");
	return ret;
}

static int vic_log_status(struct v4l2_subdev *sd)
{
	csp_vic_dump_regs(__func__);
	return 0;
}

static int vic_reset(struct v4l2_subdev *sd, u32 val)
{
	struct viss_dev *vic = v4l2_get_subdevdata(sd);
	unsigned long flags;

	PRT_INFO("called!\n");

	spin_lock_irqsave(&vic->slock, flags);
	if (val == 0) {
		csp_vic_disable();
	} else {
		csp_vic_enable();
		csp_vic_reset();
	}
	spin_unlock_irqrestore(&vic->slock, flags);

	return 0;
}

static const struct v4l2_subdev_pad_ops vic_subdev_pad_ops = {
	.enum_mbus_code		= vic_subdev_enum_mbus_code,
	.get_selection		= vic_subdev_get_selection,
	.set_selection		= vic_subdev_set_selection,
	.get_fmt		= vic_subdev_get_fmt,
	.set_fmt		= vic_subdev_set_fmt,
};

static const struct v4l2_subdev_video_ops vic_subdev_video_ops = {
	.s_stream		= vic_subdev_s_stream,
};

static const struct v4l2_subdev_core_ops vic_core_ops = {
	.log_status		= vic_log_status,
	.reset			= vic_reset,
};

static struct v4l2_subdev_ops vic_subdev_ops = {
	.core			= &vic_core_ops,
	.video			= &vic_subdev_video_ops,
	.pad			= &vic_subdev_pad_ops,
};

static int vic_subdev_registered(struct v4l2_subdev *sd)
{
	struct viss_dev *vic = v4l2_get_subdevdata(sd);
	struct vb2_queue *q = &vic->vb_queue;
	struct video_device *vfd = &vic->vfd;
	int ret = 0;

	PRT_TRACE_BEGIN("\n");
	memset(vfd, 0, sizeof(*vfd));

	vic->inp_frame.fmt = &vic_formats[0];
	vic->out_frame.fmt = &vic_formats[0];
	atomic_set(&vic->out_path, VISS_IO_DMA);

	snprintf(vfd->name, sizeof(vfd->name), "viss-vic.capture");

	vfd->fops = &vic_fops;
	vfd->ioctl_ops = &vic_ioctl_ops;
	vfd->v4l2_dev = sd->v4l2_dev;
	vfd->minor = -1;
	vfd->release = video_device_release_empty;
	vfd->queue = q;
#if 0
	vfd->lock = &vic->lock;
#endif
	vic->reqbufs_count = 0;

	INIT_LIST_HEAD(&vic->pending_buf_q);
	INIT_LIST_HEAD(&vic->active_buf_q);

	memset(q, 0, sizeof(*q));
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF;
	q->ops = &vic_qops;
	q->mem_ops = &vb2_dma_contig_memops;
	q->buf_struct_size = sizeof(struct viss_buffer);
	q->drv_priv = vic;
	q->timestamp_type = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &vic->lock;

	ret = vb2_queue_init(q);
	if (ret < 0)
		return ret;

	vic->vd_pad.flags = MEDIA_PAD_FL_SINK;
	ret = media_entity_init(&vfd->entity, 1, &vic->vd_pad, 0);
	if (ret < 0)
		return ret;

	video_set_drvdata(vfd, vic);
	vic->pipeline_ops = v4l2_get_subdev_hostdata(sd);

#if 0
	/*
	 * For proper order of acquiring/releasing the video
	 * and the graph mutex.
	 */
	v4l2_disable_ioctl_locking(vfd, VIDIOC_TRY_FMT);
	v4l2_disable_ioctl_locking(vfd, VIDIOC_S_FMT);
#endif

	ret = video_register_device(vfd, VFL_TYPE_GRABBER, vic->nr);
	if (ret < 0) {
		media_entity_cleanup(&vfd->entity);
		vic->pipeline_ops = NULL;
		return ret;
	}

	PRT_INFO("Registered %s as /dev/%s\n",
		  vfd->name, video_device_node_name(vfd));
	PRT_TRACE_END("\n");
	return 0;
}

static void vic_subdev_unregistered(struct v4l2_subdev *sd)
{
	struct viss_dev *vic = v4l2_get_subdevdata(sd);

	if (vic == NULL)
		return;

	if (video_is_registered(&vic->vfd)) {
		video_unregister_device(&vic->vfd);
		media_entity_cleanup(&vic->vfd.entity);
		vic->pipeline_ops = NULL;
	}
}

static const struct v4l2_subdev_internal_ops vic_subdev_internal_ops = {
	.registered = vic_subdev_registered,
	.unregistered = vic_subdev_unregistered,
};

static int vic_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct viss_dev *vic = container_of(ctrl->handler, struct viss_dev,
					      ctrl_handler);
	PRT_TRACE_BEGIN("\n");
	set_bit(ST_PREV_CONFIG, &vic->state);
	PRT_TRACE_END("\n");
	return 0;
}

static const struct v4l2_ctrl_ops vic_ctrl_ops = {
	.s_ctrl	= vic_s_ctrl,
};

static const struct v4l2_ctrl_config vic_ctrl = {
	.ops	= &vic_ctrl_ops,
	.id	= V4L2_CTRL_CLASS_USER | 0x1001,
	.type	= V4L2_CTRL_TYPE_BOOLEAN,
	.name	= "Test Pattern 640x480",
	.step	= 1,
};

static int vic_create_capture_subdev(struct viss_dev *vic)
{
	struct v4l2_ctrl_handler *handler = &vic->ctrl_handler;
	struct v4l2_subdev *sd = &vic->subdev;
	int ret;

	PRT_TRACE_BEGIN("\n");

	v4l2_subdev_init(sd, &vic_subdev_ops);
	sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	snprintf(sd->name, sizeof(sd->name), "VISS-VIC");

	vic->subdev_pads[VISS_SD_PAD_SINK].flags = MEDIA_PAD_FL_SINK;
	vic->subdev_pads[VISS_SD_PAD_SOURCE_DMA].flags = MEDIA_PAD_FL_SOURCE;
	vic->subdev_pads[VISS_SD_PAD_SOURCE_ISP].flags = MEDIA_PAD_FL_SOURCE;
	ret = media_entity_init(&sd->entity, VISS_SD_PADS_NUM,
				vic->subdev_pads, 0);
	if (ret)
		return ret;

	v4l2_ctrl_handler_init(handler, 1);
	vic->test_pattern = v4l2_ctrl_new_custom(handler, &vic_ctrl, NULL);
	if (handler->error) {
		media_entity_cleanup(&sd->entity);
		return handler->error;
	}

	sd->ctrl_handler = handler;
	sd->internal_ops = &vic_subdev_internal_ops;
	sd->entity.ops = &vic_subdev_media_ops;
	sd->owner = THIS_MODULE;
	v4l2_set_subdevdata(sd, vic);

	PRT_TRACE_END("\n");
	return 0;
}

static void vic_unregister_capture_subdev(struct viss_dev *vic)
{
	struct v4l2_subdev *sd = &vic->subdev;

	PRT_TRACE_BEGIN("\n");
	v4l2_device_unregister_subdev(sd);
	media_entity_cleanup(&sd->entity);
	v4l2_ctrl_handler_free(&vic->ctrl_handler);
	v4l2_set_subdevdata(sd, NULL);
	PRT_TRACE_END("\n");
}

static void vic_clk_put(struct viss_dev *vic)
{
	/* TODO */
}

static int vic_clk_get(struct viss_dev *vic)
{
	int ret = 0;

	/* TODO */

	return ret;
}

static const struct of_device_id vic_of_match[];

/* do things when waiting for hardware interrupts timeout. */
static void lombo_vic_to_timer(unsigned long data)
{
	struct viss_dev *vic;
	unsigned long flags;

	vic = (struct viss_dev *)data;

	spin_lock_irqsave(&vic->slock, flags);

	if (test_bit(ST_PREV_RUN, &vic->state)) {
		if (!list_empty(&vic->active_buf_q) &&
			!list_is_singular(&vic->active_buf_q)) {
			vic->to_cnt++;

			if ((vic->to_cnt % 10) == 0)
				csp_vic_reset();

			if ((vic->to_cnt == 10) ||
				((vic->to_cnt % 100) == 0)) {
				spin_unlock_irqrestore(&vic->slock, flags);
				PRT_ERR("vic Timeout waiting interrupt.\n");
				csp_vic_dump_regs(__func__);
				spin_lock_irqsave(&vic->slock, flags);
			}
		}
	}

	mod_timer(&vic->timer, jiffies + HZ / 10);

	mmiowb();
	spin_unlock_irqrestore(&vic->slock, flags);
}

static int vic_probe(struct platform_device *pdev)
{
	struct viss_drvdata *drv_data = NULL;
	struct device *dev = &pdev->dev;
	const struct of_device_id *of_id;
	struct viss_dev *vic;
	struct resource *res;
	int ret;

	PRT_TRACE_BEGIN("\n");

	if (dev->of_node == NULL) {
		PRT_DBG("no device data specified\n");
		return -ENOENT;
	}

	vic = devm_kzalloc(dev, sizeof(*vic), GFP_KERNEL);
	if (!vic)
		return -ENOMEM;

	vic->nr = of_alias_get_id(dev->of_node, "video");

	of_id = of_match_node(vic_of_match, dev->of_node);
	if (of_id)
		drv_data = (struct viss_drvdata *)of_id->data;

	if (!drv_data)
		return -EINVAL;

	vic->dd = drv_data;
	vic->pdev = pdev;

	PRT_DBG("vic->dd->max_width=%d\n", vic->dd->max_width);
	PRT_DBG("vic->dd->max_height=%d\n", vic->dd->max_height);
	PRT_DBG("vic->dd->out_width_align=%d\n", vic->dd->out_width_align);
	PRT_DBG("vic->dd->win_hor_offs_align=%d\n",
		vic->dd->win_hor_offs_align);
	PRT_DBG("vic->dd->out_hor_offs_align=%d\n",
		vic->dd->out_hor_offs_align);

	init_waitqueue_head(&vic->irq_queue);
	spin_lock_init(&vic->slock);
	mutex_init(&vic->lock);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	vic->regs = devm_ioremap_resource(dev, res);
	if (IS_ERR(vic->regs))
		return PTR_ERR(vic->regs);

	csp_vic_set_register_base(vic->regs);

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (res == NULL) {
		dev_err(dev, "Failed to get IRQ resource\n");
		return -ENXIO;
	}

	ret = vic_clk_get(vic);
	if (ret)
		return ret;

	setup_timer(&vic->timer, lombo_vic_to_timer, (unsigned long)vic);
	/* PRT_ERR("res->start (%d)\n", res->start); */
	ret = devm_request_irq(dev, res->start, vic_irq_handler,
			       0, dev_name(dev), vic);
	if (ret) {
		dev_err(dev, "Failed to install irq (%d)\n", ret);
		goto err_clk;
	}

	/* The video node will be created within the subdev's registered() op */
	ret = vic_create_capture_subdev(vic);
	if (ret)
		goto err_clk;

	ret = device_create_file(&pdev->dev, &dev_attr_vic_reparse);
	if (ret) {
		PRT_ERR("device_create_file failed (%d)\n", ret);
		goto err_sd;
	}

	platform_set_drvdata(pdev, vic);
	pm_runtime_enable(dev);
	ret = pm_runtime_get_sync(dev);
	if (ret < 0)
		goto err_sd;

	vic->alloc_ctx = vb2_dma_contig_init_ctx(dev);
	if (IS_ERR(vic->alloc_ctx)) {
		ret = PTR_ERR(vic->alloc_ctx);
		goto err_pm;
	}
	pm_runtime_put(dev);

	PRT_DBG("VISS-VIC registered successfully\n");
	return 0;
err_pm:
	pm_runtime_put(dev);
err_sd:
	vic_unregister_capture_subdev(vic);
err_clk:
	vic_clk_put(vic);
	PRT_TRACE_END("\n");
	return ret;
}

#ifdef CONFIG_PM_RUNTIME
static int vic_runtime_resume(struct device *dev)
{
	/* struct viss_dev *vic = dev_get_drvdata(dev); */

	/* TODO */

	return 0;
}

static int vic_runtime_suspend(struct device *dev)
{
	/* struct viss_dev *vic = dev_get_drvdata(dev); */

	/* TODO */
	return 0;
}
#endif

#ifdef CONFIG_PM_SLEEP
static int vic_resume(struct device *dev)
{
	struct viss_dev *vic = dev_get_drvdata(dev);
	unsigned long flags;
	int ret = 0;
	PRT_TRACE_BEGIN("\n");
	spin_lock_irqsave(&vic->slock, flags);
	if (!test_and_clear_bit(ST_PREV_LPM, &vic->state) ||
	    !test_bit(ST_PREV_IN_USE, &vic->state)) {
		spin_unlock_irqrestore(&vic->slock, flags);
		PRT_TRACE_END("\n");
		return 0;
	}

	if (atomic_read(&vic->out_path) == VISS_IO_DMA) {
		del_timer(&vic->timer);
		vic->to_cnt = 0;
	}

	csp_vic_reset();
	csp_viss_top_vic_reset();
	spin_unlock_irqrestore(&vic->slock, flags);

	if (!test_and_clear_bit(ST_PREV_SUSPENDED, &vic->state)) {
		PRT_TRACE_END("\n");
		return 0;
	}

	INIT_LIST_HEAD(&vic->active_buf_q);
	viss_pipeline_call(vic, open, &vic->pipeline,
			   &vic->vfd.entity, false);

	spin_lock_irqsave(&vic->slock, flags);
	vic_hw_init(vic, atomic_read(&vic->out_path) == VISS_IO_ISP);
	clear_bit(ST_PREV_SUSPENDED, &vic->state);

	if (vb2_is_streaming(&vic->vb_queue)) {
		ret = vic_start_capture(vic);
		spin_unlock_irqrestore(&vic->slock, flags);
		if (ret != 0)
			goto out;

		ret = viss_pipeline_call(vic, set_stream, &vic->pipeline, 1);
		if (ret != 0)
			goto out;

		mod_timer(&vic->timer, jiffies + HZ / 10);
		vic->to_cnt = 0;
	} else {
		spin_unlock_irqrestore(&vic->slock, flags);
	}

out:
	PRT_TRACE_END("\n");
	return ret;
}

static int vic_suspend(struct device *dev)
{
	struct viss_dev *vic = dev_get_drvdata(dev);
	bool suspend = test_bit(ST_PREV_IN_USE, &vic->state);
	int ret;

	PRT_TRACE_BEGIN("\n");
	if (test_and_set_bit(ST_PREV_LPM, &vic->state)) {
		PRT_TRACE_END("\n");
		return 0;
	}

	if (!suspend)
		return 0;
	ret = vic_stop_capture(vic, suspend);
	if (ret < 0 || !test_bit(ST_PREV_IN_USE, &vic->state))
		return ret;

	return viss_pipeline_call(vic, close, &vic->pipeline);
}
#endif /* CONFIG_PM_SLEEP */

static int vic_remove(struct platform_device *pdev)
{
	struct viss_dev *vic = platform_get_drvdata(pdev);
	struct device *dev = &pdev->dev;

	PRT_TRACE_BEGIN("\n");
	pm_runtime_disable(dev);
	pm_runtime_set_suspended(dev);
	vic_unregister_capture_subdev(vic);
	vb2_dma_contig_cleanup_ctx(vic->alloc_ctx);
	vic_clk_put(vic);

	del_timer_sync(&vic->timer);

	dev_info(dev, "Driver unloaded\n");
	PRT_TRACE_END("\n");
	return 0;
}

static const struct dev_pm_ops vic_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(vic_suspend, vic_resume)
	SET_RUNTIME_PM_OPS(vic_runtime_suspend, vic_runtime_resume,
			   NULL)
};

static struct viss_drvdata vic_drvdata_n7 = {
	.max_width		= 8192,
	.max_height		= 8192,
	.out_width_align	= 8,
	.win_hor_offs_align	= 2,
	.out_hor_offs_align	= 8,
};

static const struct of_device_id vic_of_match[] = {
	{
		.compatible = "lombo,n7-viss-vic",
		.data = &vic_drvdata_n7,
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, vic_of_match);

static struct platform_driver vic_driver = {
	.probe		= vic_probe,
	.remove		= vic_remove,
	.driver = {
		.of_match_table = vic_of_match,
		.name		= VIC_DRV_NAME,
		.owner		= THIS_MODULE,
		.pm		= &vic_pm_ops,
	}
};
module_platform_driver(vic_driver);
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" VIC_DRV_NAME);
