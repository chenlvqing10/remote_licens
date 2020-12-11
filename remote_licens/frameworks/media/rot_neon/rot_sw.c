/*
 * rotation_sw.c - Rotation sw module driver code for LomboTech
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

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include <log/log.h>
#include "rotation_sw.h"
#include "rot_sw.h"

/*
 * Framebuffer information
 */
typedef struct tag_fb_info {
	unsigned int format;		/* Pixel format */
	unsigned int planes;		/* Planes of buffer */
	unsigned char bpp[3];		/* Byte per pixel */
	unsigned int addr[3];		/* Buffer addr */
	unsigned int width[3];		/* Image width */
	unsigned int height[3];		/* Image height */
	unsigned int linestride_bit[3];	/* Line stride of buffer */
} fb_info_t;

typedef struct tag_rot_cfg {
	fb_info_t infb;
	fb_info_t outfb;
} rot_cfg_t;

static bool is_rot_format(unsigned int format, unsigned char *plane_num)
{
	switch (format) {
	case DISP_FORMAT_ARGB8888:
	case DISP_FORMAT_ABGR8888:
	case DISP_FORMAT_RGBA8888:
	case DISP_FORMAT_BGRA8888:

	case DISP_FORMAT_ARGB4444:
	case DISP_FORMAT_ABGR4444:
	case DISP_FORMAT_RGBA4444:
	case DISP_FORMAT_BGRA4444:
		*plane_num = 1;
		break;

	case DISP_FORMAT_NV12:
	case DISP_FORMAT_NV21:
	case DISP_FORMAT_NV16:
	case DISP_FORMAT_NV61:
		*plane_num = 2;
		break;

	case DISP_FORMAT_YUV420:
	case DISP_FORMAT_YVU420:
	case DISP_FORMAT_YUV422:
	case DISP_FORMAT_YVU422:
		*plane_num = 3;
		break;
	default:
		/* LOG_D("is not rot format"); */
		return false;
	}

	/* LOG_D("is rot format"); */
	return true;
}

bool is_422_format(unsigned int format)
{
	switch (format) {
	case DISP_FORMAT_YUV422:
	case DISP_FORMAT_YVU422:
	case DISP_FORMAT_NV16:
	case DISP_FORMAT_NV61:
		return true;
		break;

	default:
		/* LOG_D("is not 422 format"); */
		return false;
	}

	/* LOG_D("is 422 format"); */
	return false;
}

/* while input is 422 and is in vertival flip mode output must be 420 */
static bool rot_match_422_format(unsigned int format_in, unsigned int format_out)
{
	if ((DISP_FORMAT_YUV422 == format_in) && (DISP_FORMAT_YUV420 == format_out))
		return true;
	else if ((DISP_FORMAT_YVU422 == format_in) && (DISP_FORMAT_YVU420 == format_out))
		return true;
	else if ((DISP_FORMAT_NV16 == format_in) && (DISP_FORMAT_NV12 == format_out))
		return true;
	else if ((DISP_FORMAT_NV61 == format_in) && (DISP_FORMAT_NV21 == format_out))
		return true;

	return false;
}

/* get bytes per pixel from format */
/* plane_cnt: 0,1,2 */
unsigned char get_bpp_from_format(unsigned int format, unsigned char plane_cnt)
{
	unsigned char bpp = 0;

	switch (format) {
	case DISP_FORMAT_ARGB8888:
	case DISP_FORMAT_ABGR8888:
	case DISP_FORMAT_RGBA8888:
	case DISP_FORMAT_BGRA8888:
		if (0 == plane_cnt)
			bpp = 4;
		break;
	case DISP_FORMAT_ARGB4444:
	case DISP_FORMAT_ABGR4444:
	case DISP_FORMAT_RGBA4444:
	case DISP_FORMAT_BGRA4444:
		if (0 == plane_cnt)
			bpp = 2;
		break;

	case DISP_FORMAT_NV12:
	case DISP_FORMAT_NV21:
	case DISP_FORMAT_NV16:
	case DISP_FORMAT_NV61:
		if (0 == plane_cnt)
			bpp = 1;
		else if (1 == plane_cnt)
			bpp = 2;
		break;

	case DISP_FORMAT_YUV420:
	case DISP_FORMAT_YVU420:
	case DISP_FORMAT_YUV422:
	case DISP_FORMAT_YVU422:
		bpp = 1;
		break;
	default:
		/* LOG_D("is not rot format"); */
		return false;
	}

	return bpp;
}

static int rot_check_cfg(disp_rot_cfg_t *cfgs, rot_cfg_t *rot_cfgs,
				unsigned char *plane_cnt, bool sw_flag)
{
	int ret = 0;
	/* while input is 422 and is in vertival flip mode output must be 420 */
	bool is422_ver_flip_mode = false;
	unsigned char i, plane_nums_i = 0, plane_nums_o = 0;
	char *fmt;
	bool is_size_err = false;
	unsigned int w_align_in, w_align_out;

	if (cfgs->mode >= LOMBO_TRANSFORM_MAX) {
		ret = -1;
		ALOGE("mode[%d] not legal mode", cfgs->mode);
		goto exit;
	}

	if (!is_rot_format(cfgs->infb.format, &plane_nums_i)) {
		ret = -1;
		fmt = (char *)&cfgs->infb.format;
		ALOGE("infb format[%c%c%c%c] is not rot format",
			fmt[0], fmt[1], fmt[2], fmt[3]);
		goto exit;
	}
	if (!is_rot_format(cfgs->outfb.format, &plane_nums_o)) {
		ret = -1;
		fmt = (char *)&cfgs->infb.format;
		ALOGE("outfb format[%c%c%c%c] is not rot format",
			fmt[0], fmt[1], fmt[2], fmt[3]);
		goto exit;
	}

	/* sw rot do no support yuv422 format */
	if (sw_flag && is_422_format(cfgs->infb.format)) {
		ret = -1;
		ALOGE("sw rot do no support yuv422 format");
		goto exit;
	}

	if ((cfgs->mode % 2 != 0) && is_422_format(cfgs->infb.format)) {
		is422_ver_flip_mode = true;
		if (!rot_match_422_format(cfgs->infb.format, cfgs->outfb.format)) {
			ret = -1;
			ALOGE("when input 422 output must be 420 in vertival flip mode");
			goto exit;
		}
	}

	/* this just make sure infb format = outfb format*/
	rot_cfgs->infb.format = cfgs->outfb.format;
	rot_cfgs->outfb.format = cfgs->outfb.format;

	if (plane_nums_i != plane_nums_o) {
		ret = -1;
		ALOGE("plane_nums err, check if infb format match with oufb format");
		goto exit;
	}
	assert(plane_nums_i <= 3);

	*plane_cnt = plane_nums_i;

	ALOGD("plane_nums_i[%d] plane_nums_o[%d]\n", plane_nums_i, plane_nums_o);

	rot_cfgs->infb.planes = plane_nums_i;
	rot_cfgs->outfb.planes = plane_nums_o;

	for (i = 0; i < plane_nums_i; i++) {
		if ((cfgs->infb.width[i] == 0) || (cfgs->infb.height[i] == 0) ||
			(cfgs->outfb.width[i] == 0) || (cfgs->outfb.height[i] == 0)) {
			ret = -1;
			ALOGE("width or height err");
			is_size_err = true;
			goto exit;
		}

		if (cfgs->mode % 2 == 0) {
			if ((cfgs->infb.width[i] != cfgs->outfb.width[i]) ||
				(cfgs->infb.height[i] != cfgs->outfb.height[i])) {
				ret = -1;
				ALOGE("width or height err");
				is_size_err = true;
				goto exit;
			}
		} else {
			if ((cfgs->infb.width[i] != cfgs->outfb.height[i]) ||
				(cfgs->infb.height[i] != cfgs->outfb.width[i])) {
				ret = -1;
				ALOGE("width or height err");
				is_size_err = true;
				goto exit;
			}
		}

		if (!sw_flag && ((cfgs->infb.width[i] < 32) ||
			(cfgs->infb.height[i] < 32) ||
			(cfgs->infb.width[i] > 8192) ||
			(cfgs->infb.height[i] > 8192))) {
			ret = -1;
			ALOGE("only support min size [16 x 16] max size[8192 x 8192]");
			is_size_err = true;
			goto exit;
		}

		rot_cfgs->infb.bpp[i] = get_bpp_from_format(cfgs->infb.format, i);
		rot_cfgs->outfb.bpp[i] = get_bpp_from_format(cfgs->outfb.format, i);

		ALOGD("plane[%d] infb bpp[%d] outfb bpp[%d]\n", i,
			rot_cfgs->infb.bpp[i], rot_cfgs->outfb.bpp[i]);

		if ((rot_cfgs->infb.bpp[i] == 0) || (rot_cfgs->outfb.bpp[i] == 0)) {
			ret = -1;
			ALOGE("get bpp err");
			goto exit;
		}

		w_align_in = 16 / rot_cfgs->infb.bpp[i];
		w_align_out = 16 / rot_cfgs->outfb.bpp[i];
		if (!sw_flag && ((cfgs->infb.width[i] % w_align_in != 0) ||
			(cfgs->outfb.width[i] % w_align_out != 0))) {
			ret = -1;
			ALOGE("width align in[%d] out[%d] bytes err",
				w_align_in, w_align_out);
			is_size_err = true;
			goto exit;
		}

		if (!sw_flag && ((cfgs->infb.height[i] % 16 != 0) ||
			(cfgs->outfb.height[i] % 16 != 0))) {
			ret = -1;
			ALOGE("height align 16 bytes err");
			is_size_err = true;
			goto exit;
		}

		rot_cfgs->infb.width[i] = cfgs->infb.width[i];
		rot_cfgs->infb.height[i] = cfgs->infb.height[i];

		ALOGD("plane[%d] infb width[%d] height[%d]\n", i,
			rot_cfgs->infb.width[i], rot_cfgs->infb.height[i]);

		rot_cfgs->outfb.width[i] = cfgs->outfb.width[i];
		rot_cfgs->outfb.height[i] = cfgs->outfb.height[i];

		ALOGD("plane[%d] outfb width[%d] height[%d]\n", i,
			rot_cfgs->outfb.width[i], rot_cfgs->outfb.height[i]);

		assert(cfgs->infb.addr[i] != 0);
		assert(cfgs->outfb.addr[i] != 0);

		if (!sw_flag && ((cfgs->infb.addr[i] % 8 != 0) ||
			(cfgs->outfb.addr[i] % 8 != 0))) {
			ret = -1;
			ALOGE("addr align 32 bytes err in[0x%x] out[0x%x]\n",
				cfgs->infb.addr[i], cfgs->outfb.addr[i]);
			goto exit;
		}

		rot_cfgs->infb.addr[i] = cfgs->infb.addr[i];
		rot_cfgs->outfb.addr[i] = cfgs->outfb.addr[i];
		ALOGD("plane[%d] infb addr[%x] outfb addr[%x]\n", i,
			cfgs->infb.addr[i], cfgs->outfb.addr[i]);

		if (cfgs->infb.linestride_bit[i] == 0)
			rot_cfgs->infb.linestride_bit[i] =
				DISP_F * rot_cfgs->infb.width[i] * rot_cfgs->infb.bpp[i];
		else
			rot_cfgs->infb.linestride_bit[i] = cfgs->infb.linestride_bit[i];
		if (cfgs->outfb.linestride_bit[i] == 0)
			rot_cfgs->outfb.linestride_bit[i] =
				DISP_F * rot_cfgs->outfb.width[i] * rot_cfgs->infb.bpp[i];
		else
			rot_cfgs->outfb.linestride_bit[i] = cfgs->outfb.linestride_bit[i];

		ALOGD("inls[%d] outls[%d]\n", rot_cfgs->infb.linestride_bit[i],
				rot_cfgs->outfb.linestride_bit[i]);

		if (!sw_flag && ((rot_cfgs->infb.linestride_bit[i] % 8 != 0) ||
			(rot_cfgs->outfb.linestride_bit[i] % 8 != 0))) {
			ret = -1;
			ALOGE("linestride_bit align 16 bytes err inls[%d] outls[%d]\n",
				rot_cfgs->infb.linestride_bit[i],
				rot_cfgs->outfb.linestride_bit[i]);
			goto exit;
		}

		/* rot is only suppor 420 input,  */
		/* when input is 422 we should do some change */
		if (is422_ver_flip_mode) {
			rot_cfgs->infb.height[i] = rot_cfgs->infb.height[i] / 2;
			rot_cfgs->infb.linestride_bit[i] =
				rot_cfgs->infb.linestride_bit[i] * 2;
		}
	}

exit:
	if (is_size_err) {
		ALOGE("plane[%d] input size [%d %d] output size [%d %d]\n",
			i, cfgs->infb.width[i], cfgs->infb.height[i],
			cfgs->outfb.width[i], cfgs->outfb.height[i]);
	}
	return ret;
}

static int rot_sw(rot_cfg_t *rot_cfg, int mode, unsigned char plane_cnt)
{
	fb_info_t *infb = &(rot_cfg->infb);
	fb_info_t *outfb = &(rot_cfg->outfb);

	unsigned char *src = (unsigned char *)infb->addr[plane_cnt];
	unsigned char *dst = (unsigned char *)outfb->addr[plane_cnt];

	sw_transform(src, infb->linestride_bit[plane_cnt],
		dst, outfb->linestride_bit[plane_cnt],
		infb->width[plane_cnt], infb->height[plane_cnt],
		mode, infb->bpp[plane_cnt]);

	return 0;
}

int rot_sw_process(disp_rot_cfg_t *cfgs)
{
	int ret = 0;
	unsigned char i, plane_num = 0;
	rot_cfg_t rot_cfg;

	ret = rot_check_cfg(cfgs, &rot_cfg, &plane_num, true);
	if (ret != 0) {
		ALOGE("rot_check_cfg error %d\n", ret);
		goto exit;
	}

	for (i = 0; i < plane_num; i++)
		rot_sw(&rot_cfg, cfgs->mode, i);

exit:
	return ret;
}

