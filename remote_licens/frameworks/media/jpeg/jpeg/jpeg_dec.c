/*
 * jpeg_dec.c
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#define LOG_TAG "JPEG_DEC"
#include "log/log.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <malloc.h>
#include <assert.h>

#include "oscl_mem.h"
#include "lombo_jpegdec_plugin.h"
#include "jpeg_dec.h"
#include "jpeg.h"
#include "drm_display.h"
#include "jpeg_hal.h"
#include "jpeg_rescale.h"

#define DEC_STREAM_BUF_SIZE	(1024 * 1024)

typedef struct pic_yuv_size {
	unsigned int one_frame_size;
	unsigned int luma_size;
	unsigned int chroma_size;
} pic_yuv_size_t;

static void get_buffer_info(jpeg_dec_parm_t *dec_parm,
					jpeg_dec_output_buf_t *dec_output_buf,
					pic_yuv_size_t *pic_size)
{
	unsigned int aligned_bytes, size_luma, size_sec_uv;

	/* align */
	dec_parm->width_stride = (dec_parm->width_stride + 0x1f)&(~0x1f);
	dec_parm->height_aligned = (dec_parm->height_aligned + 0x0f)&(~0x0f);

	/* count offset */
	size_luma = dec_parm->width_stride * dec_parm->height_aligned;
	aligned_bytes = (size_luma + 1023)/1024*1024 - size_luma;
	size_sec_uv = ((dec_parm->width_stride/2+31)/32*32)*dec_parm->height_aligned/2;

	/* set output offset */
	dec_output_buf->offset[0] = 0;
	dec_output_buf->offset[1] = size_luma + aligned_bytes;
	dec_output_buf->offset[2] = dec_output_buf->offset[1] + size_sec_uv;

	/* get pic size */
	pic_size->luma_size = size_luma;
	pic_size->chroma_size = size_sec_uv*2;
	pic_size->one_frame_size = size_luma + aligned_bytes + size_sec_uv*2;

}
static int jpeg_auto_scale(jpeg_dec_header_info_t *header_info, jpeg_dec_parm_t *dec_parm,
			unsigned int unlimited)
{
	float  image_width, image_height, dest_width, dest_height;
	float  a, b, scale_denom;
	int i = 0;
	image_width = header_info->width_stride;
	image_height = header_info->height_aligned;
	dest_width = dec_parm->width_stride;
	dest_height = dec_parm->height_aligned;
	dec_parm->down_scale_ratio = 0;
	if (!unlimited) {
		if (image_width >= dest_width || image_height >= dest_height) {
			a = image_width/dest_width;
			b = image_height/dest_height;
			scale_denom = (a > b) ? a : b;

			for (i = 0; i < 2; i++) {
				if (scale_denom <= (1<<i))
						break;
			}
		}
	} else {
		if (image_width >= dest_width && image_height >= dest_height) {
			a = image_width/dest_width;
			b = image_height/dest_height;
			scale_denom = (int)((a > b) ? b : a);
			for (i = 0; i < 2; i++) {
				if (scale_denom <= (1<<i))
						break;
			}
		}
	}

	dec_parm->down_scale_ratio = i;
	return 0;
}

static int get_fb_id(int drm_fd,
				void *image_virt_addr,
				int image_format,
				int size[3],
				int aligned_width,
				int aligned_height)
{
	int ret = 0;
	unsigned int dmabuf_fd = 0;
	int w = aligned_width;
	int h = aligned_height;
	int format = image_format;
	int bpp;
	uint32_t bo_handle = 0;
	unsigned int fb_handle = 0;
	size_t pitch;
	unsigned int offsets[4] = { 0 };
	unsigned int pitches[4] = { 0 };
	unsigned int bo_handles[4] = { bo_handle };
	struct drm_gem_close drm_arg;

	ALOGD("format = %.4s", (char *)&format);
	ALOGD("aligned_width = %d", aligned_width);
	ALOGD("aligned_height = %d", aligned_height);

	switch (format) {
	case DRM_FORMAT_NV12:
	case DRM_FORMAT_NV21:
	case DRM_FORMAT_NV16:
	case DRM_FORMAT_NV61:
	case DRM_FORMAT_YUV420:
	case DRM_FORMAT_YVU420:
		bpp = 8;  /* 1byte per pixel */
		break;

	case DRM_FORMAT_ARGB4444:
	case DRM_FORMAT_XRGB4444:
	case DRM_FORMAT_ABGR4444:
	case DRM_FORMAT_XBGR4444:
	case DRM_FORMAT_RGBA4444:
	case DRM_FORMAT_RGBX4444:
	case DRM_FORMAT_BGRA4444:
	case DRM_FORMAT_BGRX4444:
	case DRM_FORMAT_ARGB1555:
	case DRM_FORMAT_XRGB1555:
	case DRM_FORMAT_ABGR1555:
	case DRM_FORMAT_XBGR1555:
	case DRM_FORMAT_RGBA5551:
	case DRM_FORMAT_RGBX5551:
	case DRM_FORMAT_BGRA5551:
	case DRM_FORMAT_BGRX5551:
	case DRM_FORMAT_RGB565:
	case DRM_FORMAT_BGR565:
	case DRM_FORMAT_UYVY:
	case DRM_FORMAT_VYUY:
	case DRM_FORMAT_YUYV:
	case DRM_FORMAT_YVYU:
		bpp = 16;
		break;

	case DRM_FORMAT_BGR888:
	case DRM_FORMAT_RGB888:
		bpp = 24;
		break;

	case DRM_FORMAT_ARGB8888:
	case DRM_FORMAT_XRGB8888:
	case DRM_FORMAT_ABGR8888:
	case DRM_FORMAT_XBGR8888:
	case DRM_FORMAT_RGBA8888:
	case DRM_FORMAT_RGBX8888:
	case DRM_FORMAT_BGRA8888:
	case DRM_FORMAT_BGRX8888:
	case DRM_FORMAT_ARGB2101010:
	case DRM_FORMAT_XRGB2101010:
	case DRM_FORMAT_ABGR2101010:
	case DRM_FORMAT_XBGR2101010:
	case DRM_FORMAT_RGBA1010102:
	case DRM_FORMAT_RGBX1010102:
	case DRM_FORMAT_BGRA1010102:
	case DRM_FORMAT_BGRX1010102:
		bpp = 32;
		break;

	default:
		ALOGE("unsupported format 0x%08x",  format);
		return -1;
	}
	pitch = w * ((bpp + 7) / 8);
	/* vir_size = pitch * virtual_height; */

	dmabuf_fd = oscl_vir_to_fd(image_virt_addr);
	ALOGD("image_virt_addr = %p, dmabuf_fd:%d", image_virt_addr, dmabuf_fd);

	ret = drmPrimeFDToHandle(drm_fd, dmabuf_fd, &bo_handle);
	if (ret != 0) {
		ALOGE("drmPrimeFDToHandle failed %d: %s\n", ret, strerror(errno));
		return -1;
	}

	/* just testing a limited # of formats to test single
	 * and multi-planar path.. would be nice to add more..
	 */
	offsets[0] = 0;
	offsets[1] = size[0];
	offsets[2] = size[0] + size[1];
	switch (format) {
	case DRM_FORMAT_NV12:
	case DRM_FORMAT_NV21:
	case DRM_FORMAT_NV16:
	case DRM_FORMAT_NV61:
		bo_handles[0]	= bo_handle;
		pitches[0]	= pitch;
		pitches[1]	= pitches[0];
		/* be careful, cam src height(not aligned height) */
		bo_handles[1]	= bo_handle;
		break;

	case DRM_FORMAT_ARGB1555:
	case DRM_FORMAT_XRGB1555:
	case DRM_FORMAT_ABGR1555:
	case DRM_FORMAT_XBGR1555:
	case DRM_FORMAT_RGBA5551:
	case DRM_FORMAT_RGBX5551:
	case DRM_FORMAT_BGRA5551:
	case DRM_FORMAT_BGRX5551:
	case DRM_FORMAT_RGB565:
	case DRM_FORMAT_BGR565:
	case DRM_FORMAT_BGR888:
	case DRM_FORMAT_ARGB8888:
		offsets[0]	= 0;
		bo_handles[0]	= bo_handle;
		pitches[0]	= pitch;
		break;
	}

	ret = drmModeAddFB2(drm_fd, w, h, format, bo_handles,
		pitches, offsets, &fb_handle, 0);
	if (ret != 0) {
		ALOGE("add fb fail. bo_handle:%#x(%p) dmabuf_fd:%d, %s",
			bo_handle, image_virt_addr, dmabuf_fd, strerror(errno));
		return -1;
	}

	memset(&drm_arg, 0, sizeof(drm_arg));
	drm_arg.handle = bo_handle;
	drmIoctl(drm_fd, DRM_IOCTL_GEM_CLOSE, (void *)&drm_arg);

	return (0 == ret ? fb_handle : -1);
}

void *jpeg_fast_dec_init()
{
	return hal_jpeg_dec_init();
}

int jpeg_fast_dec_begin(struct jpeg_dec_input *input,
						struct jpeg_dec_output *output,
						void *handle)
{
	jpeg_dec_packet_t packet;
	jpeg_dec_parm_t dec_parm;
	jpeg_dec_output_buf_t dec_output_buf;
	jpeg_dec_header_info_t dec_header_info;
	pic_yuv_size_t pic_size;
	unsigned int scale_width, scale_height, custom_width, custom_height;
	unsigned int unlimited_enable;
	FILE *fp = NULL;
	int file_len, count, ret = -1;

	packet.vir_addr = NULL;
	dec_output_buf.vir_addr = NULL;
	custom_width = 0;
	custom_height = 0;


	/* check parameters */
	if (input == NULL || output == NULL) {
		ALOGE("param error");
		goto exit;
	}

	if (input->format < 0 || input->format >= JPEG_COLOR_MAX) {
		ALOGE("output format(%d) error", input->format);
		goto exit;
	}

	if (input->scale < 0 || input->scale >= JPEG_SCALE_MAX) {
		ALOGE("scale value(%d) error", input->scale);
		goto exit;
	}

	if (input->width_stride < 0) {
		ALOGE("width_stride(%d) error", input->width_stride);
		goto exit;
	}

	if (input->width_stride != 0 && input->height_stride < 0) {
		ALOGE("height_stride(%d) error", input->height_stride);
		goto exit;
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
	hal_jpeg_dec_read_header(handle, &packet, &dec_header_info);

	if (dec_header_info.width_stride > 8160 ||
		dec_header_info.height_aligned > 8160) {
		ALOGE("Unsupported size %dx%d", dec_header_info.width_stride,
		dec_header_info.height_aligned);
		goto exit;
	}

	/* get default param from header infomation */
	dec_parm.crop_width = dec_header_info.crop_width;
	dec_parm.crop_height = dec_header_info.crop_height;
	dec_parm.width_stride = dec_header_info.width_stride;
	dec_parm.height_aligned = dec_header_info.height_aligned;

	/* format, scale, rotation */
	dec_parm.output_format = input->format;
	dec_parm.down_scale_ratio = input->scale;
	dec_parm.rotation_degree = dec_header_info.rotation_degree;

	/* 0 means low-level determine stride */
	if (input->width_stride != 0) {
		/* LumaStride and ChromaStride should be aligned with 32;
		ChromaStride = LumaStride if format is nv12 or nv21;
		ChromaStride = LumaStride / 2 if format is yv12 or yv21 */
		if (input->width_stride & 0x1f) {
			dec_parm.width_stride = input->width_stride;
			dec_parm.width_stride += 31;
			dec_parm.width_stride &= ~0x1f;
			ALOGW("luma stride(%d) is not aligned with 32, align to %d",
					input->width_stride, dec_parm.width_stride);
		} else
			dec_parm.width_stride = input->width_stride;
		custom_width = input->width_stride;
	} else
		if (input->scale <= JPEG_SCALEDOWN_2)
			dec_parm.width_stride = dec_parm.width_stride >> input->scale;

	if (input->height_stride != 0) {
		dec_parm.height_aligned = input->height_stride;
		dec_parm.height_aligned = (dec_parm.height_aligned + 0x0f)&(~0x0f);
		custom_height = input->height_stride;
	} else if (input->scale <= JPEG_SCALEDOWN_2)
			dec_parm.height_aligned = dec_parm.height_aligned >> input->scale;

	if (input->width_stride != 0 || input->height_stride != 0) {
		if (dec_parm.rotation_degree == 90 ||
			dec_parm.rotation_degree == 270) {
			scale_width  = dec_parm.height_aligned;
			scale_height = dec_parm.width_stride;
			dec_parm.width_stride = scale_width;
			dec_parm.height_aligned = scale_height;
		}
	}

	if (input->scale == JPEG_SCALE_UNLIM)
		unlimited_enable = 1;
	else
		unlimited_enable = 0;

	/* auto scale */
	if (input->scale >= JPEG_SCALE_AUTO) {
		ret = jpeg_auto_scale(&dec_header_info, &dec_parm, unlimited_enable);
		if (ret != 0)
			goto exit;
	}

	if (input->width_stride != 0 || input->height_stride != 0) {
		scale_width = dec_header_info.width_stride
							>> dec_parm.down_scale_ratio;
		scale_height = dec_header_info.height_aligned
							>> dec_parm.down_scale_ratio;
		if ((scale_width  > dec_parm.width_stride &&
		scale_height  > dec_parm.height_aligned) ||
		(input->scale == JPEG_SCALE_UNLIM)) {
			dec_parm.width_stride = scale_width;
			dec_parm.height_aligned = scale_height;
		}
	}

	/* get buffer offset and yuv size */
	get_buffer_info(&dec_parm, &dec_output_buf, &pic_size);


	ALOGD("dec_parm height_aligned:%d, width_stride:%d", dec_parm.height_aligned,
						dec_parm.width_stride);

	if (dec_header_info.picture_type == JPEG_ERROR) {
		ALOGE("packet tpye error");
		goto exit;
	}

	/* alloc memory for output buffer */
	dec_output_buf.vir_addr = (unsigned char *)oscl_malloc_cached(
							pic_size.one_frame_size);
	if (dec_output_buf.vir_addr == NULL) {
		ALOGE("alloc dec buf failed");
		goto exit;
	}
	dec_output_buf.phy_addr = oscl_virt_to_phys(dec_output_buf.vir_addr);
	oscl_cache_flush(dec_output_buf.phy_addr);

	ALOGD("one_frame_size:%d", pic_size.one_frame_size);

	ret = hal_jpeg_dec_frame(handle, &packet, &dec_output_buf, &dec_parm);
	if (ret != 0) {
		ALOGE("decode frame error\n");
		/* free output buffer */
		oscl_free_cached(dec_output_buf.vir_addr);
		goto exit;
	}

	if (dec_output_buf.planenum == 1)
		memset(dec_output_buf.vir_addr + dec_output_buf.offset[1],
				0x80, pic_size.chroma_size);

	oscl_cache_flush(dec_output_buf.phy_addr);

	if (input->scale == JPEG_SCALEDOWN_0) {
		custom_width = dec_parm.width_stride;
		custom_height = dec_parm.height_aligned;
	}

	ret = jpeg_output_adjust(&dec_output_buf, &dec_parm,
			custom_width, custom_height, 0, 0, unlimited_enable);
	if (ret != 0) {
		ALOGE("rescale frame error\n");
		/* free output buffer */
		oscl_free_cached(dec_output_buf.vir_addr);
		goto exit;
	}

	output->addr[0] = dec_output_buf.vir_addr;
	output->phy_addr[0] = dec_output_buf.phy_addr;
	output->actual_width = dec_parm.crop_width;
	output->actual_height = dec_parm.crop_height;
	output->encode_width = dec_header_info.width_stride;
	output->encode_height = dec_header_info.height_aligned;
	output->stride_width = dec_parm.width_stride;
	output->stride_height = dec_parm.height_aligned;

	pic_size.luma_size = dec_parm.width_stride*dec_parm.height_aligned;
	pic_size.chroma_size = pic_size.luma_size/2;

	output->size[0] = pic_size.luma_size;
	output->addr[1] = output->addr[0] + dec_output_buf.offset[1];
	output->phy_addr[1] = output->phy_addr[0] + dec_output_buf.offset[1];
	if (dec_parm.output_format < JPEG_NV12) {
		output->addr[2] = output->addr[0] + dec_output_buf.offset[2];
		output->phy_addr[2] = output->phy_addr[0] + dec_output_buf.offset[2];
		output->size[1] = output->size[2] = pic_size.chroma_size/2;
	} else{
		output->addr[2] = NULL;
		output->phy_addr[2] = 0;
		output->size[1] = pic_size.chroma_size;
		output->size[2] = 0;
	}

	ALOGD("actual w:%d, h:%d, encode w:%d, h:%d, stride w:%d, h:%d",
			output->actual_width, output->actual_height,
			output->encode_width, output->encode_height,
			output->stride_width, output->stride_height);
	ALOGD("luma size:%d, chroma offset:%ld",
					output->stride_width * output->stride_height,
					(long)(output->addr[1] - output->addr[0]));
exit:
	if (packet.vir_addr)
		free(packet.vir_addr);
	if (fp)
		fclose(fp);


	return ret;
}

int jpeg_fast_dec_data(char *jpeg_data, int data_len, int width_stride,
			int height_stride, enum jpeg_scale scale_mode,
			struct jpeg_dec_output *output, void *handle)
{
	jpeg_dec_packet_t packet;
	jpeg_dec_parm_t dec_parm;
	jpeg_dec_output_buf_t dec_output_buf;
	jpeg_dec_header_info_t dec_header_info;
	pic_yuv_size_t pic_size;
	unsigned int scale_width, scale_height, custom_width, custom_height;
	unsigned int unlimited_enable;
	FILE *fp = NULL;
	int ret = -1;

	packet.vir_addr = NULL;
	dec_output_buf.vir_addr = NULL;
	custom_width = 0;
	custom_height = 0;


	/* check parameters */
	if (jpeg_data == NULL || output == NULL) {
		ALOGE("param error");
		goto exit;
	}

	packet.stream_len = data_len;
	packet.vir_addr = (unsigned char *)malloc(packet.stream_len);
	if (packet.vir_addr == NULL) {
		ALOGE("alloc mem for file failed");
		goto exit;
	}

	memcpy(packet.vir_addr, jpeg_data, data_len);

	/* dec read header infomation */
	hal_jpeg_dec_read_header(handle, &packet, &dec_header_info);

	if (dec_header_info.width_stride > 8160 ||
		dec_header_info.height_aligned > 8160) {
		ALOGE("Unsupported size %dx%d", dec_header_info.width_stride,
		dec_header_info.height_aligned);
		goto exit;
	}

	/* get default param from header infomation */
	dec_parm.crop_width = dec_header_info.crop_width;
	dec_parm.crop_height = dec_header_info.crop_height;
	dec_parm.width_stride = dec_header_info.width_stride;
	dec_parm.height_aligned = dec_header_info.height_aligned;

	/* format, scale, rotation */
	dec_parm.output_format = JPEG_COLOR_NV12;
	dec_parm.down_scale_ratio = scale_mode;
	dec_parm.rotation_degree = dec_header_info.rotation_degree;

	/* 0 means low-level determine stride */
	if (width_stride != 0) {
		/* LumaStride and ChromaStride should be aligned with 32;
		ChromaStride = LumaStride if format is nv12 or nv21;
		ChromaStride = LumaStride / 2 if format is yv12 or yv21 */
		if (width_stride & 0x1f) {
			dec_parm.width_stride = width_stride;
			dec_parm.width_stride += 31;
			dec_parm.width_stride &= ~0x1f;
			ALOGW("luma stride(%d) is not aligned with 32, align to %d",
					width_stride, dec_parm.width_stride);
		} else
			dec_parm.width_stride = width_stride;
		custom_width = width_stride;
	} else
		if (scale_mode <= JPEG_SCALEDOWN_2)
			dec_parm.width_stride = dec_parm.width_stride >> scale_mode;

	if (height_stride != 0) {
		dec_parm.height_aligned = height_stride;
		dec_parm.height_aligned = (dec_parm.height_aligned + 0x0f)&(~0x0f);
		custom_height = height_stride;
	} else if (scale_mode <= JPEG_SCALEDOWN_2)
			dec_parm.height_aligned = dec_parm.height_aligned >> scale_mode;

	if (width_stride != 0 || height_stride != 0) {
		if (dec_parm.rotation_degree == 90 ||
			dec_parm.rotation_degree == 270) {
			scale_width  = dec_parm.height_aligned;
			scale_height = dec_parm.width_stride;
			dec_parm.width_stride = scale_width;
			dec_parm.height_aligned = scale_height;
		}
	}

	if (scale_mode == JPEG_SCALE_UNLIM)
		unlimited_enable = 1;
	else
		unlimited_enable = 0;

	/* auto scale */
	if (scale_mode >= JPEG_SCALE_AUTO) {
		ret = jpeg_auto_scale(&dec_header_info, &dec_parm, unlimited_enable);
		if (ret != 0)
			goto exit;
	}

	if (width_stride != 0 || height_stride != 0) {
		scale_width = dec_header_info.width_stride
							>> dec_parm.down_scale_ratio;
		scale_height = dec_header_info.height_aligned
							>> dec_parm.down_scale_ratio;
		if ((scale_width  > dec_parm.width_stride &&
		scale_height  > dec_parm.height_aligned) ||
		(scale_mode == JPEG_SCALE_UNLIM)) {
			dec_parm.width_stride = scale_width;
			dec_parm.height_aligned = scale_height;
		}
	}

	/* get buffer offset and yuv size */
	get_buffer_info(&dec_parm, &dec_output_buf, &pic_size);

	ALOGD("dec_parm height_aligned:%d, width_stride:%d", dec_parm.height_aligned,
						dec_parm.width_stride);

	if (dec_header_info.picture_type == JPEG_ERROR) {
		ALOGE("packet tpye error");
		goto exit;
	}

	/* alloc memory for output buffer */
	dec_output_buf.vir_addr = (unsigned char *)oscl_malloc_cached(
							pic_size.one_frame_size);
	if (dec_output_buf.vir_addr == NULL) {
		ALOGE("alloc dec buf failed");
		goto exit;
	}
	dec_output_buf.phy_addr = oscl_virt_to_phys(dec_output_buf.vir_addr);
	oscl_cache_flush(dec_output_buf.phy_addr);

	ALOGD("one_frame_size:%d", pic_size.one_frame_size);

	ret = hal_jpeg_dec_frame(handle, &packet, &dec_output_buf, &dec_parm);
	if (ret != 0) {
		ALOGE("decode frame error\n");
		/* free output buffer */
		oscl_free_cached(dec_output_buf.vir_addr);
		goto exit;
	}

	if (dec_output_buf.planenum == 1)
		memset(dec_output_buf.vir_addr + dec_output_buf.offset[1],
				0x80, pic_size.chroma_size);

	oscl_cache_flush(dec_output_buf.phy_addr);

	if (scale_mode == JPEG_SCALEDOWN_0) {
		custom_width = dec_parm.width_stride;
		custom_height = dec_parm.height_aligned;
	}

	ret = jpeg_output_adjust(&dec_output_buf, &dec_parm,
			custom_width, custom_height, 0, 0, unlimited_enable);
	if (ret != 0) {
		ALOGE("rescale frame error\n");
		/* free output buffer */
		oscl_free_cached(dec_output_buf.vir_addr);
		goto exit;
	}

	output->addr[0] = dec_output_buf.vir_addr;
	output->phy_addr[0] = dec_output_buf.phy_addr;
	output->actual_width = dec_parm.crop_width;
	output->actual_height = dec_parm.crop_height;
	output->encode_width = dec_header_info.width_stride;
	output->encode_height = dec_header_info.height_aligned;
	output->stride_width = dec_parm.width_stride;
	output->stride_height = dec_parm.height_aligned;

	pic_size.luma_size = dec_parm.width_stride*dec_parm.height_aligned;
	pic_size.chroma_size = pic_size.luma_size/2;

	output->size[0] = pic_size.luma_size;
	output->addr[1] = output->addr[0] + dec_output_buf.offset[1];
	output->phy_addr[1] = output->phy_addr[0] + dec_output_buf.offset[1];
	if (dec_parm.output_format < JPEG_NV12) {
		output->addr[2] = output->addr[0] + dec_output_buf.offset[2];
		output->phy_addr[2] = output->phy_addr[0] + dec_output_buf.offset[2];
		output->size[1] = output->size[2] = pic_size.chroma_size/2;
	} else{
		output->addr[2] = NULL;
		output->phy_addr[2] = 0;
		output->size[1] = pic_size.chroma_size;
		output->size[2] = 0;
	}

	ALOGD("actual w:%d, h:%d, encode w:%d, h:%d, stride w:%d, h:%d",
			output->actual_width, output->actual_height,
			output->encode_width, output->encode_height,
			output->stride_width, output->stride_height);
	ALOGD("luma size:%d, chroma offset:%ld",
					output->stride_width * output->stride_height,
					(long)(output->addr[1] - output->addr[0]));
exit:
	if (packet.vir_addr)
		free(packet.vir_addr);
	if (fp)
		fclose(fp);


	return ret;
}
void jpeg_fast_dec_end(struct jpeg_dec_output *output)
{
	jpeg_dec_end(output);
}

int jpeg_fast_dec_deinit(void *handle)
{
	return hal_jpeg_dec_deinit(handle);
}

int jpeg_fast_read_header(void *handle, char *jpeg_data, int data_len, int *width_stride,
			int *height_aligned)
{
	jpeg_dec_packet_t packet;
	jpeg_dec_header_info_t dec_header_info;
	int ret;

	if (handle == NULL || jpeg_data == NULL ||
		data_len == 0 || width_stride == NULL || height_aligned == NULL) {
		ALOGE("parameter error\n");
			return -1;
	}

	packet.vir_addr = (unsigned char *)jpeg_data;
	packet.stream_len = data_len;

	*width_stride = 0;
	*height_aligned = 0;

	ret = hal_jpeg_dec_read_header(handle, &packet, &dec_header_info);
	if (ret != 0) {
		ALOGE("read header error");
		return -1;
	}

	if (dec_header_info.rotation_degree == 90 ||
				dec_header_info.rotation_degree == 270) {
		*width_stride = dec_header_info.height_aligned;
		*height_aligned = dec_header_info.width_stride;
	} else{
		*width_stride = dec_header_info.width_stride;
		*height_aligned = dec_header_info.height_aligned;
	}

	return 0;
}
int jpeg_fast_read_file_header(void *handle, char *file_name, int *width_stride,
			int *height_aligned)
{
	FILE *fp = NULL;
	int file_len, count, ret = -1;
	char *jpeg_data = NULL;

	if (handle == NULL || file_name == NULL
		|| width_stride == NULL || height_aligned == NULL) {
		ALOGE("parameter error\n");
			return -1;
	}
	*width_stride = 0;
	*height_aligned = 0;

	fp = fopen((const char *)file_name, "rb");
	if (fp == NULL) {
		ALOGE("open %s failed", file_name);
		return -1;
	}

	/* get file length */
	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);
	if (file_len <= 0) {
		ALOGE("get file length error");
		goto exit;
	}
	fseek(fp, 0, SEEK_SET);

	jpeg_data = (char *)malloc(file_len);
	if (jpeg_data == NULL) {
		ALOGE("alloc mem for file failed");
		goto exit;
	}

	count = fread(jpeg_data, 1, file_len, fp);
	if (count != file_len) {
		ALOGE("read file failed, count:%d, file_len:%d", count, file_len);
		goto exit;
	}

	ret = jpeg_fast_read_header(handle, jpeg_data, file_len,
					width_stride, height_aligned);

exit:
	if (jpeg_data)
		free(jpeg_data);
	if (fp)
		fclose(fp);

	return ret;
}

int jpeg_read_data_header(char *jpeg_data, int data_len, int *width_stride,
			int *height_aligned)
{
	int ret;
	void *handle = NULL;

	handle = hal_jpeg_dec_init();
	if (handle == NULL) {
		ALOGE("jpeg dec init failed");
		return -1;
	}

	ret = jpeg_fast_read_header(handle, jpeg_data, data_len,
					width_stride, height_aligned);

	if (handle)
		hal_jpeg_dec_deinit(handle);

	return ret;
}

int jpeg_read_file_header(char *file_name, int *width_stride,
			int *height_aligned)
{
	int ret;
	void *handle = NULL;

	handle = hal_jpeg_dec_init();
	if (handle == NULL) {
		ALOGE("jpeg dec init failed");
		return -1;
	}

	ret = jpeg_fast_read_file_header(handle, file_name, width_stride, height_aligned);

	if (handle)
		hal_jpeg_dec_deinit(handle);
	return ret;
}

int jpeg_dec_begin(struct jpeg_dec_input *input, struct jpeg_dec_output *output)
{
	jpeg_dec_packet_t packet;
	jpeg_dec_parm_t dec_parm;
	jpeg_dec_output_buf_t dec_output_buf;
	jpeg_dec_header_info_t dec_header_info;
	pic_yuv_size_t pic_size;
	FILE *fp = NULL;
	void *handle = NULL;
	int file_len, count, ret = -1;
	unsigned int scale_width, scale_height, custom_width, custom_height;
	unsigned int unlimited_enable;
	packet.vir_addr = NULL;
	dec_output_buf.vir_addr = NULL;
	custom_width = 0;
	custom_height = 0;

	jpeg_lock();

	/* check parameters */
	if (input == NULL || output == NULL) {
		ALOGE("param error");
		goto exit;
	}

	if (input->format < 0 || input->format >= JPEG_COLOR_MAX) {
		ALOGE("output format(%d) error", input->format);
		goto exit;
	}

	if (input->scale < 0 || input->scale >= JPEG_SCALE_MAX) {
		ALOGE("scale value(%d) error", input->scale);
		goto exit;
	}

	if (input->width_stride < 0) {
		ALOGE("width_stride(%d) error", input->width_stride);
		goto exit;
	}

	if (input->width_stride != 0 && input->height_stride < 0) {
		ALOGE("height_stride(%d) error", input->height_stride);
		goto exit;
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

	count = fread(packet.vir_addr, 1, file_len, fp);
	if (count != file_len) {
		ALOGE("read file failed, count:%d, file_len:%d", count, file_len);
		goto exit;
	}

	handle = hal_jpeg_dec_init();
	if (handle == NULL) {
		ALOGE("jpeg dec init failed");
		goto exit;
	}

	/* dec read header infomation */
	hal_jpeg_dec_read_header(handle, &packet, &dec_header_info);

	if (dec_header_info.width_stride > 8160 ||
		dec_header_info.height_aligned > 8160) {
		ALOGE("Unsupported size %dx%d", dec_header_info.width_stride,
		dec_header_info.height_aligned);
		goto exit;
	}

	/* get default param from header infomation */
	dec_parm.crop_width = dec_header_info.crop_width;
	dec_parm.crop_height = dec_header_info.crop_height;
	dec_parm.width_stride = dec_header_info.width_stride;
	dec_parm.height_aligned = dec_header_info.height_aligned;

	/* format, scale, rotation */
	dec_parm.output_format = input->format;
	dec_parm.down_scale_ratio = input->scale;
	dec_parm.rotation_degree = dec_header_info.rotation_degree;

	/* 0 means low-level determine stride */
	if (input->width_stride != 0) {
		/* LumaStride and ChromaStride should be aligned with 32;
		ChromaStride = LumaStride if format is nv12 or nv21;
		ChromaStride = LumaStride / 2 if format is yv12 or yv21 */
		if (input->width_stride & 0x1f) {
			dec_parm.width_stride = input->width_stride;
			dec_parm.width_stride += 31;
			dec_parm.width_stride &= ~0x1f;
			ALOGW("luma stride(%d) is not aligned with 32, align to %d",
					input->width_stride, dec_parm.width_stride);
		} else
			dec_parm.width_stride = input->width_stride;
		custom_width = input->width_stride;
	} else
		if (input->scale <= JPEG_SCALEDOWN_2)
			dec_parm.width_stride = dec_parm.width_stride >> input->scale;

	if (input->height_stride != 0) {
		dec_parm.height_aligned = input->height_stride;
		dec_parm.height_aligned = (dec_parm.height_aligned + 0x0f)&(~0x0f);
		custom_height = input->height_stride;
	} else if (input->scale <= JPEG_SCALEDOWN_2)
			dec_parm.height_aligned = dec_parm.height_aligned >> input->scale;

	if (input->width_stride != 0 || input->height_stride != 0) {
		if (dec_parm.rotation_degree == 90 ||
			dec_parm.rotation_degree == 270) {
			scale_width  = dec_parm.height_aligned;
			scale_height = dec_parm.width_stride;
			dec_parm.width_stride = scale_width;
			dec_parm.height_aligned = scale_height;
		}
	}

	if (input->scale == JPEG_SCALE_UNLIM)
		unlimited_enable = 1;
	else
		unlimited_enable = 0;

	/* auto scale */
	if (input->scale >= JPEG_SCALE_AUTO) {
		ret = jpeg_auto_scale(&dec_header_info, &dec_parm, unlimited_enable);
		if (ret != 0)
			goto exit;
	}

	if (input->width_stride != 0 || input->height_stride != 0) {
		scale_width = dec_header_info.width_stride
							>> dec_parm.down_scale_ratio;
		scale_height = dec_header_info.height_aligned
							>> dec_parm.down_scale_ratio;
		if ((scale_width  > dec_parm.width_stride &&
		scale_height  > dec_parm.height_aligned) ||
		(input->scale == JPEG_SCALE_UNLIM)) {
			dec_parm.width_stride = scale_width;
			dec_parm.height_aligned = scale_height;
		}
	}

	/* get buffer offset and yuv size */
	get_buffer_info(&dec_parm, &dec_output_buf, &pic_size);



	ALOGD("dec_parm height_aligned:%d, width_stride:%d", dec_parm.height_aligned,
						dec_parm.width_stride);

	if (dec_header_info.picture_type == JPEG_ERROR) {
		ALOGE("packet tpye error");
		goto exit;
	}

	/* alloc memory for output buffer */
	dec_output_buf.vir_addr = (unsigned char *)oscl_malloc_cached(
							pic_size.one_frame_size);
	if (dec_output_buf.vir_addr == NULL) {
		ALOGE("alloc dec buf failed");
		goto exit;
	}
	dec_output_buf.phy_addr = oscl_virt_to_phys(dec_output_buf.vir_addr);
	oscl_cache_flush(dec_output_buf.phy_addr);

	ALOGD("one_frame_size:%d", pic_size.one_frame_size);

	ret = hal_jpeg_dec_frame(handle, &packet, &dec_output_buf, &dec_parm);
	if (ret != 0) {
		ALOGE("decode frame error\n");
		/* free output buffer */
		oscl_free_cached(dec_output_buf.vir_addr);
		goto exit;
	}

	if (dec_output_buf.planenum == 1)
		memset(dec_output_buf.vir_addr + dec_output_buf.offset[1],
				0x80, pic_size.chroma_size);

	oscl_cache_flush(dec_output_buf.phy_addr);

	if (input->scale == JPEG_SCALEDOWN_0) {
		custom_width = dec_parm.width_stride;
		custom_height = dec_parm.height_aligned;
	}

	ret = jpeg_output_adjust(&dec_output_buf, &dec_parm,
			custom_width, custom_height, 0, 0, unlimited_enable);
	if (ret != 0) {
		ALOGE("rescale frame error\n");
		/* free output buffer */
		oscl_free_cached(dec_output_buf.vir_addr);
		goto exit;
	}

	output->addr[0] = dec_output_buf.vir_addr;
	output->phy_addr[0] = dec_output_buf.phy_addr;
	output->actual_width = dec_parm.crop_width;
	output->actual_height = dec_parm.crop_height;
	output->encode_width = dec_header_info.width_stride;
	output->encode_height = dec_header_info.height_aligned;
	output->stride_width = dec_parm.width_stride;
	output->stride_height = dec_parm.height_aligned;

	pic_size.luma_size = dec_parm.width_stride*dec_parm.height_aligned;
	pic_size.chroma_size = pic_size.luma_size/2;

	output->size[0] = pic_size.luma_size;
	output->addr[1] = output->addr[0] + dec_output_buf.offset[1];
	output->phy_addr[1] = output->phy_addr[0] + dec_output_buf.offset[1];
	if (dec_parm.output_format < JPEG_NV12) {
		output->addr[2] = output->addr[0] + dec_output_buf.offset[2];
		output->phy_addr[2] = output->phy_addr[0] + dec_output_buf.offset[2];
		output->size[1] = output->size[2] = pic_size.chroma_size/2;
	} else{
		output->addr[2] = NULL;
		output->phy_addr[2] = 0;
		output->size[1] = pic_size.chroma_size;
		output->size[2] = 0;
	}

	ALOGD("actual w:%d, h:%d, encode w:%d, h:%d, stride w:%d, h:%d",
			output->actual_width, output->actual_height,
			output->encode_width, output->encode_height,
			output->stride_width, output->stride_height);
	ALOGD("luma size:%d, chroma offset:%ld",
					output->stride_width * output->stride_height,
					(long)(output->addr[1] - output->addr[0]));
exit:
	if (handle)
		hal_jpeg_dec_deinit(handle);
	if (packet.vir_addr)
		free(packet.vir_addr);
	if (fp)
		fclose(fp);

	jpeg_unlock();

	return ret;
}

int jpeg_dec_data(char *jpeg_data, int data_len, int width_stride,
			int height_stride, enum jpeg_scale scale_mode,
			struct jpeg_dec_output *output)
{
	jpeg_dec_packet_t packet;
	jpeg_dec_parm_t dec_parm;
	jpeg_dec_output_buf_t dec_output_buf;
	jpeg_dec_header_info_t dec_header_info;
	pic_yuv_size_t pic_size;
	unsigned int scale_width, scale_height, custom_width, custom_height;
	unsigned int unlimited_enable;
	FILE *fp = NULL;
	void *handle = NULL;
	int ret = -1;

	packet.vir_addr = NULL;
	dec_output_buf.vir_addr = NULL;
	custom_width = 0;
	custom_height = 0;

	jpeg_lock();

	/* check parameters */
	if (jpeg_data == NULL || output == NULL) {
		ALOGE("param error");
		goto exit;
	}

	packet.stream_len = data_len;
	packet.vir_addr = (unsigned char *)malloc(packet.stream_len);
	if (packet.vir_addr == NULL) {
		ALOGE("alloc mem for file failed");
		goto exit;
	}

	memcpy(packet.vir_addr, jpeg_data, data_len);

	handle = hal_jpeg_dec_init();
	if (handle == NULL) {
		ALOGE("jpeg dec init failed");
		goto exit;
	}

	/* dec read header infomation */
	hal_jpeg_dec_read_header(handle, &packet, &dec_header_info);

	if (dec_header_info.width_stride > 8160 ||
		dec_header_info.height_aligned > 8160) {
		ALOGE("Unsupported size %dx%d", dec_header_info.width_stride,
		dec_header_info.height_aligned);
		goto exit;
	}

	/* get default param from header infomation */
	dec_parm.crop_width = dec_header_info.crop_width;
	dec_parm.crop_height = dec_header_info.crop_height;
	dec_parm.width_stride = dec_header_info.width_stride;
	dec_parm.height_aligned = dec_header_info.height_aligned;

	/* format, scale, rotation */
	dec_parm.output_format = JPEG_COLOR_NV12;
	dec_parm.down_scale_ratio = scale_mode;
	dec_parm.rotation_degree = dec_header_info.rotation_degree;

	/* 0 means low-level determine stride */
	if (width_stride != 0) {
		/* LumaStride and ChromaStride should be aligned with 32;
		ChromaStride = LumaStride if format is nv12 or nv21;
		ChromaStride = LumaStride / 2 if format is yv12 or yv21 */
		if (width_stride & 0x1f) {
			dec_parm.width_stride = width_stride;
			dec_parm.width_stride += 31;
			dec_parm.width_stride &= ~0x1f;
			ALOGW("luma stride(%d) is not aligned with 32, align to %d",
					width_stride, dec_parm.width_stride);
		} else
			dec_parm.width_stride = width_stride;
		custom_width = width_stride;
	} else
		if (scale_mode <= JPEG_SCALEDOWN_2)
			dec_parm.width_stride = dec_parm.width_stride >> scale_mode;

	if (height_stride != 0) {
		dec_parm.height_aligned = height_stride;
		dec_parm.height_aligned = (dec_parm.height_aligned + 0x0f)&(~0x0f);
		custom_height = height_stride;
	} else if (scale_mode <= JPEG_SCALEDOWN_2)
			dec_parm.height_aligned = dec_parm.height_aligned >> scale_mode;

	if (width_stride != 0 || height_stride != 0) {
		if (dec_parm.rotation_degree == 90 ||
			dec_parm.rotation_degree == 270) {
			scale_width  = dec_parm.height_aligned;
			scale_height = dec_parm.width_stride;
			dec_parm.width_stride = scale_width;
			dec_parm.height_aligned = scale_height;
		}
	}

	if (scale_mode == JPEG_SCALE_UNLIM)
		unlimited_enable = 1;
	else
		unlimited_enable = 0;

	/* auto scale */
	if (scale_mode >= JPEG_SCALE_AUTO) {
		ret = jpeg_auto_scale(&dec_header_info, &dec_parm, unlimited_enable);
		if (ret != 0)
			goto exit;
	}

	if (width_stride != 0 || height_stride != 0) {
		scale_width = dec_header_info.width_stride
							>> dec_parm.down_scale_ratio;
		scale_height = dec_header_info.height_aligned
							>> dec_parm.down_scale_ratio;
		if ((scale_width  > dec_parm.width_stride &&
		scale_height  > dec_parm.height_aligned) ||
		(scale_mode == JPEG_SCALE_UNLIM)) {
			dec_parm.width_stride = scale_width;
			dec_parm.height_aligned = scale_height;
		}
	}

	/* get buffer offset and yuv size */
	get_buffer_info(&dec_parm, &dec_output_buf, &pic_size);

	ALOGD("dec_parm height_aligned:%d, width_stride:%d", dec_parm.height_aligned,
						dec_parm.width_stride);

	if (dec_header_info.picture_type == JPEG_ERROR) {
		ALOGE("packet tpye error");
		goto exit;
	}

	/* alloc memory for output buffer */
	dec_output_buf.vir_addr = (unsigned char *)oscl_malloc_cached(
							pic_size.one_frame_size);
	if (dec_output_buf.vir_addr == NULL) {
		ALOGE("alloc dec buf failed");
		goto exit;
	}
	dec_output_buf.phy_addr = oscl_virt_to_phys(dec_output_buf.vir_addr);
	oscl_cache_flush(dec_output_buf.phy_addr);

	ALOGD("one_frame_size:%d", pic_size.one_frame_size);

	ret = hal_jpeg_dec_frame(handle, &packet, &dec_output_buf, &dec_parm);
	if (ret != 0) {
		ALOGE("decode frame error\n");
		/* free output buffer */
		oscl_free_cached(dec_output_buf.vir_addr);
		goto exit;
	}

	if (dec_output_buf.planenum == 1)
		memset(dec_output_buf.vir_addr + dec_output_buf.offset[1],
				0x80, pic_size.chroma_size);

	oscl_cache_flush(dec_output_buf.phy_addr);

	if (scale_mode == JPEG_SCALEDOWN_0) {
		custom_width = dec_parm.width_stride;
		custom_height = dec_parm.height_aligned;
	}

	ret = jpeg_output_adjust(&dec_output_buf, &dec_parm,
			custom_width, custom_height, 0, 0, unlimited_enable);
	if (ret != 0) {
		ALOGE("rescale frame error\n");
		/* free output buffer */
		oscl_free_cached(dec_output_buf.vir_addr);
		goto exit;
	}

	output->addr[0] = dec_output_buf.vir_addr;
	output->phy_addr[0] = dec_output_buf.phy_addr;
	output->actual_width = dec_parm.crop_width;
	output->actual_height = dec_parm.crop_height;
	output->encode_width = dec_header_info.width_stride;
	output->encode_height = dec_header_info.height_aligned;
	output->stride_width = dec_parm.width_stride;
	output->stride_height = dec_parm.height_aligned;

	pic_size.luma_size = dec_parm.width_stride*dec_parm.height_aligned;
	pic_size.chroma_size = pic_size.luma_size/2;

	output->size[0] = pic_size.luma_size;
	output->addr[1] = output->addr[0] + dec_output_buf.offset[1];
	output->phy_addr[1] = output->phy_addr[0] + dec_output_buf.offset[1];
	if (dec_parm.output_format < JPEG_NV12) {
		output->addr[2] = output->addr[0] + dec_output_buf.offset[2];
		output->phy_addr[2] = output->phy_addr[0] + dec_output_buf.offset[2];
		output->size[1] = output->size[2] = pic_size.chroma_size/2;
	} else{
		output->addr[2] = NULL;
		output->phy_addr[2] = 0;
		output->size[1] = pic_size.chroma_size;
		output->size[2] = 0;
	}

	ALOGD("actual w:%d, h:%d, encode w:%d, h:%d, stride w:%d, h:%d",
			output->actual_width, output->actual_height,
			output->encode_width, output->encode_height,
			output->stride_width, output->stride_height);
	ALOGD("luma size:%d, chroma offset:%ld",
					output->stride_width * output->stride_height,
					(long)(output->addr[1]-output->addr[0]));
exit:
	if (handle)
		hal_jpeg_dec_deinit(handle);
	if (packet.vir_addr)
		free(packet.vir_addr);
	if (fp)
		fclose(fp);

	jpeg_unlock();

	return ret;
}

void jpeg_dec_end(struct jpeg_dec_output *output)
{
	if (output == NULL)
		return;

	if (output->addr[0])
		oscl_free_cached(output->addr[0]);
}

int image_scale(struct pic_scale_param *src, struct pic_scale_param *dst)
{
	const char *card = "/dev/dri/card0";
	drm_lombo_scale_user_handle_t handle = 0;
	struct drm_lombo_scale_exec_arg exec_arg;
	struct drm_lombo_scale_timeout timeout;
	int drm_fd;
	int format;
	int ret = -1;
	int in_fb_id = -1, out_fb_id = -1;

	drm_fd = drm_open_dev_dumb(card);
	if (drm_fd < 0)
		return -1;
	ret = lombo_scale_open(drm_fd, &handle);
	if (ret)
		goto EXIT;
	timeout.handle = handle;
	timeout.time_ns = 1000000000ULL; /* 1s */
	ret = lombo_scale_set_timeout(drm_fd, &timeout);
	if (ret)
		goto EXIT;

	if (src->format == IMG_FMT_NV12)
		format = DRM_FORMAT_NV12;
	else {
		ALOGE("unsupport format. %d", src->format);
		goto EXIT;
	}

	in_fb_id = get_fb_id(drm_fd, src->buf, format, src->size,
				src->stride_width, src->stride_height);
	if (in_fb_id < 0)
		goto EXIT;
	if (dst->format == IMG_FMT_NV12)
		format = DRM_FORMAT_NV12;
	else if (dst->format == IMG_FMT_ARGB8888)
		format = DRM_FORMAT_ARGB8888;
	else
		goto EXIT;
	out_fb_id = get_fb_id(drm_fd, dst->buf, format, dst->size,
				dst->stride_width, dst->stride_height);
	if (out_fb_id < 0)
		goto EXIT;
	exec_arg.handle = handle;

	exec_arg.in_fb.fb_id = in_fb_id;
	exec_arg.in_fb.crop_x = 0;
	exec_arg.in_fb.crop_y = 0;
	exec_arg.in_fb.crop_w = src->width;
	exec_arg.in_fb.crop_h = src->height;

	exec_arg.out_fb.fb_id = out_fb_id;
	exec_arg.out_fb.crop_x = 0;
	exec_arg.out_fb.crop_y = 0;
	exec_arg.out_fb.crop_w = dst->width;
	exec_arg.out_fb.crop_h = dst->height;

	ret = lombo_scale_exec(drm_fd, &exec_arg);
	if (ret) {
		ALOGE("lombo_scale_exec fail. ret=%d\n", ret);
		goto EXIT;
	}
	ret = 0;
EXIT:
	ALOGD("%s %d scale end, ret:%d.", __func__, __LINE__, ret);
	if (in_fb_id >= 0)
		drmModeRmFB(drm_fd, (uint32_t)in_fb_id);
	if (out_fb_id >= 0)
		drmModeRmFB(drm_fd, (uint32_t)out_fb_id);
	if (handle != 0)
		lombo_scale_close(drm_fd, &handle);
	if (drm_fd >= 0)
		close(drm_fd);
	 return ret;
}

int jpeg_decode(char *data, int size,	struct pic_dec_param *outparam)
{
	void *deocder = NULL;
	struct jpeg_dec_output output;
	struct pic_scale_param sacle_in;
	struct pic_scale_param scale_out;
	int data_size, ret;

	if (!outparam || (outparam->format >= IMG_FMT_MAX)) {
		ALOGE("Decode out format unspport.");
		return -1;
	}
	deocder = jpeg_fast_dec_init();
	if (!deocder) {
		ALOGE("Create jpeg decoder fail.");
		return -1;
	}

	ret = jpeg_fast_dec_data(data, size, 0, 0, JPEG_SCALEDOWN_0, &output, deocder);
	if (ret) {
		ALOGE("Jpeg decode fail.");
		goto EXIT;
	}
	memset(&sacle_in, 0, sizeof(sacle_in));
	memset(&scale_out, 0, sizeof(scale_out));
	sacle_in.format = IMG_FMT_NV12;
	sacle_in.buf = output.addr[0];
	sacle_in.bufsize = output.size[0] + output.size[1];
	sacle_in.stride_width = output.stride_width;
	sacle_in.stride_height = output.stride_height;
	sacle_in.width = output.actual_width;
	sacle_in.height = output.actual_height;
	sacle_in.size[0] = output.size[0];
	sacle_in.size[1] = output.size[1];

	outparam->out_width =
		outparam->out_width ? outparam->out_width : output.actual_width;
	outparam->out_height =
		outparam->out_height ? outparam->out_height : output.actual_height;

	scale_out.buf = outparam->buf;
	scale_out.bufsize = outparam->bufsize;
	scale_out.width = outparam->out_width;
	scale_out.height = outparam->out_height;
	scale_out.format = outparam->format;
	if (scale_out.format == IMG_FMT_NV12) {
		scale_out.stride_width = outparam->out_width;
		scale_out.stride_height = outparam->out_height;
		scale_out.size[0] =
			scale_out.stride_width * scale_out.stride_height;
		scale_out.size[1] =
			scale_out.stride_width * scale_out.stride_height >> 1;
	} else if (scale_out.format == IMG_FMT_ARGB8888) {
		scale_out.stride_width = outparam->out_width;
		scale_out.stride_height = outparam->out_height;
		scale_out.size[0] = scale_out.width * scale_out.height * 4;
	} else
		goto EXIT;
	data_size = scale_out.size[0] + scale_out.size[1];
	if (outparam->bufsize < data_size) {
		ALOGE("Decode out buffer too few.");
		goto EXIT;
	}
	if ((scale_out.width != output.stride_width) ||
		(scale_out.height != output.stride_height) ||
		(scale_out.format != IMG_FMT_NV12)) {
		ret = image_scale(&sacle_in, &scale_out);
	} else {
		memcpy(scale_out.buf, sacle_in.buf, sacle_in.bufsize);
	}
	outparam->size[0] = scale_out.size[0];
	outparam->size[1] = scale_out.size[1];
EXIT:
	jpeg_fast_dec_end(&output);
	jpeg_fast_dec_deinit(deocder);

	return ret;
}

