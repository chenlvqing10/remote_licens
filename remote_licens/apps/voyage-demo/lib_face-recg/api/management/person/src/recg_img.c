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
#include <png.h>

#define LOG_TAG "RECG_IMG"
#include "log/log.h"

#include "jpeg_dec.h"
#include "bmp_dec.h"
#include "jpeg_hal.h"
#include "lombo_jpegdec_plugin.h"

#include "png_dec.h"
#include "recg_img.h"

#define FILE_HEADER_LEN (20)

static void *jpg_hdl;

static int recg_img_dec_jpg(struct recg_img_intput *input,
		struct recg_img_output *output)
{
	int ret = -1;
	struct jpeg_dec_input in;

	if (input == NULL || output == NULL) {
		ALOGE("param error");
		return -1;;
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

int recg_img_get_jpg_header_info(struct recg_img_intput *input,
		struct recg_img_output *output)
{
	jpeg_dec_packet_t packet;
	jpeg_dec_header_info_t dec_header_info;
	FILE *fp = NULL;
	int file_len, count, ret = -1;

	packet.vir_addr = NULL;

	if (input == NULL || output == NULL) {
		ALOGE("param error");
		goto exit;
	}

	if (jpg_hdl == NULL) {
		jpg_hdl = jpeg_fast_dec_init();
		if (jpg_hdl == NULL) {
			ALOGE("jpeg fast dec init failed");
			goto exit;
		}
	}

	fp = fopen((const char *)input->file, "rb");
	if (fp == NULL) {
		ALOGE("open %s failed", input->file);
		goto exit;
	}

	/* get file length */
	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);

	if (file_len <= 0) {
		ALOGE("get file length error");
		goto exit;
	}
	fseek(fp, 0, SEEK_SET);

	packet.stream_len = file_len;
	packet.vir_addr = (unsigned char *)malloc(packet.stream_len);
	if (packet.vir_addr == NULL) {
		ALOGE("alloc mem for file failed");
		goto exit;
	}

	count = fread(packet.vir_addr, 1, packet.stream_len, fp);
	if (count != packet.stream_len) {
		ALOGE("read file failed, count:%d, file_len:%d", count, file_len);
		goto exit;
	}

	/* dec read header infomation */
	ret = hal_jpeg_dec_read_header(jpg_hdl, &packet, &dec_header_info);
	if (ret != 0) {
		ALOGE("dec read header infomation failed\n");
		goto exit;
	}

	output->out_jpeg.actual_width = dec_header_info.crop_width;
	output->out_jpeg.actual_height = dec_header_info.crop_height;

exit:
	if (packet.vir_addr)
		free(packet.vir_addr);
	if (fp)
		fclose(fp);

	return ret;
}

int recg_img_get_bmp_header_info(struct recg_img_intput *input,
		struct recg_img_output *output)
{
	FILE *fp = NULL;
	int len,ret = 0;
	BITMAPFILEHEADER head;
	BITMAPINFOHEADER info;

	memset(&head, 0, sizeof(head));
	memset(&info, 0, sizeof(info));

	fp = fopen(input->file, "rb");
	if (fp == NULL) {
		ALOGE("fopen file fail\n");
		ret = -1;
		goto exit;
	}

	fread(&head, 1, sizeof(BITMAPFILEHEADER), fp);
	len = fread(&info, 1, sizeof(BITMAPINFOHEADER), fp);
	if (len != sizeof(BITMAPINFOHEADER)) {
		ALOGE("read head information fail\n");
		ret = -1;
		goto exit;
	}

	output->out_bmp.width = info.bi_width; /* img width px */
	output->out_bmp.height = info.bi_height; /* img height px */
exit:
	if (fp)
		fclose(fp);

	return ret;
}

int recg_img_get_png_header_info(struct recg_img_intput *input,
		struct recg_img_output *output)
{
	FILE *fp = NULL;
	int ret = 0;
	png_structp png = NULL;
	png_infop info = NULL;

	memset(&info, 0, sizeof(info));

	fp = fopen(input->file, "rb");
	if (fp == NULL) {
		ALOGE("fopen file fail\n");
		ret = -1;
		goto exit;
	}

	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png == NULL) {
		ret = -1;
		goto exit;
	}

	if (setjmp(png_jmpbuf(png))) {
		ret = -1;
		goto exit;
	}

	info = png_create_info_struct(png);
	if (info == NULL) {
		ret = -1;
		goto exit;
	}

	png_init_io(png, fp);
	png_read_png(png, info,
		PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_ALPHA, NULL);

	output->out_png.width = info->width; /* img width px */
	output->out_png.height = info->height; /* img height px */
exit:
	if (png) {
		png_destroy_read_struct(&png, &info, NULL);
	}
	if (fp)
		fclose(fp);

	return ret;
}

int recg_img_size_check(struct recg_img_intput *input,
		struct recg_img_output *output)
{
	int ret = -1;
	int img_type;

	if (input == NULL || output == NULL) {
		ALOGE("param error");
		return -1;;
	}

	img_type = recg_img_get_type(input->file);

	switch (img_type) {
	case type_jpeg:
		ret = recg_img_get_jpg_header_info(input, output);
		if (ret) {
			ALOGE("dec read jpg header infomation failed");
		} else {

			if ((output->out_jpeg.actual_width > input->stride_width * 4 ) ||
			(output->out_jpeg.actual_height > input->stride_height * 4)) {
			printf("image size exceeds the maximum size limit!\n");
			ret =  -1;
			}
		}
		break;
	case type_bmp:
		ret = recg_img_get_bmp_header_info(input, output);
		if (ret) {
			ALOGE("dec read jpg header infomation failed");
		} else {

			if ((output->out_bmp.width > input->stride_width * 4 ) ||
			(output->out_bmp.height > input->stride_height * 4)) {
				printf("image size exceeds the maximum size limit!\n");
				ret = -1;
			}
		}
		break;
	case type_png:
		ret = recg_img_get_png_header_info(input, output);
		if (ret) {
			ALOGE("dec read jpg header infomation failed");
		} else {

			if ((output->out_png.width > input->stride_width * 4 ) ||
			(output->out_png.height > input->stride_height * 4)) {
				printf("image size exceeds the maximum size limit!\n");
				ret = -1;
			}
		}
		break;
	case type_unknow:
		ALOGE("unknow img type");
		break;
	default:
		break;
	}

	return ret;
}

__attribute__((visibility("hidden")))
int recg_img_dec_begin(struct recg_img_intput *input,
		struct recg_img_output *output)
{
	int ret = -1;
	int img_type;

	if (input == NULL || output == NULL) {
		ALOGE("param error");
		return -1;;
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

__attribute__((visibility("hidden")))
void recg_img_dec_end(struct recg_img_output *output)
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

