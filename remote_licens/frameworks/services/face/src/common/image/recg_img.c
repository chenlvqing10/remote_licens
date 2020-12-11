/*
 * recg_img.c - Public interface for personnel management
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
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

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define LOG_TAG "RECG_IMG"
#include "log/log.h"

#include "jpeg_dec.h"
#include "bmp_dec.h"
#include "png_dec.h"
#include "recg_img.h"

#define FILE_HEADER_LEN (20)

static void *jpg_hdl;

static int recg_img_dec_jpg(struct recg_img_intput *input,
		struct recg_img_output *output)
{
	int ret = -1;
	struct jpeg_dec_input in = {0};

	if (input == NULL || output == NULL) {
		ALOGE("param error");
		return -1;
	}

	memset(&in, 0 ,sizeof(in));
	in.file = input->file;
	in.format = JPEG_COLOR_NV12;
	in.scale = JPEG_SCALE_AUTO;
	in.width_stride = input->stride_width;
	in.height_stride = input->stride_height;
	if (jpg_hdl == NULL) {
		jpg_hdl = jpeg_fast_dec_init();
		if (jpg_hdl == NULL) {
			ALOGE("jpeg fast dec init failed");
			return -1;
		}
	}
	ret = jpeg_fast_dec_begin(&in, &output->out_jpeg, jpg_hdl);
	if (ret) {
		ALOGE("jpeg dec failed");
		return -1;
	}

	output->type = type_jpeg;
	output->stride_width = output->out_jpeg.stride_width;
	output->stride_height = output->out_jpeg.stride_height;
	output->virt_addr[0] = output->out_jpeg.addr[0];
	output->virt_addr[1] = output->out_jpeg.addr[1];
	output->virt_addr[2] = output->out_jpeg.addr[2];
	output->phy_addr[0] = output->out_jpeg.phy_addr[0];
	output->phy_addr[1] = output->out_jpeg.phy_addr[1];
	output->phy_addr[2] = output->out_jpeg.phy_addr[2];
	output->size[0] = output->out_jpeg.size[0];
	output->size[1] = output->out_jpeg.size[1];
	output->size[2] = output->out_jpeg.size[2];

	return ret;
}

static int recg_img_dec_jpg_data(struct recg_img_intput *input,
		struct recg_img_output *output)
{
	int ret = -1;

	if (input == NULL || output == NULL) {
		ALOGE("param error");
		return -1;
	}

	if (jpg_hdl == NULL) {
		jpg_hdl = jpeg_fast_dec_init();
		if (jpg_hdl == NULL) {
			ALOGE("jpeg fast dec init failed");
			return -1;
		}
	}
	ret = jpeg_fast_dec_data(input->data, input->len, input->stride_width,
		input->stride_height, JPEG_SCALE_AUTO, &output->out_jpeg, jpg_hdl);
	if (ret) {
		ALOGE("jpeg dec failed");
		return -1;
	}

	output->type = type_jpeg;
	output->stride_width = output->out_jpeg.stride_width;
	output->stride_height = output->out_jpeg.stride_height;
	output->virt_addr[0] = output->out_jpeg.addr[0];
	output->virt_addr[1] = output->out_jpeg.addr[1];
	output->virt_addr[2] = output->out_jpeg.addr[2];
	output->phy_addr[0] = output->out_jpeg.phy_addr[0];
	output->phy_addr[1] = output->out_jpeg.phy_addr[1];
	output->phy_addr[2] = output->out_jpeg.phy_addr[2];
	output->size[0] = output->out_jpeg.size[0];
	output->size[1] = output->out_jpeg.size[1];
	output->size[2] = output->out_jpeg.size[2];

	return ret;
}

static int recg_img_dec_jpg_info(struct recg_img_intput *input,
		struct recg_img_dec_info *info)
{
	int ret = -1;

	if (input == NULL || info == NULL) {
		ALOGE("param error");
		return -1;
	}

	if (jpg_hdl == NULL) {
		jpg_hdl = jpeg_fast_dec_init();
		if (jpg_hdl == NULL) {
			ALOGE("jpeg fast dec init failed");
			return -1;
		}
	}

	ret = jpeg_fast_read_file_header(jpg_hdl, input->file,
		&info->out_jpg.width, &info->out_jpg.height);
	if (ret) {
		ALOGE("jpeg dec info failed");
		return -1;
	}
	info->width = info->out_jpg.width;
	info->height = info->out_jpg.height;
	ALOGI("jpg width: %d, height: %d\n", info->width, info->height);

	return ret;
}

static int recg_img_dec_jpg_data_info(struct recg_img_intput *input,
		struct recg_img_dec_info *info)
{
	int ret = -1;

	if (input == NULL || info == NULL) {
		ALOGE("param error");
		return -1;
	}

	if (jpg_hdl == NULL) {
		jpg_hdl = jpeg_fast_dec_init();
		if (jpg_hdl == NULL) {
			ALOGE("jpeg fast dec init failed");
			return -1;
		}
	}
	ret = jpeg_fast_read_header(jpg_hdl, input->data, input->len,
		&info->out_jpg.width, &info->out_jpg.height);
	if (ret) {
		ALOGE("jpeg dec info failed");
		return -1;
	}
	info->width = info->out_jpg.width;
	info->height = info->out_jpg.height;
	ALOGI("jpg width: %d, height: %d\n", info->width, info->height);

	return ret;
}

static int recg_img_dec_bmp(struct recg_img_intput *input,
		struct recg_img_output *output)
{
	int ret = -1;
	struct bmp_dec_input in;

	memset(&in, 0 ,sizeof(in));
	in.file = input->file;
	in.format = BMP_COLOR_NV12;
	in.scale = BMP_SCALE_AUTO;
	in.stride_width = input->stride_width;
	in.stride_height = input->stride_height;
	ret = bmp_dec_begin(&in, &output->out_bmp);
	if (ret) {
		return -1;
	}

	output->type = type_bmp;
	output->stride_width = output->out_bmp.stride_width;
	output->stride_height = output->out_bmp.stride_height;
	output->virt_addr[0] = output->out_bmp.virt_addr[0];
	output->virt_addr[1] = output->out_bmp.virt_addr[1];
	output->virt_addr[2] = output->out_bmp.virt_addr[2];
	output->phy_addr[0] = output->out_bmp.phy_addr[0];
	output->phy_addr[1] = output->out_bmp.phy_addr[1];
	output->phy_addr[2] = output->out_bmp.phy_addr[2];
	output->size[0] = output->out_bmp.size[0];
	output->size[1] = output->out_bmp.size[1];
	output->size[2] = output->out_bmp.size[2];

	return ret;
}

static int recg_img_dec_bmp_data(struct recg_img_intput *input,
		struct recg_img_output *output)
{
	int ret = -1;
	struct bmp_dec_input in;

	memset(&in, 0 ,sizeof(in));
	in.data = input->data;
	in.len = input->len;
	in.format = BMP_COLOR_NV12;
	in.scale = BMP_SCALE_AUTO;
	in.stride_width = input->stride_width;
	in.stride_height = input->stride_height;
	ret = bmp_dec_data(&in, &output->out_bmp);
	if (ret) {
		return -1;
	}

	output->type = type_bmp;
	output->stride_width = output->out_bmp.stride_width;
	output->stride_height = output->out_bmp.stride_height;
	output->virt_addr[0] = output->out_bmp.virt_addr[0];
	output->virt_addr[1] = output->out_bmp.virt_addr[1];
	output->virt_addr[2] = output->out_bmp.virt_addr[2];
	output->phy_addr[0] = output->out_bmp.phy_addr[0];
	output->phy_addr[1] = output->out_bmp.phy_addr[1];
	output->phy_addr[2] = output->out_bmp.phy_addr[2];
	output->size[0] = output->out_bmp.size[0];
	output->size[1] = output->out_bmp.size[1];
	output->size[2] = output->out_bmp.size[2];

	return ret;
}

static int recg_img_dec_bmp_info(struct recg_img_intput *input,
		struct recg_img_dec_info *info)
{
	int ret = -1;
	struct bmp_dec_input in;

	memset(&in, 0 ,sizeof(in));
	in.file = input->file;
	in.format = BMP_COLOR_NV12;
	in.scale = BMP_SCALE_AUTO;
	in.stride_width = input->stride_width;
	in.stride_height = input->stride_height;

	ret = bmp_dec_info_read(&in, &info->out_bmp);
	if (ret) {
		return -1;
	}
	info->width = info->out_bmp.width;
	info->height = info->out_bmp.height;

	return ret;
}

static int recg_img_dec_bmp_data_info(struct recg_img_intput *input,
		struct recg_img_dec_info *info)
{
	int ret = -1;
	struct bmp_dec_input in;

	memset(&in, 0 ,sizeof(in));
	in.data = input->data;
	in.len = input->len;
	in.format = BMP_COLOR_NV12;
	in.scale = BMP_SCALE_AUTO;
	in.stride_width = input->stride_width;
	in.stride_height = input->stride_height;
	ret = bmp_dec_info_read_data(&in, &info->out_bmp);
	if (ret) {
		return -1;
	}
	info->width = info->out_bmp.width;
	info->height = info->out_bmp.height;

	return ret;
}

static int recg_img_dec_png(struct recg_img_intput *input,
		struct recg_img_output *output)
{
	int ret = -1;
	struct png_dec_input in;

	memset(&in, 0 ,sizeof(in));
	in.file = input->file;
	in.format = PNG_COLOR_NV12;
	in.scale = PNG_SCALE_AUTO;
	in.stride_width = input->stride_width;
	in.stride_height = input->stride_height;
	ret = png_dec_begin(&in, &output->out_png);
	if (ret) {
		return -1;
	}

	output->type = type_png;
	output->stride_width = output->out_png.stride_width;
	output->stride_height = output->out_png.stride_height;
	output->virt_addr[0] = output->out_png.virt_addr[0];
	output->virt_addr[1] = output->out_png.virt_addr[1];
	output->virt_addr[2] = output->out_png.virt_addr[2];
	output->phy_addr[0] = output->out_png.phy_addr[0];
	output->phy_addr[1] = output->out_png.phy_addr[1];
	output->phy_addr[2] = output->out_png.phy_addr[2];
	output->size[0] = output->out_png.size[0];
	output->size[1] = output->out_png.size[1];
	output->size[2] = output->out_png.size[2];

	return ret;
}

static int recg_img_dec_png_data(struct recg_img_intput *input,
		struct recg_img_output *output)
{
	int ret = -1;
	struct png_dec_input in;

	memset(&in, 0 ,sizeof(in));
	in.data = input->data;
	in.len = input->len;
	in.format = PNG_COLOR_NV12;
	in.scale = PNG_SCALE_AUTO;
	in.stride_width = input->stride_width;
	in.stride_height = input->stride_height;
	ret = png_dec_data(&in, &output->out_png);
	if (ret) {
		return -1;
	}

	output->type = type_png;
	output->stride_width = output->out_png.stride_width;
	output->stride_height = output->out_png.stride_height;
	output->virt_addr[0] = output->out_png.virt_addr[0];
	output->virt_addr[1] = output->out_png.virt_addr[1];
	output->virt_addr[2] = output->out_png.virt_addr[2];
	output->phy_addr[0] = output->out_png.phy_addr[0];
	output->phy_addr[1] = output->out_png.phy_addr[1];
	output->phy_addr[2] = output->out_png.phy_addr[2];
	output->size[0] = output->out_png.size[0];
	output->size[1] = output->out_png.size[1];
	output->size[2] = output->out_png.size[2];

	return ret;
}

static int recg_img_dec_png_info(struct recg_img_intput *input,
		struct recg_img_dec_info *info)
{
	int ret = -1;
	struct png_dec_input in;

	memset(&in, 0 ,sizeof(in));
	in.file = input->file;
	in.format = PNG_COLOR_NV12;
	in.scale = PNG_SCALE_AUTO;
	in.stride_width = input->stride_width;
	in.stride_height = input->stride_height;
	ret = png_dec_info_read(&in, &info->out_png);
	if (ret) {
		return -1;
	}
	info->width = info->out_png.width;
	info->height = info->out_png.height;

	return ret;
}

static int recg_img_dec_png_data_info(struct recg_img_intput *input,
		struct recg_img_dec_info *info)
{
	int ret = -1;
	struct png_dec_input in;

	memset(&in, 0 ,sizeof(in));
	in.data = input->data;
	in.len = input->len;
	in.format = PNG_COLOR_NV12;
	in.scale = PNG_SCALE_AUTO;
	in.stride_width = input->stride_width;
	in.stride_height = input->stride_height;
	ret = png_dec_info_read_data(&in, &info->out_png);
	if (ret) {
		return -1;
	}
	info->width = info->out_png.width;
	info->height = info->out_png.height;

	return ret;
}

static e_recg_img_type recg_img_get_type(char *file)
{
	e_recg_img_type type = type_unknow;
	unsigned char *raw = NULL;
	FILE *fp = NULL;
	int count = 0;

	raw = (unsigned char *)malloc(FILE_HEADER_LEN);
	if (raw == NULL) {
		ALOGE("alloc mem for file failed");
		goto exit;
	}
	memset(raw, 0, FILE_HEADER_LEN);

	fp = fopen((const char *)file, "rb");
	if (fp == NULL) {
		ALOGE("open %s failed", file);
		goto exit;
	}

	count = fread(raw, 1, FILE_HEADER_LEN, fp);
	if (count != FILE_HEADER_LEN) {
		ALOGE("read file failed, count:%d, file_len:%d",
			count, FILE_HEADER_LEN);
		goto exit;
	}

	/* check file header */
	if(raw[0] == 0xff && raw[1] == 0xd8)
		type = type_jpeg;
	else if (raw[0] == 0x42 && raw[1] == 0x4D)
		type = type_bmp;
	else if (raw[0] == 0x89 && raw[1] == 0x50 &&
				raw[2] == 0x4e && raw[3] == 0x47 &&
				raw[4] == 0x0d && raw[5] == 0x0a &&
				raw[6] == 0x1a && raw[7] == 0x0a)
		type = type_png;
	else
		type = type_unknow;
exit:

	if (raw) {
		free(raw);
		raw = NULL;
	}
	if (fp) {
		fclose(fp);
		fp = NULL;
	}

	return type;
}

static e_recg_img_type recg_img_data_get_type(char *data)
{
	e_recg_img_type type = type_unknow;
	unsigned char *raw = NULL;

	raw = (unsigned char *)malloc(FILE_HEADER_LEN);
	if (raw == NULL) {
		ALOGE("alloc mem for file failed");
		goto exit;
	}
	memset(raw, 0, FILE_HEADER_LEN);
	memcpy(raw, data, FILE_HEADER_LEN - 1);

	/* check file header */
	if(raw[0] == 0xff && raw[1] == 0xd8)
		type = type_jpeg;
	else if (raw[0] == 0x42 && raw[1] == 0x4D)
		type = type_bmp;
	else if (raw[0] == 0x89 && raw[1] == 0x50 &&
				raw[2] == 0x4e && raw[3] == 0x47 &&
				raw[4] == 0x0d && raw[5] == 0x0a &&
				raw[6] == 0x1a && raw[7] == 0x0a)
		type = type_png;
	else
		type = type_unknow;
exit:

	if (raw) {
		free(raw);
		raw = NULL;
	}
	return type;
}


LOCAL int recg_img_dec_begin(struct recg_img_intput *input,
		struct recg_img_output *output)
{
	int ret = -1;
	int img_type;

	if (input == NULL || output == NULL) {
		ALOGE("param error");
		return -1;
	}

	img_type = recg_img_get_type(input->file);

	switch (img_type) {
	case type_jpeg:
		ret = recg_img_dec_jpg(input, output);
		break;
	case type_bmp:
		ret = recg_img_dec_bmp(input, output);
		break;
	case type_png:
		ret = recg_img_dec_png(input, output);
		break;
	case type_unknow:
		ALOGE("unknow img type");
		break;
	default:
		break;
	}

	return ret;
}

LOCAL int recg_img_dec_data(struct recg_img_intput *input,
		struct recg_img_output *output)
{
	int ret = -1;
	int img_type;

	if (input == NULL || output == NULL) {
		ALOGE("param error");
		return -1;
	}

	img_type = recg_img_data_get_type(input->data);

	switch (img_type) {
	case type_jpeg:
		ret = recg_img_dec_jpg_data(input, output);
		break;
	case type_bmp:
		ret = recg_img_dec_bmp_data(input, output);
		break;
	case type_png:
		ret = recg_img_dec_png_data(input, output);
		break;
	case type_unknow:
		ALOGE("unknow img type");
		break;
	default:
		break;
	}

	return ret;
}

LOCAL void recg_img_output_adjust_end(struct recg_img_output *output)
{
	if(output == NULL)
		return;

	switch (output->type) {
	case type_jpeg:
		output->out_jpeg.addr[0] = output->virt_addr[0];
		break;
	case type_bmp:
		output->out_bmp.virt_addr[0] = output->virt_addr[0];
		break;
	case type_png:
		output->out_png.virt_addr[0] = output->virt_addr[0];
		break;
	case type_unknow:
		break;
	default:
		break;
	}
}

LOCAL void recg_img_dec_end(struct recg_img_output *output)
{
	if(output == NULL)
		return;

	switch (output->type) {
	case type_jpeg:
		jpeg_fast_dec_end(&output->out_jpeg);
		break;
	case type_bmp:
		bmp_dec_end(&output->out_bmp);
		break;
	case type_png:
		png_dec_end(&output->out_png);
		break;
	case type_unknow:
		break;
	default:
		break;
	}
}

LOCAL int recg_img_dec_info(struct recg_img_intput *input,
		struct recg_img_dec_info *info)
{
	int ret = -1;
	int img_type;

	img_type = recg_img_get_type(input->file);

	switch (img_type) {
	case type_jpeg:
		ret = recg_img_dec_jpg_info(input, info);
		break;
	case type_bmp:
		ret = recg_img_dec_bmp_info(input, info);
		break;
	case type_png:
		ret = recg_img_dec_png_info(input, info);
		break;
	case type_unknow:
		ALOGE("unknow img type");
		break;
	default:
		break;
	}

	return ret;
}

LOCAL int recg_img_data_dec_info(struct recg_img_intput *input,
		struct recg_img_dec_info *info)
{
	int ret = -1;
	int img_type;

	img_type = recg_img_data_get_type(input->data);

	switch (img_type) {
	case type_jpeg:
		ret = recg_img_dec_jpg_data_info(input, info);
		break;
	case type_bmp:
		ret = recg_img_dec_bmp_data_info(input, info);
		break;
	case type_png:
		ret = recg_img_dec_png_data_info(input, info);
		break;
	case type_unknow:
		ALOGE("unknow img type");
		break;
	default:
		break;
	}

	return ret;
}

