/*
 * vc-test.c - clk test driver
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
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
#include <linux/stat.h>
#include <linux/fs.h>
#include <asm/unistd.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/ioctl.h>

#include <linux/videodev2.h>

#include "common.h"

/* redefine mode name, for PTR_XXX */
#undef MOD_NAME
#define MOD_NAME	"VC_TEST"	/* for print message */

#define LB_DEC_DEV_NAME "dev/video0"

#define MAX_DEC_OUT_BUF 2
#define MAX_DEC_CAP_BUF 8

#define DEC_OUT_PLANES 1
#define DEC_CAP_PLANES 2

#define LB_POLL_DEC_TIMEOUT 25

#define MAX_DEC_INPUT_BUF_SIZE  (1024 * 3072)

enum VB_BUF_STATUS {
	BUF_AVAILABLE = 0,
	BUF_USED_BY_DRIVER,
	BUF_USED_BY_APP,
};

struct lombo_dec_params {
	int nalu_type;
	int nal_ref_idc;
	int width_in_mb;
	int height_in_mb;
	int slice_type;
	int pic_order_cnt_type;
	int max_frame_num_minus4;
	int order_cnt_lsb_minus4;
	int entropy_mode;
	int cabac_init_idc;
	int constrained_intra_flag;
	int transform_8x8_mode_flag;
	int chroma_qp_offset;
	int slice_qpy;
	int pos_mb_y;
	int is_first_slice;
	int disable_dbfilter_idc;
	int alpha_c0_offset_div2;
	int slice_beta_offset_div2;
};

struct lombo_decoder {
	struct file *filp;
	mm_segment_t fs;
	int b_stream_finished;
	int decode_status; /* decode status */
	struct lombo_dec_params dec_params;

	/* (souce)Output queue related */
	int out_buf_cnt;
	int out_buf_size;
	int cur_outbuf_index;
	int out_buf_off[MAX_DEC_OUT_BUF];
	char *out_buf_addr[MAX_DEC_OUT_BUF];/* output queue buf addr */
	/* buffer satus£ºenqueued or dequeued */
	int out_buf_flag[MAX_DEC_OUT_BUF];

	/* (dst)Capture queue related */
	int cap_w;
	int cap_h;
	int cap_crop_w;
	int cap_crop_h;
	int cap_crop_left;
	int cap_crop_top;
	int cap_buf_cnt;
	int cur_capbuf_index;
	int cap_buf_size[DEC_CAP_PLANES];
	int cap_buf_off[MAX_DEC_CAP_BUF][DEC_CAP_PLANES];
	/* captture queue buf addr */
	char *cap_buf_addr[MAX_DEC_CAP_BUF][DEC_CAP_PLANES];
	/* buffer status£ºenqueued or dequeued */
	int cap_buf_flag[MAX_DEC_CAP_BUF];
	int cap_buf_queued;
};

static struct lombo_decoder decoder;

int decoder_open_test(void)
{
	decoder.filp = filp_open(LB_DEC_DEV_NAME, O_RDWR|O_NONBLOCK, 0);
	if (IS_ERR(decoder.filp)) {
		PRT_ERR("open decoder error\n");
		return -1;
	}
	decoder.fs = get_fs();
	set_fs(KERNEL_DS);

	return 0;
}

void decoder_close_test(void)
{
	set_fs(decoder.fs);
	filp_close(decoder.filp, NULL);
}

int query_cap_test(void)
{
	int ret;
	struct v4l2_capability cap;

	ret = 0;
	/* query the capabilities to see if we support all we need */
	memset(&cap, 0, sizeof(cap));
	ret = decoder.filp->f_op->unlocked_ioctl(decoder.filp,
		VIDIOC_QUERYCAP, (unsigned long)&cap);
	if (ret < 0) {
		PRT_ERR("decoder querry cap failed!\n");
		return ret;
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)) {
		PRT_ERR("decoder doesn't support capture\n");
		ret = -1;
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT_MPLANE)) {
		PRT_ERR("decoder doesn't support output\n");
		ret = -1;
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		PRT_ERR(" decoder doesn't support capture streaming\n");
		ret = -1;
	}

	return ret;
}

int output_fmt_test(void)
{
	int ret;
	struct v4l2_format fmt;

	ret = 0;
	memset(&fmt, 0, sizeof(fmt));
	/* Default is set to H264 */
	fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_H264;
	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	fmt.fmt.pix_mp.plane_fmt[0].sizeimage = MAX_DEC_INPUT_BUF_SIZE;
	ret = decoder.filp->f_op->unlocked_ioctl(decoder.filp, VIDIOC_S_FMT,
		(unsigned long)&fmt);

	if (ret != 0) {
		PRT_ERR("decoder set fmt error!\n");
		ret = -1;
	}
	decoder.out_buf_size = MAX_DEC_INPUT_BUF_SIZE;

	return ret;
}

int request_mmap_output_buf_test(void)
{
	int ret;
	int i;
	struct v4l2_requestbuffers reqbuf;
	struct v4l2_buffer buf;
	struct v4l2_plane planes[DEC_OUT_PLANES];

	ret = 0;
	/* request buffer */
	memset(&(reqbuf), 0, sizeof(reqbuf));
	reqbuf.count = MAX_DEC_OUT_BUF;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	ret = decoder.filp->f_op->unlocked_ioctl(decoder.filp, VIDIOC_REQBUFS,
		(unsigned long)&reqbuf);
	if (ret != 0) {
		PRT_ERR(" decoder require buffer error !!\n");
		return ret;
	}
	decoder.out_buf_cnt = reqbuf.count;

	/* for all the buffer requested above, query the buffer info */
	for (i = 0; i < decoder.out_buf_cnt; i++) {
		memset(&(buf), 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		buf.m.planes = planes;
		buf.length = 1;

		ret = decoder.filp->f_op->unlocked_ioctl(decoder.filp,
		VIDIOC_QUERYBUF, (unsigned long)&buf);
		if (ret != 0) {
			PRT_ERR("query buf failed !!\n");
			return ret;
		}
		PRT_INFO("plane0 lenght %d, offset %d\n",
			buf.m.planes[0].length,
			buf.m.planes[0].m.mem_offset);

		decoder.out_buf_flag[i] = BUF_AVAILABLE;
	}

	return 0;
}

int queue_outbuf_test(int index, int lenght)
{
	struct v4l2_buffer qbuf;
	struct v4l2_plane planes[DEC_OUT_PLANES];
	int ret = 0;

	memset(&qbuf, 0, sizeof(qbuf));
	qbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	qbuf.memory = V4L2_MEMORY_MMAP;
	qbuf.index = index;
	qbuf.m.planes = planes;
	qbuf.length = 1;
	qbuf.m.planes[0].bytesused = lenght;

	/* call ioctl to queue the out buffer */
	ret = decoder.filp->f_op->unlocked_ioctl(decoder.filp, VIDIOC_QBUF,
		(unsigned long)&qbuf);

	if (ret != 0) {
		PRT_ERR("queue out buf %d error\n", index);
		return -1;
	}

	decoder.out_buf_flag[index] = BUF_USED_BY_DRIVER;

	return 0;
}

int dequeue_outbuf_test(int *index)
{
	struct v4l2_buffer qbuf;
	struct v4l2_plane planes[DEC_OUT_PLANES];
	int ret = 0;

	memset(&qbuf, 0, sizeof(qbuf));
	qbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
	qbuf.memory = V4L2_MEMORY_MMAP;
	qbuf.m.planes = planes;
	qbuf.length = 1;

	ret = decoder.filp->f_op->unlocked_ioctl(decoder.filp, VIDIOC_DQBUF,
		(unsigned long)&qbuf);

	if (ret != 0) {
		PRT_ERR("dequeue out buffer error !\n");
		return -1;
	}

	decoder.out_buf_flag[qbuf.index] = BUF_AVAILABLE;
	/* return the index off the buffer dequeued */
	*index = qbuf.index;
	return 0;
}

void vc_test(void)
{
	PRT_INFO("--start--\n");

	if (decoder_open_test())
		return;
	if (query_cap_test())
		return;
	if (output_fmt_test())
		return;
	if (request_mmap_output_buf_test())
		return;
	if (queue_outbuf_test(1, 1024))
		return;

	decoder_close_test();

	PRT_INFO("--end--\n");
}
